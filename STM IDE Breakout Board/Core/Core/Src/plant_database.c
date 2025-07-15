/* SoilSense F4 Debug
 * Stores data on types of plants and corresponding moisture, temperature,
 * humidity, and light levels */

#include <stdint.h>

#include "plant_database.h"

Plant flower = {"flower", 50,100,50,100,1,5,81,100};
Plant tree = {"tree", 1,2,3,4,5,6,7,8};
Plant succulent = {"succulent", 1,2,3,4,5,6,7,8};


Plant *plant_list[3] = {&flower, &tree, &succulent};
