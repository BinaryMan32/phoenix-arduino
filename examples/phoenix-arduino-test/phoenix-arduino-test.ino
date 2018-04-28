/*
 * SN74HC165N_shift_reg
 *
 * Program to shift in the bit values from a SN74HC165N 8-bit
 * parallel-in/serial-out shift register.
 *
 * This sketch demonstrates reading in 16 digital states from a
 * pair of daisy-chained SN74HC165N shift registers while using
 * only 4 digital pins on the Arduino.
 *
 * You can daisy-chain these chips by connecting the serial-out
 * (Q7 pin) on one shift register to the serial-in (Ds pin) of
 * the other.
 *
 * Of course you can daisy chain as many as you like while still
 * using only 4 Arduino pins (though you would have to process
 * them 4 at a time into separate unsigned long variables).
 *
 * https://playground.arduino.cc/Code/ShiftRegSN74HC165N
*/
#include "hc165.h"

/* Optional delay between shift register reads.
*/
#define POLL_DELAY_MSEC   10

const size_t numButtonGroups = 3;

const hc165_config_t hc165_configs[numButtonGroups] = {
  {
    .name = "handle",
    .ploadPin = 11,
    .clockDataPin = 12,
  },
  {
    .name = "  wing",
    .ploadPin = 3,
    .clockDataPin = 2,
  },
  {
    .name = "thrttl",
    .ploadPin = 7,
    .clockDataPin = 6,
  },
};

hc165_data_t hc165_data[numButtonGroups];

hc165_collection_t hc165_collection = {
  .config = hc165_configs,
  .data = hc165_data,
  .size = numButtonGroups,
};

void setup()
{
    Serial.begin(9600);

    hc165_collection_setup(hc165_collection);
    hc165_collection_read(hc165_collection);
    hc165_collection_print(hc165_collection);

    analogReference(DEFAULT);
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A4, INPUT);
    pinMode(A5, INPUT);
}

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
 *  Vsupply * Rref / V1 - Rref = Rpot
 *
 * The ADC maps 5V (assumed to be both the ADC reference voltage and the supply
 * voltage) to a value 0 - 1023 (kAdcOutputMax). Since only the ratio of
 * Vsupply and V1 is important, we can treat both as ADC outputs instead of
 * converting from ADC to voltage on both sides.
 *
 * See http://www.built-to-spec.com/blog/2009/09/10/using-a-pc-joystick-with-the-arduino/
 */
const long kDivResistanceKOhm = 100;
const long kAdcOutputMax = 1023;

uint8_t get_potentiometer_resistance(int adc_in)
{
  if(adc_in > 0) {
    return uint8_t((kAdcOutputMax * kDivResistanceKOhm) / adc_in - kDivResistanceKOhm);
  } else {
    return 0;
  }
}

void loop()
{
    hc165_collection_read(hc165_collection);
    if(hc165_collection_changed(hc165_collection)) {
        hc165_collection_print(hc165_collection);
    }

    bool isButtonPressed = false;
    for(int i = 0; i < hc165_collection.size; i++) {
        if(hc165_data[i].values != 0xff) {
            isButtonPressed = true;
            break;
        }
    }

    if(!isButtonPressed) {
        Serial.print("Axes: x:");
        Serial.print(get_potentiometer_resistance(analogRead(A0)));
        Serial.print(" y:");
        Serial.print(get_potentiometer_resistance(analogRead(A1)));
        Serial.print(" r:");
        Serial.print(get_potentiometer_resistance(analogRead(A4)));
        Serial.print(" t:");
        Serial.print(get_potentiometer_resistance(analogRead(A5)));
        Serial.print("\r\n");
    }

    delay(POLL_DELAY_MSEC);
}

