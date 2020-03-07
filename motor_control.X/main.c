#include "main.h"

char buf[32];
int i;
int j;
CommandPacket cmdpkt;


int main(void) {
//	unsigned int size;

	/* set PPS for UART to use pin D2 (Rx) and D3 (Tx) */
	U1RXR = 0x00;
	RPD3R = 0x01;

	/* init UART1 bus with 115200 baudrate*/
	U1MODEbits.ON = 1;
	U1STAbits.URXEN = 1;
	U1STAbits.UTXEN = 1;
	U1BRG = 82;

	i = 0;
	j = 0;

	actuator_init();

	for(;;) {
		while (U1STAbits.RIDLE == 1);
		while (U1STAbits.URXDA == 1) {
			buf[i++] = U1RXREG;
			if (i == 32) {
				i = 0;
			}
		}
		unpack_command_packet(&cmdpkt, buf);
		/* TODO: flip gpios to light LEDs according to the received command */
		actuator_set(i, (i % 2 == 0));
	}

	return 0;
}
