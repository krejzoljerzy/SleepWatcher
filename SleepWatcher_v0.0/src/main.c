/*
 ===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC11xx.h"
#endif

#include "gpio.h"
#include "board.h"
#include "timer32.h"
#include "events.h"

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

int main(void) {

	SystemCoreClockUpdate();

	GPIOInit();

	TIMInit(0, 0);

	SSP_IOConfig(SSP_NUM); /* initialize SSP port, share pins with SPI1
	 on port2(p2.0-3). */
	SSP_Init(SSP_NUM);

	// TODO: insert code here

	// Force the counter to be placed into memory
	// Enter an infinite loop, just incrementing a counter
	// Dispatch events.
	uint8_t bufferTX [1];
	bufferTX[0] = 0x9F;
	uint8_t bufferRX[3]={0};
	GPIOSetValue(MEM_SS_PORT,MEM_SS_PIN,0);

	SSP_Send(SSP_NUM,bufferTX,1);
	SSP_Receive(SSP_NUM,bufferRX,3);

	GPIOSetValue(MEM_SS_PORT,MEM_SS_PIN,1);
	while (1) {

		if (checkEvent(BtnPressed)) {
			// Start measuring.

		}
		if (checkEvent(BtnHold)) {
			// Turn off device.
			clearEvent(BtnHold);
			GPIOSetValue(POWER_PORT, POWER_PIN, 1);
		}

	}
	return 0;
}
