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
#include "uart.h"

#include <cr_section_macros.h>

#define SYSTICK_DELAY		(SystemCoreClock/20)
volatile uint32_t TimeTick=0;
static uint32_t measure=0;

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler(void)
{
  TimeTick=1;
}


static void start_systick()
{
  /* Clear SysTick Counter */
  SysTick->VAL = 0;
  /* Enable the SysTick Counter */
  SysTick->CTRL |= (0x1<<0);
  return;
}

static void stop_systick()
{
  /* Disable SysTick Counter */
  SysTick->CTRL &= ~(0x1<<0);
  /* Clear SysTick Counter */
  SysTick->VAL = 0;
  return;
}

int main(void) {

	SystemCoreClockUpdate();

	GPIOInit();



	TIMInit(0, 2*SystemCoreClock);
	TIMInit(1, SystemCoreClock);

	POWER_ON;
	//LED1_BLINK;

	SSP_IOConfig(SSP_NUM); /* initialize SSP port, share pins with SPI1
	 on port2(p2.0-3). */
	SSP_Init(SSP_NUM);
	UARTInit(115200);

	// Initialize SYstick
	SysTick_Config( SYSTICK_DELAY );

	/* Enable AHB clock to the GPIO domain. */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

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

	bufferTX[0]=0x1E;

	GPIOSetValue(SEN_SS_PORT,MEM_SS_PIN,0);

	SSP_Send(SSP_NUM,bufferTX,1);

	delay32Ms(1,40);

	GPIOSetValue(SEN_SS_PORT,MEM_SS_PIN,1);

	bufferTX[0]=0xA0;

	GPIOSetValue(SEN_SS_PORT,MEM_SS_PIN,0);

	SSP_Send(SSP_NUM,bufferTX,1);
	SSP_Receive(SSP_NUM,bufferRX,2);

	GPIOSetValue(SEN_SS_PORT,MEM_SS_PIN,1);

	while (1) {

		if (checkEvent(BtnPressed)) {
			// Start measuring.
			//GPIOSetValue(LED1_PORT, LED1_PIN, ~GPIOGetValue(LED1_PORT,LED1_PIN)&0x1);
			clearEvent(BtnPressed);
			measure = ~measure;
			if(measure==0){
				LED1_OFF;
			}
			UARTSend("krotko\r\n",8);
			clearEvent(BtnPressed);
		}
		if (checkEvent(PowerOff)) {
			// Turn off device.
			LED2_OFF;
			POWER_OFF;
			measure=0;
			UARTSend("dlugo\r\n",7);
			clearEvent(PowerOff);

		}

		if(TimeTick&measure)
		{
			stop_systick();
			TimeTick=0;
			/* Do stuff */
			LED1_TOGGLE;
			start_systick();
		}
		else {
			start_systick();
			__WFI();
		}

	}
	return 0;
}
