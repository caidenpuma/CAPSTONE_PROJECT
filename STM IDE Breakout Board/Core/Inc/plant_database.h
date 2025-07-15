/* SoilSense F4 Debug
 * Header file for plant database
 *
 */

#include <stdint.h>

#ifndef INC_PLANT_DATABASE_H_
#define INC_PLANT_DATABASE_H_

typedef struct {
    char plant_name[16];        // limit plant name to 16 chars -> one line on display
    uint8_t low_temp;         // low end of temperature range, in degrees F
    uint8_t high_temp;        // high end of temperature range, in degrees F
    uint8_t low_humidity;     // low end of humidity range, in %RH
    uint8_t high_humidity;    // high end of humidity range, in %RH
    uint8_t light_level;      // 1: low light/shade, 2: medium light, 3: bright indirect light, 4: direct light
    uint8_t light_time;       // daily time in ideal light range, in hours
    uint8_t low_moisture;     // minimum % humidity of soil
    uint8_t high_moisture;    // maximum % humidity of soil
} Plant;

extern Plant flower;
extern Plant tree;
extern Plant *plant_list[3];

#endif /* INC_PLANT_DATABASE_H_ */
