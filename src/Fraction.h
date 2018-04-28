#ifndef PHOENIX_ARDUINO_FRACTION_H
#define PHOENIX_ARDUINO_FRACTION_H

#include <Printable.h>

/*
 * Euclid's algorithm to find GCF (Greatest Common Factor) of a and b.
 * Note that gcf(0, X) == gcf(X, 0) == X.
 */
long gcf(long a, long b);

/*
 * Simplify a fraction by dividing the numerator and denominator by the GCF
 * (Greatest Common Factor). Return the GCF.
 */
long simplifyFraction(long& numerator, long& denominator);

class Fraction : public Printable
{
public:
  /**
   * Create from numerator and denominator
   */
  Fraction(long numerator, long denominator);

  /**
   * Returns a new Fraction (denominator / numerator).
   */
  Fraction getInverse() const;

  /**
   * Get result of (numerator / denomonator as a rounded whole number
   */
  long getResultRounded() const;

  /**
   * Enables printing the Fraction via Serial.print(Fraction{1, 2})
   */
  size_t printTo(Print& p) const override;

  /** Unary negation */
  Fraction operator-() const;

protected:
  /**
   * Tag used internally to construct an already simplified fraction.
   */
  struct AlreadySimplifiedTag {};

  /**
   * Construct a Fraction which is already simplified.
   */
  Fraction(long numerator, long denominator, AlreadySimplifiedTag);

  long mNumerator;
  long mDenominator;

  friend Fraction operator + (long factor, const Fraction& fraction);
  friend Fraction operator + (const Fraction& fraction, long factor);
  friend Fraction operator + (const Fraction& a, const Fraction& b);

  friend Fraction operator - (long factor, const Fraction& fraction);
  friend Fraction operator - (const Fraction& fraction, long factor);
  friend Fraction operator - (const Fraction& a, const Fraction& b);

  friend Fraction operator * (long factor, const Fraction& fraction);
  friend Fraction operator * (const Fraction& fraction, long factor);
  friend Fraction operator * (const Fraction& a, const Fraction& b);

  friend Fraction operator / (long factor, const Fraction& fraction);
  friend Fraction operator / (const Fraction& fraction, long factor);
  friend Fraction operator / (const Fraction& a, const Fraction& b);
};

Fraction operator + (long factor, const Fraction& fraction);
Fraction operator + (const Fraction& fraction, long factor);
Fraction operator + (const Fraction& a, const Fraction& b);

Fraction operator - (long factor, const Fraction& fraction);
Fraction operator - (const Fraction& fraction, long factor);
Fraction operator - (const Fraction& a, const Fraction& b);

Fraction operator * (long factor, const Fraction& fraction);
Fraction operator * (const Fraction& fraction, long factor);
Fraction operator * (const Fraction& a, const Fraction& b);

Fraction operator / (long factor, const Fraction& fraction);
Fraction operator / (const Fraction& fraction, long factor);
Fraction operator / (const Fraction& a, const Fraction& b);

#endif /* PHOENIX_ARDUINO_FRACTION_H */
