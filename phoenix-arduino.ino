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
#define POLL_DELAY_MSEC   1

const size_t numButtonGroups = 3;

const hc165_config_t hc165_configs[numButtonGroups] = {
  {
    .name = "handle",
    .ploadPin = 22,
    .clockDataPin = 23,
  },
  {
    .name = "  wing",
    .ploadPin = 24,
    .clockDataPin = 25,
  },
  {
    .name = "thrttl",
    .ploadPin = 26,
    .clockDataPin = 27,
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
}

void loop()
{
    hc165_collection_read(hc165_collection);
    if(hc165_collection_changed(hc165_collection)) {
        hc165_collection_print(hc165_collection);
    }

    delay(POLL_DELAY_MSEC);
}
