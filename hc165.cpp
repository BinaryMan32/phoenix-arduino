#include <Arduino.h>
#include "hc165.h"

/* Width of data (how many ext lines).
*/
#define DATA_WIDTH 8

/* Width of pulse to trigger the shift register to read and latch.
*/
#define PULSE_WIDTH_USEC 5

void hc165_setup(const hc165_config_t& config)
{
  pinMode(config.ploadPin, OUTPUT);
  digitalWrite(config.ploadPin, HIGH);

  pinMode(config.clockDataPin, OUTPUT);
  digitalWrite(config.clockDataPin, HIGH);
}

uint8_t hc165_read(const hc165_config_t& config)
{
  uint8_t bytesVal = 0;

  hc165_parallel_load_set(config, LOW);
  delayMicroseconds(PULSE_WIDTH_USEC);
  hc165_parallel_load_set(config, HIGH);

  for(int i = 0; i < DATA_WIDTH; i++) {
    hc165_read_bit_prepare(config);
    delayMicroseconds(PULSE_WIDTH_USEC);

    uint8_t bitVal = hc165_read_bit(config);
    bytesVal |= (bitVal << ((DATA_WIDTH-1) - i));

    hc165_shift(config);
    delayMicroseconds(PULSE_WIDTH_USEC);
  }

  return bytesVal;
}

void hc165_parallel_load_set(const hc165_config_t& config, uint8_t value)
{
  digitalWrite(config.ploadPin, value);
}

void hc165_read_bit_prepare(const hc165_config_t& config)
{
  pinMode(config.clockDataPin, INPUT);
  digitalWrite(config.clockDataPin, LOW);
}

uint8_t hc165_read_bit(const hc165_config_t& config)
{
  return digitalRead(config.clockDataPin);
}

void hc165_shift(const hc165_config_t& config)
{
  pinMode(config.clockDataPin, OUTPUT);
  digitalWrite(config.clockDataPin, HIGH);
}

void hc165_data_read(const hc165_config_t& config, hc165_data_t& data)
{
  data.oldValues = data.values;
  data.values = hc165_read(config);
}

bool hc165_data_changed(const hc165_data_t& data)
{
  return data.values != data.oldValues;
}

void hc165_data_print(const hc165_config_t& config, const hc165_data_t& data)
{
  Serial.print(config.name);
  Serial.print(" ");

  for(int i = 0; i < DATA_WIDTH; i++) {
    Serial.print((data.values & (1 << i)) ? "." : "0");
  }

  Serial.print("\r\n");
}

void hc165_collection_setup(const hc165_collection_t& collection)
{
  for(int i = 0; i < collection.size; i++) {
    hc165_setup(collection.config[i]);
  }
  hc165_collection_read(collection);
}

void hc165_collection_read(const hc165_collection_t& collection)
{
  for(int i = 0; i < collection.size; i++) {
    hc165_data_read(collection.config[i], collection.data[i]);
  }
}

bool hc165_collection_changed(const hc165_collection_t& collection)
{
  for(int i = 0; i < collection.size; i++) {
    if(hc165_data_changed(collection.data[i])) {
      return true;
    }
  }
  return false;
}

void hc165_collection_print(const hc165_collection_t& collection)
{
  Serial.print(" group 01234567\r\n");
  for(int i = 0; i < collection.size; i++) {
    hc165_data_print(collection.config[i], collection.data[i]);
  }
  Serial.print("\r\n");
}

