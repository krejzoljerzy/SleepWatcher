/*
 * SST25VF064C.c
 *
 *  Created on: 6 wrz 2014
 *      Author: bbogu_000
 */

#include "stdint.h"
#include "LPC11xx.h"
#include "ssp.h"
#include "timer32.h"
#include "gpio.h"
#include "board.h"
#include "SST25VF064C.h"

/* commands */
#define READ 0x03
#define ERASE_SECTOR 0x20
#define ERASE_32 0x52
#define ERASE_64 0xD8
#define ERASE_CHIP 0x60
#define PROGRAM_PAGE 0x02
#define READ_STATUS 0x05
#define WRITE_ENABLE 0x06
#define WRITE_DISABLE 0x04
#define READ_ID 0x90
#define READ_JEDEC 0x9F
#define ENABLE_STATUS_WRITE 0x50
#define WRITE_STATUS 0x01

#define MEMORY MEM_SS_PORT,MEM_SS_PIN

static uint8_t buf[4];

void SST25_init() {

	SST25_clear_BPs();

}

void SST25_read(uint8_t* buffer, uint32_t address, uint32_t length) {

	while(SST25_is_busy());

	buf[0] = READ;
	buf[1] = (address >> 16) & 0xff;
	buf[2] = (address >> 8) & 0xff;
	buf[3] = (address) & 0xff;

	GPIOSetValue(MEMORY, 0);
	SSP_Send(SSP_NUM, buf, 4);
	SSP_Receive(SSP_NUM, buffer, length);
	GPIOSetValue(MEMORY, 1);

}

uint8_t SST25_read_status() {

	buf[0] = READ_STATUS;

	GPIOSetValue(MEMORY, 0);
	SSP_Send(SSP_NUM, buf, 1);
	SSP_Receive(SSP_NUM, buf, 1);
	GPIOSetValue(MEMORY, 1);

	return buf[0];
}

uint8_t SST25_is_busy() {

	uint8_t status = SST25_read_status();
	return status & 1;
}

void SST25_write(uint8_t* buffer, uint32_t address, uint32_t length) {

	SST25_write_enable();

	while(SST25_is_busy());

	buf[0] = WRITE;
	buf[1] = (address >> 16) & 0xff;
	buf[2] = (address >> 8) & 0xff;
	buf[3] = (address) & 0xff;

	GPIOSetValue(MEMORY, 0);
	SSP_Send(SSP_NUM, buf, 4);
	SSP_Send(SSP_NUM, buffer, length);
	GPIOSetValue(MEMORY, 1);

}

void SST25_write_enable() {

	while(SST25_is_busy());

	buf[0] = WRITE_ENABLE;

	GPIOSetValue(MEMORY, 0);
	SSP_Send(SSP_NUM, buf, 1);
	GPIOSetValue(MEMORY, 1);

}

void SST25_clear_BPs(){

		buf[0] = ENABLE_STATUS_WRITE;

		GPIOSetValue(MEMORY, 0);
		SSP_Send(SSP_NUM, buf, 1);
		GPIOSetValue(MEMORY, 1);

		buf[0] = WRITE_STATUS;
		buf[1] = 0;

		GPIOSetValue(MEMORY, 0);
		SSP_Send(SSP_NUM, buf, 2);
		GPIOSetValue(MEMORY, 1);
}

void SST25_write_disable() {

	while(SST25_is_busy());

	buf[0] = WRITE_DISABLE;

	GPIOSetValue(MEMORY, 0);
	SSP_Send(SSP_NUM, buf, 1);
	GPIOSetValue(MEMORY, 1);

}

void SST25_read_JEDEC(uint8_t* buffer) {

	while(SST25_is_busy());

	buf[0] = READ_JEDEC;

	GPIOSetValue(MEMORY, 0);
	SSP_Send(SSP_NUM, buf, 1);
	SSP_Receive(SSP_NUM, buffer, 3);
	GPIOSetValue(MEMORY, 1);

}

void SST25_erase( uint32_t address, uint8_t type) {

	SST25_write_enable();

	while(SST25_is_busy());

	switch(type){
		case SIZE_4:
			buf[0]=ERASE_SECTOR;
			break;
		case SIZE_32:
			buf[0]=ERASE_32;
			break;
		case SIZE_64:
			buf[0]=ERASE_64;
			break;
		case SIZE_FULL:
			buf[0]=ERASE_CHIP;
			GPIOSetValue(MEMORY, 0);
			SSP_Send(SSP_NUM, buf, 1);
			GPIOSetValue(MEMORY, 1);
			return;
		default:
			SST25_write_disable();
			return;

	}

	buf[1] = (address >> 16) & 0xff;
	buf[2] = (address >> 8) & 0xff;
	buf[3] = (address) & 0xff;

	GPIOSetValue(MEMORY, 0);
	SSP_Send(SSP_NUM, buf, 4);
	GPIOSetValue(MEMORY, 1);

}



//SST25_erase();
