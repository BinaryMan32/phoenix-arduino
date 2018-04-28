#include <Print.h>

#include "Fraction.h"

long gcf(long a, long b)
{
  return (b == 0) ? a : gcf(b, a % b);
}

long simplifyFraction(long& numerator, long& denominator)
{
  long factor = gcf(numerator, denominator);
  if(factor > 1) {
    numerator /= factor;
    denominator /= factor;
  }
  return factor;
}

Fraction::Fraction(long numerator, long denominator)
: mNumerator{numerator}, mDenominator{denominator}
{
  simplifyFraction(mNumerator, mDenominator);
  if(mDenominator < 0) {
    mNumerator = -mNumerator;
    mDenominator = -mDenominator;
  }
}

Fraction Fraction::getInverse() const
{
  return Fraction{mDenominator, mNumerator, AlreadySimplifiedTag{}};
}

long Fraction::getResultRounded() const
{
  return (mNumerator + mDenominator / 2) / mDenominator;
}

size_t Fraction::printTo(Print& p) const
{
  return p.print('(') + p.print(mNumerator) + p.print('/')
    + p.print(mDenominator) + p.print(")~=") + p.print(getResultRounded());
}

Fraction Fraction::operator-() const
{
  return Fraction{-mNumerator, mDenominator, AlreadySimplifiedTag{}};
}

Fraction::Fraction(long numerator, long denominator, AlreadySimplifiedTag)
: mNumerator{numerator}, mDenominator{denominator}
{
}

Fraction operator + (long factor, const Fraction& fraction)
{
  return Fraction{factor * fraction.mDenominator + fraction.mNumerator,
                  fraction.mDenominator};
}

Fraction operator + (const Fraction& fraction, long factor)
{
  return factor + fraction;
}

Fraction operator + (const Fraction& a, const Fraction& b)
{
  const long factor = gcf(a.mDenominator, b.mDenominator);
  return Fraction{a.mNumerator * (b.mDenominator / factor) +
                  b.mNumerator * (a.mDenominator / factor),
                  (a.mDenominator / factor) * b.mDenominator};
}

Fraction operator - (long factor, const Fraction& fraction)
{
  return (-factor) + fraction;
}

Fraction operator - (const Fraction& fraction, long factor)
{
  return (-factor) + fraction;
}

Fraction operator - (const Fraction& a, const Fraction& b)
{
  return a + (-b);
}

Fraction operator * (long factor, const Fraction& fraction)
{
  return Fraction(factor * fraction.mNumerator, fraction.mDenominator);
}

Fraction operator * (const Fraction& fraction, long factor)
{
  return factor * fraction;
}

Fraction operator * (const Fraction& a, const Fraction& b)
{
  Fraction(a.mNumerator * b.mNumerator, a.mDenominator * b.mDenominator);
}

Fraction operator / (long factor, const Fraction& fraction)
{
  return factor * fraction.getInverse();
}

Fraction operator / (const Fraction& fraction, long factor)
{
  return fraction * Fraction{1, factor, Fraction::AlreadySimplifiedTag{}};
}

Fraction operator / (const Fraction& a, const Fraction& b)
{
  return a * b.getInverse();
}
