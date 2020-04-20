/*
 * Main source file for the motor control system of the rocket
 * 
 * Authors:
 *	  Simon Gaudy
 *	  Maxime Guillemette
 *	  Elliot Lafront
 * 
 * Brief:
 *	  Module to control the rocket's launch sequence
 */


#ifndef _MAIN_H_
#define _MAIN_H_


/* standard lib includes */
#include "config_bits.h"
#include "xc.h"
#include <sys/kmem.h>
#include <sys/attribs.h>

/* lib includes */
#include "rocket-packet/rocket_packet.h"
#include "actuator.h"
#include "manometer.h"


/*
 * pinout definitions:
 * 	COMM MODULE: RX: D2, TX: D3
 */
#define UART1_RX_PS() (U1RXR = 0b0000)
#define UART1_TX_PS() (RPD3R = 0b0001)

/* CRC definitions, see rocket packet documentation for details */
#define CRC_POLY 0x1021
#define CRC_LEN 16

/*
 * warning: the hardware CRC is using the indirect algorithm,
 * we need to convert the direct seed value (0xffff) to 0x84cf
 * to get the seed for the indirect algorithm
 */
#define CRC_SEED 0x84cf

/* buffer for commands from the communications module */
#define CMD_RX_BUF_SIZE CMD_PACKET_SIZE
char __attribute__((coherent)) cmd_rx_buf[CMD_RX_BUF_SIZE];

#define ACK_TX_BUF_SIZE ACK_PACKET_SIZE
char __attribute__((coherent)) ack_tx_buf[ACK_TX_BUF_SIZE];


/* global struct to manage motor commands */
struct {
	enum {idle, cmd_received, waiting, done} state;
	enum {none, ack=1, nack=0xff} ack;
	CommandPacket cmd;
	AckPacket ackpkt;
} motor_cmd_h;


/* global motor data */
struct {
	union {
		MotorData data;
		char buf[MOTOR_DATA_SIZE];
	};
} motor_data;


/* public functions */
int init_all(void);
int init_sdcard(void);
int init_timer();
int init_interrupts(void);
int enable_dma(void);
int init_comm_module_uart(void);
int init_comm_module_dma(void);
int comm_module_write(char* src, int size);
int execute_motor_cmd(CommandPacket* cmd);


#endif /* _MAIN_H_ */
