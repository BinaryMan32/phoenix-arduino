#ifndef PHOENIX_ARDUINO_POTENTIOMETER_H
#define PHOENIX_ARDUINO_POTENT

#include <Printable.h>

#include "Fraction.h"

/*
 * Attempting to read the voltage across only a potentiometer will always
 * return 1023. The voltage difference is 5V because there is nothing else in
 * the circuit. To measure the resistance, we use a reference resistor in
 * series with the potentiometer:
 *
 *  5V --v^v^v-- V1 --v^v^v-- GROUND
 *        Rpot         Rref
 *
 * To solve for Rpot, use Ohm's law and the fact that 2 resistors in series
 * must have the same current:
 *
 *  (Vsupply - V1) / Rpot = (V1 - 0V) / Rref
 *  (Vsupply - V1) * Rref = V1 * Rpot
 *  (Vsupply / V1 - 1) * Rref = Rpot
 *
 * The ADC maps 5V (assumed to be both the ADC reference voltage and the supply
 * voltage) to a value 0 - 1023 (kAdcOutputMax). Since only the ratio of
 * Vsupply and V1 is important, we can treat both as ADC outputs instead of
 * converting from ADC to voltage on both sides.
 *
 * See http://www.built-to-spec.com/blog/2009/09/10/using-a-pc-joystick-with-the-arduino/
 */
class Potentiometer : public Printable
{
public:
  struct RangeParameters
  {
    long min;
    long max;
  };

  struct Parameters
  {
    const char* name;
    int analogInputPin;
    long referenceResistance;
    RangeParameters resistanceRange;
    RangeParameters outputRange;
  };

  Potentiometer(const Parameters& params);

  long read() const;

  Fraction readOutput() const;

  Fraction readResistance() const;

  long readAdc() const;

  Fraction mapAdcToResistance(long adc);

  Fraction mapResistanceToOutput(const Fraction& resistance);

  long clampOutput(long output);

  /**
   * Enables printing the Potentiometer via Serial.print(Fraction{1, 2})
   */
  size_t printTo(Print& p) const override;

protected:

  struct Range
  {
    Range(const RangeParameters& params);
    long min;
    long max;
    long size;
  };

  const char* mName;
  int mAnalogInputPin;
  long mReferenceResistance;
  Range mResistanceRange;
  Range mOutputRange;
};

#endif /* PHOENIX_ARDUINO_POTENTIOMETER_H */
