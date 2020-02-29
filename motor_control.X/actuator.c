#include "actuator.h"


/* private variables */
static int timer;


void actuator_init(void) {
	/* set pins as outputs */
	TRISJbits.TRISJ0 = 0;
	TRISJbits.TRISJ1 = 0;
	TRISJbits.TRISJ2 = 0;
	TRISJbits.TRISJ3 = 0;
	TRISJbits.TRISJ4 = 0;
	TRISJbits.TRISJ6 = 0;
	
	/* set address to 0, data to 0 and latch in memory mode */
	LATJbits.LATJ0 = 0; // S0
	LATJbits.LATJ1 = 0; // S1
	LATJbits.LATJ2 = 0; // S2
	LATJbits.LATJ3 = 0; // D
	LATJbits.LATJ4 = 1; // !G

	/* clear latch states */
	LATJbits.LATJ6 = 0; // !CLR
	for (timer = 0;  timer < 100; ++timer);
	LATJbits.LATJ6 = 1; // !CLR
}


void actuator_set(uint8_t id, uint8_t state) {
	/* set chip in memory mode */
	LATJbits.LATJ4 = 1;
	LATJbits.LATJ6 = 1;

	/* set address and data bit */
	LATJbits.LATJ0 = !!(id & 0b001);
	LATJbits.LATJ1 = !!(id & 0b010);
	LATJbits.LATJ2 = !!(id & 0b100);
	LATJbits.LATJ3 = !!state;

	/* enable cycle */
	LATJbits.LATJ4 = 0;
	for (timer = 0; timer < 100; ++timer);
	LATJbits.LATJ4 = 1;

}
