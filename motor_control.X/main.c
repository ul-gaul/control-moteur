#include "main.h"


int main(void) {

	/* TODO: RWD */
	TRISKbits.TRISK0 = 0;
	LATKbits.LATK0 = 0;

	if (init_all()) {
		while(1);
	}
	
	/* testing */
	int i;
	unsigned int j;
	for (i = 0; i < 10; ++i) {
		j = 0x7fffff;
		actuator_set(0, !!(i % 2));
		actuator_set(2, !!(i % 2));
		actuator_set(4, !!(i % 2));
		while (j-- > 0);
	}

	for(;;) {
		manometer_read(motor_data.manometers);
		switch (motor_cmd_h.state) {
		case idle:
			break;
		case cmd_received:
			/* unpack the command */
			unpack_command_packet(&motor_cmd_h.cmd, (uint8_t *) cmd_rx_buf);
			/* validate motor command's function and arg */
			if (mc_valid_function(motor_cmd_h.cmd.function)
					&& mc_valid_arg(motor_cmd_h.cmd.function, motor_cmd_h.cmd.arg)) {
				motor_cmd_h.ack = NACK;
				motor_cmd_h.state = done;
				break;
			}
			/* execute the command */
			if (execute_motor_cmd(&motor_cmd_h.cmd) != 0) {
				motor_cmd_h.ack = NACK;
			}
			/* set the state to done */
			motor_cmd_h.state = done;
			break;
		case done:
			/* send acknowledge back */
			motor_cmd_h.ackpkt.start_short = motor_cmd_h.cmd.start_short;
			motor_cmd_h.ackpkt.id = motor_cmd_h.cmd.id;
			motor_cmd_h.ackpkt.ack = motor_cmd_h.ack;
			pack_ack_packet(&motor_cmd_h.ackpkt, (uint8_t *) ack_tx_buf);
			send_ack(ack_tx_buf, ACK_TX_BUF_SIZE);
			/* TODO: resume sending motor data */

			/* reset state */
			motor_cmd_h.state = idle;
			break;
		default:
			break;
		}
	}

	return 0;
}


int init_all(void) {
	int err = 0;

	err = actuator_init();
	if (err) goto exit;

	err = manometer_init();
	if (err) goto exit;

	err = init_sdcard();
	if (err) goto exit;

	err = init_comm_module_uart();
	if (err) goto exit;

	err = init_comm_module_dma();
	if (err) goto exit;

	err = init_timer();
	if (err) goto exit;

	err = init_interrupts();
	if (err) goto exit;

	err = enable_dma();
	if (err) goto exit;

exit:
	return 0;
}


int init_sdcard(void) {

	return 0;
}


int init_interrupts(void) {
	/* enable multi-vector interrupt */
	INTCONbits.MVEC = 1;
	/* assign shadow set 7 through 1 to priority level 7 through 1 */
	PRISS = 0x76543210;
	/* enable interrupts */
	__builtin_enable_interrupts();

	return 0;
}


int init_timer(void) {
	/*
	 * use timer 2 to generate an interrupt every 10 ms,
	 * 16-bit mode, clock source is internal peripheral clock,
	 * prescaler is 4
	 */
	T2CONbits.SIDL = 0;
	T2CONbits.TGATE = 0;
	T2CONbits.TCKPS = 0b110;
	T2CONbits.T32 = 0;
	T2CONbits.TCS = 0;

	TMR2 = 0;

	PR2 = 0x5ae7;

	/* enable interrupts for timer 2, priority = 2, sub-priority = 3 */
	IFS0bits.T2IF = 0;
	IPC2bits.T2IP = 2;
	IPC2bits.T2IS = 3;
	IEC0bits.T2IE = 1;

	/* start the timer */
	T2CONbits.ON = 1;

	return 0;
}


int init_comm_module_uart(void) {
	/* init pins and pin select registers */
	UART1_RX_PS();
	UART1_TX_PS();

	/* init UART1 bus with 115200 baudrate */
	U1MODEbits.ON = 1;
	U1STAbits.URXEN = 1;
	U1STAbits.UTXEN = 1;
	U1BRG = 82;

	/*
	 * UART1 interrupts:
	 * enable RX interrupt
	 * priority = 1, subpriority = 2
	 */
	IEC3bits.U1RXIE = 1;
	IPC28bits.U1RXIP = 1;
	IPC28bits.U1RXIS = 3;

	return 0;
}


int enable_dma(void) {
	/* enable DMA controller */
	DMACONbits.ON = 1;

	return 0;
}


int init_comm_module_dma(void) {
	/* CRC enabled, 16-bit polynomial, background mode, CRC on channel 0 */
	DCRCXOR = CRC_POLY;
	DCRCDATA = CRC_SEED;
	DCRCCONbits.PLEN = CRC_LEN - 1;
	DCRCCONbits.CRCEN = 1;

	/* start IRQ is UART1 RX (113), disable pattern matching */
	DCH0ECONbits.CHSIRQ = _UART1_RX_VECTOR;
	DCH0ECONbits.SIRQEN = 1;

	/* source physical address is UART1 RX register */
	DCH0SSA = KVA_TO_PA((void *) &U1RXREG);
	/* destination physical address is the command buffer */
	DCH0DSA = KVA_TO_PA((void *) cmd_rx_buf);
	/* source size and offset */
	DCH0SSIZ = 1;
	DCH0SPTR = 0;
	/* destination size and offset */
	DCH0DSIZ = CMD_RX_BUF_SIZE;
	DCH0DPTR = 0;
	/* 1 byte per UART transfer */
	DCH0CSIZ = 1;

	/* enable block complete and error to trigger interrupts */
	DCH0INTbits.CHBCIE = 1;
	DCH0INTbits.CHERIE = 1;

	/* channel 0 on, auto re-enable, highest priority, no chaining */
	DCH0CONbits.CHEN = 1;
	DCH0CONbits.CHAEN = 1;
	DCH0CONbits.CHPRI = 3;

	/* clear DMA0 interrupt flag */
	IFS4bits.DMA0IF = 0;
	/* enable DMA channel 0 interrupt with priority 5 and sub-priority 2*/
	IEC4bits.DMA0IE = 1;
	IPC33bits.DMA0IP = 5;
	IPC33bits.DMA0IS = 2;

	return 0;
}


int send_ack(char* src, int size) {
	int i;

	for (i = 0; i < size; ++i) {
		U1TXREG = src[i];
		/* wait for TX buffer to be empty */
		while (U1STAbits.TRMT == 0);
		while (U1STAbits.UTXBF == 1);
	}

	return i;
}


int execute_motor_cmd(CommandPacket* cmd) {
	int i;

	switch (cmd->function) {
	case MC_SET_ACTUATOR:
		actuator_set(cmd->arg, 1);
		motor_data.actuator_states[cmd->arg] = 1;
		break;
	case MC_CLR_ACTUATOR:
		actuator_set(cmd->arg, 0);
		motor_data.actuator_states[cmd->arg] = 0;
		break;
	case MC_SET_ACTUATOR_MASK:
		for (i = 0; i < ACTUATORS_NUM; ++i) {
			if (cmd->arg & (1 << i)) {
				actuator_set(i, 1);
				motor_data.actuator_states[i] = 1;
			}
		}
		break;
	case MC_CLR_ACTUATOR_MASK:
		for (i = 0; i < ACTUATORS_NUM; ++i) {
			if (cmd->arg & (1 << i)) {
				actuator_set(i, 0);
				motor_data.actuator_states[i] = 0;
			}
		}
		break;
	case MC_SET_STATES:
		for (i = 0; i < ACTUATORS_NUM; ++i) {
			motor_data.actuator_states[i] = !!(cmd->arg & (1 << i));
			actuator_set(i, motor_data.actuator_states[i]);
		}
		break;
	}

	return 0;
}


void __ISR_AT_VECTOR(_TIMER_2_VECTOR, IPL2SRS) _timer2_isr_h(void) {
	LATKbits.LATK0 = !LATKbits.LATK0;

	/* clear timer 2 interrupt flag */
	IFS0bits.T2IF = 0;
}


void __ISR_AT_VECTOR(_DMA0_VECTOR, IPL5SRS) _dma_comm_module_isr_h(void) {
	/* TODO: stop sending motor data */

	/* check if CRC is valid */
	if (DCRCDATA == 0) {
		motor_cmd_h.ack = ack;
		motor_cmd_h.state = cmd_received;
	} else {
		motor_cmd_h.ack = nack;
		motor_cmd_h.state = done;
	}

	/* seed the CRC */
	DCRCDATA = CRC_SEED;

	/* clear DMA0 interrupt bits */
	DCH0INT &= ~0x000000ff;
	IFS4bits.DMA0IF = 0;
}


void __ISR_AT_VECTOR(_UART1_RX_VECTOR, IPL1SRS) _uart1_rx_isr_h(void) {
	/* clear UART interrupt */
	IFS3bits.U1RXIF = 0;
}


