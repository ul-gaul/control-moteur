/*
 *
 * Actuator.h
 * 	Author:
 * 		Maxime Guillemette
 * 		Elliot Lafront
 */


#ifndef _ACTUATOR_H_
#define _ACTUATOR_H_


/* includes */
#include <xc.h>
#include <stdint.h>


/* public functions */

int actuator_init(void);
void actuator_set(uint8_t id, uint8_t state);

#endif /* _ACTUATOR_H_ */
