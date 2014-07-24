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
#include "ssp.h"
<<<<<<< origin/master
=======
#include "uart.h"
>>>>>>> local

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

int main(void) {

	SystemCoreClockUpdate();

	GPIOInit();



	TIMInit(0, 2*SystemCoreClock); // 120000000/12000*2000 = 2000ms
	TIMInit(1, SystemCoreClock); // 120000000/12000*2000 = 2000ms

	POWER_ON;
	LED1_ON;

	SSP_IOConfig(SSP_NUM); /* initialize SSP port, share pins with SPI1
	 on port2(p2.0-3). */
	SSP_Init(SSP_NUM);
	UARTInit(115200);

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
			//GPIOSetValue(LED1_PORT, LED1_PIN, ~GPIOGetValue(LED1_PORT,LED1_PIN)&0x1);
			clearEvent(BtnPressed);

			UARTSend("krotko\r\n",8);
			clearEvent(BtnPressed);
		}
		if (checkEvent(PowerOff)) {
			// Turn off device.
<<<<<<< origin/master
			clearEvent(BtnHold);
			//GPIOSetValue(LED2_PORT, LED2_PIN, ~GPIOGetValue(LED2_PORT,LED2_PIN)&0x1);
			//GPIOSetValue(POWER_PORT, POWER_PIN, 1);
=======
			LED2_OFF;
			UARTSend("dlugo\r\n",7);
			clearEvent(PowerOff);

>>>>>>> local
		}

	}
	return 0;
}
