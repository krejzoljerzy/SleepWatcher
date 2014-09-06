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
#include "MSP5701.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "SST25VF064C.h"

#include <cr_section_macros.h>

#define SYSTICK_DELAY		(SystemCoreClock/20)
volatile uint32_t TimeTick=0;
static uint32_t measure=0;
char buffer[10];
uint8_t buf8[10];

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
	POWER_ON;
	SSP_IOConfig(SSP_NUM); /* initialize SSP port, share pins with SPI1
	 on port2(p2.0-3). */
	SSP_Init(SSP_NUM);
	UARTInit(115200);



	TIMInit(0, 2*SystemCoreClock);
	TIMInit(1, SystemCoreClock);

	// Initialize SYstick
	SysTick_Config( SYSTICK_DELAY );

	/* Enable AHB clock to the GPIO domain. */


	MSP5701_init();
	SST25_init();

	int32_t temp;
	int32_t press;
	MSP5701_measure_temp(&temp);



	//LED1_BLINK;



	// TODO: insert code here


	while (1) {


		if (checkEvent(BtnPressed)) {
			// Start measuring.
			//GPIOSetValue(LED1_PORT, LED1_PIN, ~GPIOGetValue(LED1_PORT,LED1_PIN)&0x1);
			clearEvent(BtnPressed);
			measure = ~measure;
			if(measure==0){
				LED1_OFF;
			}
			clearEvent(BtnPressed);
		}
		if (checkEvent(PowerOff)) {
			// Turn off device.
			LED2_OFF;
			POWER_OFF;
			measure=0;
			clearEvent(PowerOff);

		}

		if(TimeTick&measure)
		{

			TimeTick=0;
			/* Do stuff */
			LED1_TOGGLE;
			MSP5701_measure_press(&press);
			memset(buffer,0,sizeof(buffer));
			itoa(press,buffer,10);
			UARTSend((uint8_t*)buffer,strlen(buffer));
			UARTSend((uint8_t*)"\r\n",2);

		}
		else {

			__WFI();
		}

	}
	return 0;
}
