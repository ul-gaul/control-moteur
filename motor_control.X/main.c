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
		/* TODO: flip gpios to light LEDs according to the received command:
         * function values are 1 or 2, 1 sets the actuator state to 1, 2 sets
         *  the actuator state to 0, if a function value is other than 1 or 2,
         *  it should be ignored
         * arg values are the number of the actuator, from 0 to 5, other
         *  values should be ignored
         */
		//actuator_set(i, (i % 2 == 0));
        
        if ((0 < cmdpkt.function)&&(cmdpkt.function < 3)){ 
            continue ;
        }
    
        if (cmdpkt.arg > 5){
           continue ;
        }
           
        actuator_set(cmdpkt.arg, cmdpkt.function % 2);
	}
    
	return 0;
}
