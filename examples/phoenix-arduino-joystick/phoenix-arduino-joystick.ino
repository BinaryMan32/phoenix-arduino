#include <HID.h>

#include "hc165.h"
#include "Potentiometer.h"
#include "HidDescriptorHelper.h"

const size_t kNumAxes = 4;
using AxesValueType = int16_t;
const AxesValueType kAxesMin = -32768;
const AxesValueType kAxesMax = 32767;

const size_t kNumButtonGroups = 3;

// Report data sent by the HID input device
struct ReportData {
  AxesValueType axes[kNumAxes];
  u8 buttons[kNumButtonGroups];
};

using namespace usb::hid;

const u8 kReportId = 1;
static const u8 sHidDescriptorData[] PROGMEM = {
  Global::UsagePage | 1, usage::Page::GenericDesktop,
  Local::Usage | 1, usage::generic_desktop::Application::Joystick,

  Main::Collection | 1, Collection::Application,
    Global::ReportID | 1, kReportId,

    /* 4 16bit axes */
    Main::Collection | 1, Collection::Physical,
      Global::UsagePage | 1, usage::Page::GenericDesktop,
      Local::Usage | 1, usage::generic_desktop::Axis::X,
      Local::Usage | 1, usage::generic_desktop::Axis::Y,
      Local::Usage | 1, usage::generic_desktop::Axis::Rz,
      Local::Usage | 1, usage::generic_desktop::Axis::Z,
      Global::LogicalMinimum | 2, 0x00, 0x80, // kAxesMin = -32768
      Global::LogicalMaximum | 2, 0xFF, 0x7F, // kAxesMax = 32767
      Global::ReportSize | 1, 16,
      Global::ReportCount | 1, 4,
      Main::Input | 1, DataBits::Variable,
    Main::EndCollection | 0,

    // 24 1 bit buttons
    Global::UsagePage | 1, usage::Page::Button,
    Local::UsageMinimum | 1, 1,
    Local::UsageMaximum | 1, 24,
    Global::LogicalMinimum | 1, 0,
    Global::LogicalMaximum | 1, 1,
    Global::ReportSize | 1, 1,
    Global::ReportCount | 1, 24,
    Main::Input | 1, DataBits::Variable,

#if 0
    /*
     * 8 way hat switch
     * Reports an angle in 45 degree increments by mapping the values 0-7
     * to the angles 0-315 degrees. Report -8 (the most negative value) if
     * the hat is in the null position.
     */
    Global::UsagePage | 1, usage::Page::GenericDesktop,
    Local::Usage | 1, usage::generic_desktop::Miscellaneous::HatSwitch,
    Global::LogicalMinimum | 1, 0,
    Global::LogicalMaximum | 1, 7,
    Global::PhysicalMinimum | 1, 0,
    Global::PhysicalMaximum | 2, 0x3b, 0x01, // 0x13b = 315
    Global::Unit | 1, 0x14, // Degrees | EnglishRotation
    Global::ReportSize | 1, 4,
    Global::ReportCount | 1, 1,
    Main::Input | 1, DataBits::Variable | DataBits::NullState,
#endif

  Main::EndCollection | 0,
};

#define POLL_DELAY_MSEC   1

const hc165_config_t hc165_configs[kNumButtonGroups] = {
  {
    .name = "handle",
    .ploadPin = 8,
    .clockDataPin = 7,
  },
  {
    .name = "thrttl",
    .ploadPin = 20,
    .clockDataPin = 21,
  },
  {
    .name = "  wing",
    .ploadPin = 15,
    .clockDataPin = 18,
  },
};

hc165_data_t hc165_data[kNumButtonGroups];

hc165_collection_t hc165_collection = {
  .config = hc165_configs,
  .data = hc165_data,
  .size = kNumButtonGroups,
};

const Potentiometer axesPotentiometers[kNumAxes] = {
  { Potentiometer::Parameters{
      .name = "X",
      .analogInputPin = A7,
      .referenceResistance = 100,
      .resistanceRange = {.min = 0, .max = 87},
      .outputRange = {.min = kAxesMin, .max = kAxesMax},
    }
  },
  { Potentiometer::Parameters{
      .name = "Y",
      .analogInputPin = A6,
      .referenceResistance = 100,
      .resistanceRange = {.min = 0, .max = 85},
      .outputRange = {.min = kAxesMin, .max = kAxesMax},
    }
  },
  { Potentiometer::Parameters{
      .name = "R",
      .analogInputPin = A1,
      .referenceResistance = 100,
      .resistanceRange = {.min = 0, .max = 94},
      .outputRange = {.min = kAxesMin, .max = kAxesMax},
    }
  },
  { Potentiometer::Parameters{
      .name = "T",
      .analogInputPin = A10,
      .referenceResistance = 100,
      .resistanceRange = {.min = 0, .max = 92},
      .outputRange = {.min = kAxesMin, .max = kAxesMax},
    }
  },
};

void setup()
{
  // Turn on LED as a sign of life
  int ledPins[] = {9, 5, 3};
  u8 ledColor[] = {15, 0, 0};
  for (int i = 0; i < sizeof(ledColor); i++) {
    analogWrite(ledPins[i], ledColor[i]);
  }

  hc165_collection_setup(hc165_collection);

  // Add USB HID device description of the Phoenix
  static HIDSubDescriptor sHidDescriptior(sHidDescriptorData, sizeof(sHidDescriptorData));
  HID().AppendDescriptor(&sHidDescriptior);
}

const int kNumButtons = 24;
/*
 * Map the hardware button ordering to a more sensible one by reordering The
 * bits. Each value is the source bit which should be copied to that location.
 */
u8 buttonMapping[kNumButtons] = {
  5,
  6,
  2,
  1,
  0,
  4,
  7,
  3,

  11,
  12,
  10,
  8,
  13,
  9,
  15,
  14,

  19,
  18,
  17,
  16,
  23,
  20,
  21,
  22,
};

/* Access button bits either as separate bytes or continuous bits. */
union buttons_union_t {
  u32 bits : kNumButtons;
  u8 bytes[kNumButtonGroups];
};

u32 getMappedButtons(hc165_data_t* hc165_data) {
  buttons_union_t buttonsRaw;
  for (int i = 0; i < kNumButtonGroups; i++) {
    buttonsRaw.bytes[i] = ~hc165_data[i].values;
  }

  u32 buttonsMapped = 0;
  for (int i = 0; i < kNumButtons; i++) {
    bitWrite(buttonsMapped, i, bitRead(buttonsRaw.bits, buttonMapping[i]));
  }

  return buttonsMapped;
}

void loop()
{
  ReportData reportData;

  // Read axes
  for (int i = 0; i < kNumAxes; i++) {
    reportData.axes[i] = axesPotentiometers[i].read();
  }

  // Reverse the throttle direction since min resistance is at the bottom
  // todo: make Potentiometer clamping handle to enable:
  //   .outputRange = {.min = kAxesMax, .max = kAxesMin},
  reportData.axes[3] = map(reportData.axes[3], kAxesMin, kAxesMax, kAxesMax, kAxesMin);

  // Read buttons
  hc165_collection_read(hc165_collection);
  u32 buttonsMapped = getMappedButtons(hc165_data);
  for (int i = 0; i < kNumButtonGroups; i++) {
    reportData.buttons[i] = buttonsMapped >> (i * 8);
  }

  // Report data to host
  HID().SendReport(kReportId, &reportData, sizeof(reportData));
}
