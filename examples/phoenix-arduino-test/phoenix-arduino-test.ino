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
#include "Potentiometer.h"

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

const size_t numAxes = 4;

const Potentiometer axesPotentiometers[numAxes] = {
  {Potentiometer::Parameters{
    .name = "X",
    .analogInputPin = A0,
    .referenceResistance = 100,
    .resistanceRange = {.min = 0, .max = 87},
    .outputRange = {.min = 0, .max = 127},
  }},
  {Potentiometer::Parameters{
    .name = "Y",
    .analogInputPin = A1,
    .referenceResistance = 100,
    .resistanceRange = {.min = 0, .max = 85},
    .outputRange = {.min = 0, .max = 127},
  }},
  {Potentiometer::Parameters{
    .name = "R",
    .analogInputPin = A4,
    .referenceResistance = 100,
    .resistanceRange = {.min = 0, .max = 94},
    .outputRange = {.min = 0, .max = 127},
  }},
  {Potentiometer::Parameters{
    .name = "T",
    .analogInputPin = A5,
    .referenceResistance = 100,
    .resistanceRange = {.min = 0, .max = 92},
    .outputRange = {.min = 0, .max = 127},
  }},
};

void setup()
{
    Serial.begin(9600);

    hc165_collection_setup(hc165_collection);
    hc165_collection_read(hc165_collection);
    hc165_collection_print(hc165_collection);
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
        Serial.print("axes:");
        for(const Potentiometer& axesPotentiometer : axesPotentiometers) {
          Serial.print(' ');
          Serial.print(axesPotentiometer);
        }
        Serial.println();
    }

    delay(POLL_DELAY_MSEC);
}
