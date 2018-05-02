#include "hc165.h"
#include "Potentiometer.h"

/* Optional delay between shift register reads.
*/
#define POLL_DELAY_MSEC   10

const size_t numButtonGroups = 3;

const hc165_config_t hc165_configs[numButtonGroups] = {
  {
    .name = "handle",
    .ploadPin = 7,
    .clockDataPin = 4,
  },
  {
    .name = "  wing",
    .ploadPin = 16,
    .clockDataPin = 14,
  },
  {
    .name = "thrttl",
    .ploadPin = 15,
    .clockDataPin = 18,
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
    .analogInputPin = A8,
    .referenceResistance = 100,
    .resistanceRange = {.min = 0, .max = 87},
    .outputRange = {.min = 0, .max = 127},
  }},
  {Potentiometer::Parameters{
    .name = "Y",
    .analogInputPin = A9,
    .referenceResistance = 100,
    .resistanceRange = {.min = 0, .max = 85},
    .outputRange = {.min = 0, .max = 127},
  }},
  {Potentiometer::Parameters{
    .name = "R",
    .analogInputPin = A1,
    .referenceResistance = 100,
    .resistanceRange = {.min = 0, .max = 94},
    .outputRange = {.min = 0, .max = 127},
  }},
  {Potentiometer::Parameters{
    .name = "T",
    .analogInputPin = A10,
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
