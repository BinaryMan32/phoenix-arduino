#ifndef HC165_H
#define HC165_H

struct hc165_config_t
{
  const char* name;
  int ploadPin; // when LOW, parallel load data
                // when HIGH, shift on clock transition
  int clockDataPin; // read data bits one at a time
                    // on LOW -> HIGH transition, make next bit available
};

void hc165_setup(const hc165_config_t& config);

uint8_t hc165_read(const hc165_config_t& config);

void hc165_parallel_load_set(const hc165_config_t& config, uint8_t value);

void hc165_read_bit_prepare(const hc165_config_t& config);

uint8_t hc165_read_bit(const hc165_config_t& config);

void hc165_shift(const hc165_config_t& config);

struct hc165_data_t
{
  uint8_t values;
  uint8_t oldValues;
};

void hc165_data_read(const hc165_config_t& config, hc165_data_t& data);

bool hc165_data_changed(const hc165_data_t& data);

void hc165_data_print(const hc165_config_t& config, const hc165_data_t& data);

struct hc165_collection_t
{
  const hc165_config_t* config;
  hc165_data_t* data;
  const size_t size;
};

void hc165_collection_setup(const hc165_collection_t& collection);

void hc165_collection_read(const hc165_collection_t& collection);

bool hc165_collection_changed(const hc165_collection_t& collection);

void hc165_collection_print(const hc165_collection_t& collection);

#endif /* HC165_H */

