#ifndef PHOENIX_ARDUINO_HIDDESCRIPTORHELPER_H
#define PHOENIX_ARDUINO_HIDDESCRIPTORHELPER_H

namespace usb {
namespace hid {
/*
 * Main, Global, and Local are types of usb HID elements, most of which
 * include an associated value, this size of which is specified using the
 * | operator as follows:
 *  Global::UsagePage | 1, usage::Page::GenericDesktop
 * This is a UsagePage with a size of 1 which has a value of GenericDesktop.
 *
 * See http://www.usb.org/developers/hidpage/HID1_11.pdf
 */

enum class Main : u8
{
  Input         = 0x8, // See DataBits enum for values
  Output        = 0x9, // See DataBits enum for values
  Collection    = 0xa, // See Collection enum for values
  Feature       = 0xb, // See DataBits enum for values
  EndCollection = 0xc, // No values, size always 0
};

enum class Global : u8
{
  UsagePage       = 0x0, // See usage::Page for values
  LogicalMinimum  = 0x1,
  LogicalMaximum  = 0x2,
  PhysicalMinimum = 0x3,
  PhysicalMaximum = 0x4,
  UnitExponent    = 0x5,
  Unit            = 0x6,
  ReportSize      = 0x7, // value is size of each element in bits
  ReportID        = 0x8,
  ReportCount     = 0x9, // value is the number of elements
  Push            = 0xa,
  Pop             = 0xb,
};

enum class Local : u8
{
  Usage             = 0x0, // see appropriate sub-namespace of usage
                           // based on active Global::UsagePage
  UsageMinimum      = 0x1,
  UsageMaximum      = 0x2,
  DesignatorIndex   = 0x3,
  DesignatorMinimum = 0x4,
  DesignatorMaximum = 0x5,
  StringIndex       = 0x7,
  StringMinimum     = 0x8,
  StringMaximum     = 0x9,
  Delimiter         = 0xa,
};

namespace detail {
/*
 * Helper function to use in overloaded operators which construct an item in the
 * USB HID descriptor by combining tag, type and size.
 */
constexpr u8 encodeItemSize(int size)
{
  return (size == 1 || size == 2) ? size : ((size == 4) ? 3 : 0);
}

enum class ItemType : u8
{
  Main   = 0x00,
  Global = 0x01,
  Local  = 0x02,
};

/*
 * Constructs an 8 bit item from:
 * 4 bits of u8 tag
 * 2 bits of ItemType type
 * 2 bits of int size encoded by encodeItemSize()
 */
constexpr u8 makeItem(u8 tag, ItemType type, int size)
{
  return (tag << 4) | (static_cast<u8>(type) << 2) | encodeItemSize(size);
}
} // namespace detail

/*
 * Allows user to create an item by or-ing the enum with the item value's size.
 * Since the item tags are strongly typed enums, users can't forget to or in the
 * type because the enums can't be implictly converted to a u8.
 */
constexpr u8 operator | (Main tag, int size)
{
  return detail::makeItem(static_cast<u8>(tag), detail::ItemType::Main, size);
}

/*
 * Allows user to create an item by or-ing the enum with the item value's size.
 * Since the item tags are strongly typed enums, users can't forget to or in the
 * type because the enums can't be implictly converted to a u8.
 */
constexpr u8 operator | (Global tag, int size)
{
  return detail::makeItem(static_cast<u8>(tag), detail::ItemType::Global, size);
}

/*
 * Allows user to create an item by or-ing the enum with the item value's size.
 * Since the item tags are strongly typed enums, users can't forget to or in the
 * type because the enums can't be implictly converted to a u8.
 */
constexpr u8 operator | (Local tag, int size)
{
  return detail::makeItem(static_cast<u8>(tag), detail::ItemType::Local, size);
}

// Bits used in constructing values of Main::Input, Main::Output, and Main::Feature
enum DataBits
{
  Constant    = 0x01, // Data if not set
  Variable    = 0x02, // Array if not set
  Relative    = 0x04, // Absolute if not set
  Wrap        = 0x08, // NoWrap if not set
  NonLinear   = 0x10, // Linear if not set
  NoPreferred = 0x20, // Has Preferred state if not set
  NullState   = 0x40, // NoNullPosition if not set
  Volatile    = 0x80, // NonVolatile if not set
};

// Values of Main::Collection
enum Collection
{
  Physical      = 0x00,
  Application   = 0x01,
  Logical       = 0x02,
  Report        = 0x03,
  NamedArray    = 0x04,
  UsageSwitch   = 0x05,
  UsageModifier = 0x06,
};

/*
 * Global::UsagePage will have a value from usage::Page.
 * The usage page is effectively a namespace for Local::Usage values,
 * see the comments on each Page value for details.
 *
 * See http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
 */
namespace usage {
enum Page
{
  GenericDesktop = 0x01, // See generic_desktop namespace
  Simulation     = 0x02,
  VR             = 0x03,
  Sport          = 0x04,
  Game           = 0x05,
  GenericDevice  = 0x06,
  Keyboard       = 0x07,
  LED            = 0x08,
  Button         = 0x09,
  Ordinal        = 0x0a,
  Telephony      = 0x0b,
  Consumer       = 0x0c,
  Digitizer      = 0x0d,
};

namespace generic_desktop {

enum Application
{
  Pointer   = 0x01,
  Mouse     = 0x02,
  Joystick  = 0x04,
  GamePad   = 0x05,
  Keyboard  = 0x06,
  Keypad    = 0x07,
  MultiAxis = 0x08,
  Tablet    = 0x09,
};

enum Axis
{
  X  = 0x30,
  Y  = 0x31,
  Z  = 0x32,
  Rx = 0x33,
  Ry = 0x34,
  Rz = 0x35,
};

enum Miscellaneous
{
  Slider       = 0x36,
  Dial         = 0x37,
  Wheel        = 0x38,
  HatSwitch    = 0x39,
  MotionWakeup = 0x3c,
  Start        = 0x3d,
  Select       = 0x3e,
};

enum DirectionPad
{
  Up    = 0x90,
  Down  = 0x91,
  Right = 0x92,
  Left  = 0x93,
};

} // namespace generic_desktop
} // namespace usage
} // namespace hid
} // namespace usb

#endif /* PHOENIX_ARDUINO_HIDDESCRIPTORHELPER_H */
