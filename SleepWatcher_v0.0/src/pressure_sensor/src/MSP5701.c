/*
 * MSP5701.c
 *
 *  Created on: 6 wrz 2014
 *      Author: bbogu_000
 */

#include "stdint.h"
#include "LPC11xx.h"
#include "ssp.h"
#include "timer32.h"
#include "gpio.h"
#include "MSP5701.h"

#include "board.h"

#define RESET			0x1E
#define RESULT			0x00,3
#define PROM0			0xA0,2
#define C1				0xA2,2
#define C2				0xA4,2
#define C3				0xA6,2
#define C4				0xA8,2
#define C5				0xAA,2
#define C6				0xAC,2
#define PROM7			0xAE,2

#define PRESURE_256		0x40
#define PRESURE_512		0x42
#define PRESURE_1024	0x44
#define PRESURE_2048	0x46
#define PRESURE_4096	0x48

#define TEMP_256		0x50
#define TEMP_512		0x52
#define TEMP_1024		0x54
#define TEMP_2048		0x56
#define TEMP_4096		0x58

/* POWERS OF 2*/
#define POWER_23 		8388608
#define POWER_21 		2097152
#define POWER_17 		131072
#define POWER_15 		32768
#define POWER_9 		512
#define POWER_8 		256
#define POWER_7 		128





#define SENSOR SEN_SS_PORT,SEN_SS_PIN



static uint16_t c1;
static uint32_t c2;
static uint16_t c3;
static uint16_t c4;
static uint16_t c5;
static uint16_t c6;
static uint16_t prom7;




void MSP5701_reset(){

		uint8_t buf = RESET;

		GPIOSetValue(SENSOR,0);

		SSP_Send(SSP_NUM,&buf,1);

		delay32Ms(1,40);

		GPIOSetValue(SENSOR,1);

}

void MSP5701_read(void* buffer, uint8_t command, uint8_t length){

		uint8_t temp[length];
		uint8_t buf = command;
		GPIOSetValue(SENSOR,0);
		SSP_Send(SSP_NUM,&buf,1);
		SSP_Receive(SSP_NUM,temp,length);
		GPIOSetValue(SENSOR,1);

	switch(length){
	case 2:
		*(uint16_t*)buffer = temp[0]<<8 | temp[1];
		break;
	case 3:
		*(uint32_t*)buffer = temp[0]<<16 | temp[1]<<8 | temp[2];
		break;
	default:
		break;
	}

}

void MSP5701_write(uint8_t command){

		GPIOSetValue(SENSOR,0);
		uint8_t buf = command;
		SSP_Send(SSP_NUM,&buf,1);

		GPIOSetValue(SENSOR,1);

}

void MSP5701_init(){

	MSP5701_reset();
	/* Read and store all coefficients */

	MSP5701_read(&c1,C1);
	MSP5701_read(&c2,C2);
	MSP5701_read(&c3,C3);
	MSP5701_read(&c4,C4);
	MSP5701_read(&c5,C5);
	MSP5701_read(&c6,C6);
	MSP5701_read(&prom7,PROM7);

}

void MSP5701_measure_temp(int32_t* temp){

	MSP5701_write(TEMP_256);
	delay32Ms(1,1);
	uint32_t d2;
	int32_t dT;
	MSP5701_read(&d2,RESULT);
	dT = d2-c5*256;
	*temp = 2000+dT*c6/8388608;

}

void MSP5701_measure_press(int32_t* press){

	uint32_t d2;
	int32_t dT;
	uint32_t d1;
	int64_t off;
	int64_t sens;
	int32_t press_loc;
	MSP5701_write(PRESURE_256);
	delay32Ms(1,1);

	MSP5701_read(&d1,RESULT);

	MSP5701_write(TEMP_256);
	delay32Ms(1,1);
	MSP5701_read(&d2,RESULT);
	dT=d2-c5*POWER_8;
	off = (c2*POWER_17)+((c4*dT)/POWER_7);
	sens = (c1*POWER_15)+((c3*dT)/POWER_9);
	press_loc = (d1*sens/POWER_21 - off)/POWER_15;
	*press = press_loc;



}
