#include "main.h"

char buf[32];
int i;


int main(void) {
    
    /*set PPS for UART to use pin D2 (Rx) and D3 (Tx) */
    U1RXR = 0x00;
    RPD3R = 0x01;
    
    /*init UART1 bus with 115200 baudrate*/
    U1MODEbits.ON = 1;
    U1STAbits.URXEN = 1;
    U1STAbits.UTXEN = 1;
    U1BRG = 13;
    
  
    i = 0;
    for(;;) {
        while (U1STAbits.URXDA == 0);
        buf[i] = U1RXREG;
        i ++;
        if (i == 32) {
            i = 0;
        }
        
    }
    
    return 0;
    
    
}


