#include <HID.h>

#include "hc165.h"
#include "Potentiometer.h"
#include "HidDescriptorHelper.h"

const size_t kNumAxes = 4;
using AxesValueType = int16_t;
#define AXIS_BITS 10
const AxesValueType kAxesMin = (-1 << (AXIS_BITS - 1));
const AxesValueType kAxesMax = -kAxesMin - 1;

const size_t kNumButtonGroups = 3;

#define VIRTUAL_HAT

// Report data sent by the HID input device
struct ReportData {
  AxesValueType axes[kNumAxes];
  u8 buttons[kNumButtonGroups];
};

using namespace usb::hid;

constexpr uint8_t GetByte(int16_t x, uint8_t b) {
  return (x >> (b << 3));
}

#ifdef VIRTUAL_HAT
const u8 kButtonCount = 20;
#else
const u8 kButtonCount = 24;
#endif /* VIRTUAL_HAT */

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
#if AXIS_BITS > 8
      Global::LogicalMinimum | 2, GetByte(kAxesMin, 0), GetByte(kAxesMin, 1),
      Global::LogicalMaximum | 2, GetByte(kAxesMax, 0), GetByte(kAxesMax, 1),
#else // AXIS_BITS <= 8
      Global::LogicalMinimum | 1, kAxesMin,
      Global::LogicalMaximum | 1, kAxesMax,
#endif
      Global::ReportSize | 1, sizeof(AxesValueType) * 8,
      Global::ReportCount | 1, 4,
      Main::Input | 1, DataBits::Variable,
    Main::EndCollection | 0,

    // 24 1 bit buttons
    Global::UsagePage | 1, usage::Page::Button,
    Local::UsageMinimum | 1, 1,
    Local::UsageMaximum | 1, kButtonCount,
    Global::LogicalMinimum | 1, 0,
    Global::LogicalMaximum | 1, 1,
    Global::ReportSize | 1, 1,
    Global::ReportCount | 1, kButtonCount,
    Main::Input | 1, DataBits::Variable,

#ifdef VIRTUAL_HAT
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

#ifdef VIRTUAL_HAT
const int8_t hat_null = -8;
int8_t button_hat_lut[16] = {
  hat_null, // 0000
  0,        // 0001
  2,        // 0010
  1,        // 0011
  4,        // 0100
  hat_null, // 0101
  3,        // 0110
  hat_null, // 0111
  6,        // 1000
  7,        // 1001
  hat_null, // 1010
  hat_null, // 1011
  5,        // 1100
  hat_null, // 1101
  hat_null, // 1110
  hat_null, // 1111
};
#endif /* VIRTUAL_HAT */

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

#ifdef TIME_HISTOGRAM
  Serial.begin(9600);
#endif /* TIME_HISTOGRAM */
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
#ifndef VIRTUAL_HAT
  13,
  15,
  14,
  9,
#endif /* ! VIRTUAL_HAT */

  19,
  18,
  17,
  16,
  23,
  20,
  21,
  22,

#ifdef VIRTUAL_HAT
  13,
  15,
  14,
  9,
#endif /* VIRTUAL_HAT */
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

#ifdef TIME_HISTOGRAM
const size_t num_bins = 100;
uint32_t histogram[num_bins] = {};
unsigned long micros_prev = 0;
unsigned long bin_min = 5 * 1000;
unsigned long bin_size = 100;
unsigned long bin_max = bin_min + bin_size * num_bins - 1;

void time_histogram_update(unsigned long micros_delta) {
  size_t bin = (constrain(micros_delta, bin_min, bin_max) - bin_min) / bin_size;
  histogram[bin]++;
}

void time_histogram_dump() {
  for(size_t i = 0; i < num_bins; i++) {
    unsigned long bin = (bin_min + i * bin_size) / 100;
    Serial.print(bin / 10);
    Serial.print('.');
    Serial.print(bin % 10);
    Serial.print(' ');
    Serial.print(histogram[i]);
    Serial.println();
  }
}
#endif /* TIME_HISTOGRAM */

u32 buttonsPrev = 0;

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

  #ifdef VIRTUAL_HAT
  u8& hat_button_group = reportData.buttons[kNumButtonGroups - 1];
  uint8_t hat_buttons = hat_button_group >> 4;
  uint8_t hat_angle = button_hat_lut[hat_buttons];
  hat_button_group = (hat_button_group & 0xf) | (hat_angle << 4);
  #endif /* VIRTUAL_HAT */

  // Report data to host
  HID().SendReport(kReportId, &reportData, sizeof(reportData));

#ifdef TIME_HISTOGRAM
  unsigned long micros_curr = micros();
  time_histogram_update(micros_curr - micros_prev);
  micros_prev = micros_curr;

  if(((buttonsPrev ^ buttonsMapped) & 1) && (buttonsMapped ^ 1)) {
    time_histogram_dump();
    memset(histogram, 0, sizeof(histogram));
  }
#endif /* TIME_HISTOGRAM */

  buttonsPrev = buttonsMapped;
}
