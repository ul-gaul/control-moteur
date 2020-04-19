/*
 * Library to read from pressure sensors
 *
 * Author:
 * 		Maxime Guillemette
 *
 */


#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "xc.h"

#include "rocket-packet/rocket_packet.h"


/*
 * Initialize all the manometers' ADC channels
 */
int manometer_init(void);


/*
 * Read from all manometers and write to buffer
 */
int manometer_read(int res[MANOMETERS_NUM]);


#endif /* _SENSOR_H_ */

