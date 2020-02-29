#include "main.h"

char buf[32];
int i;
int j;
CommandPacket cmdpkt;

/* public functions */
void latch_init(void);


int main(void) {
//	unsigned int size;

	/* set PPS for UART to use pin D2 (Rx) and D3 (Tx) */
//	U1RXR = 0x00;
//	RPD3R = 0x01;

	/* init UART1 bus with 115200 baudrate*/
//	U1MODEbits.ON = 1;
//	U1STAbits.URXEN = 1;
//	U1STAbits.UTXEN = 1;
//	U1BRG = 13; // TODO adjust this later to get the baudrate right
	
  
	i = 0;
	j = 0;
	
	latch_init();
	
	LATJbits.LATJ3 = 1;

	for(;;) {
//		while (U1STAbits.URXDA == 0);
//		buf[i] = U1RXREG;
//		i ++;
//		if (i >= 6) {
//			i = 0;
//			unpack_command_packet(&cmdpkt, buf);
//			/* TODO: flip gpios to light LEDs according to the received command */
//		}
		
		/* set address */
		LATJbits.LATJ0 = !!(i & 0b001);
		LATJbits.LATJ1 = !!(i & 0b010);
		LATJbits.LATJ2 = !!(i & 0b100);
		LATJbits.LATJ4 = 0;
		
		for (j = 0; j < 2000000; ++j);
		LATJbits.LATJ4 = 1;
		i++;
		
		
		if ((i >> 4) & 1) {
			LATJbits.LATJ3 = !PORTJbits.RJ3;
			i = 0;
		}
		
	}

	return 0;
}

void latch_init(void) {
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
//	LATJbits.LATJ4 = 0; // !G
	LATJbits.LATJ4 = 1; // !G
	LATJbits.LATJ6 = 0; // !CLR
	LATJbits.LATJ6 = 1; // !CLR
}


