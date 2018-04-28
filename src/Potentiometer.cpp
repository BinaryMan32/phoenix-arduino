#include <Arduino.h>
#include <Print.h>

#include "Potentiometer.h"

const long kAdcOutputMax = 1023;

Potentiometer::Potentiometer(const Potentiometer::Parameters& params)
:
mName{params.name},
mAnalogInputPin{params.analogInputPin},
mReferenceResistance{params.referenceResistance},
mResistanceRange{params.resistanceRange},
mOutputRange{params.outputRange}
{
  pinMode(mAnalogInputPin, INPUT);
}

long Potentiometer::read() const
{
  return clampOutput(readOutput().getResultRounded());
}

Fraction Potentiometer::readOutput() const
{
  return mapResistanceToOutput(readResistance());
}

Fraction Potentiometer::readResistance() const
{
  return mapAdcToResistance(readAdc());
}

long Potentiometer::readAdc() const
{
  return analogRead(mAnalogInputPin);
}

Fraction Potentiometer::mapAdcToResistance(long adc)
{
  if(adc > 0) {
    return (Fraction{kAdcOutputMax, adc} - 1) * mReferenceResistance;
  }

  return Fraction{0, 1};
}

Fraction Potentiometer::mapResistanceToOutput(const Fraction& resistance)
{
  return (resistance - mResistanceRange.min)
         * Fraction{mOutputRange.size, mResistanceRange.size}
         + mOutputRange.min;
}

long Potentiometer::clampOutput(long output)
{
  return constrain(output, mOutputRange.min, mOutputRange.max);
}

size_t Potentiometer::printTo(Print& p) const
{
  const long adc = readAdc();
  const Fraction resistance = mapAdcToResistance(adc);
  const Fraction output = mapResistanceToOutput(resistance);
  const long outputClamped = clampOutput(output.getResultRounded());

  return p.print(mName) + p.print(':')
    + p.print(" adc:") + p.print(adc)
    + p.print(" res:") + p.print(resistance.getResultRounded())
    + p.print(" out:") + p.print(output.getResultRounded())
    + p.print(" clmp:") + p.print(outputClamped);
}

Potentiometer::Range::Range(const RangeParameters& params)
:
min{params.min}, max{params.max}, size{params.max - params.min}
{
}
