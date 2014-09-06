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
#include "storage.h"
#include "SST25VF064C.h"

#define SAMPLE_BUFFER_LENGTH 256
#define SAMPLE_LENGTH 4

#include <cr_section_macros.h>

#define SYSTICK_DELAY		(SystemCoreClock/20)
volatile uint32_t TimeTick = 0;
static uint32_t measure = 0;
char buffer[10];
uint8_t meas_buffer[SAMPLE_BUFFER_LENGTH];
uint32_t meas_pointer = 0;
int32_t temp;
int32_t press;

rec_s actual_record =
  { .rec_index = -1, .rec_length = -1, .rec_ptr = -1, .rec_struct_addr = -1 };

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

/* SysTick interrupt happens every 50 ms */
void
SysTick_Handler (void)
{
  TimeTick = 1;
}

int
main (void)
{

  SystemCoreClockUpdate ();
  GPIOInit ();
  POWER_ON;
  SSP_IOConfig (SSP_NUM); /* initialize SSP port, share pins with SPI1
   on port2(p2.0-3). */
  SSP_Init (SSP_NUM);
  UARTInit (115200);

  TIMInit (0, 2 * SystemCoreClock);
  TIMInit (1, SystemCoreClock);

  // Initialize SYstick
  SysTick_Config ( SYSTICK_DELAY);

  MSP5701_init ();
  SST25_init ();

  //LED1_BLINK;

  // TODO: insert code here

  while (1)
    {

      if (checkEvent(BtnPressed))
	{
	  // Start measuring.
	  //GPIOSetValue(LED1_PORT, LED1_PIN, ~GPIOGetValue(LED1_PORT,LED1_PIN)&0x1);
	  clearEvent(BtnPressed);
	  LED2_ON;
	  delay32Ms (1, 30);
	  LED2_OFF;
	  measure = ~measure;
	  if (measure == 0)
	    {

	      /* Save data info */
	      storage_save_data_info (&actual_record);
	      meas_pointer = 0;
	      LED1_OFF;
	    }
	  else
	    {
	      meas_pointer = 0;
	      storage_init (&actual_record);
	      LED1_ON;
	    }
	  clearEvent(BtnPressed);
	}
      if (checkEvent(PowerOff))
	{
	  // Turn off device.
	  LED2_OFF;
	  storage_save_data_info (&actual_record);
	  meas_pointer = 0;
	  measure = 0;
	  POWER_OFF;
	  while (1)
	    ;
	  clearEvent(PowerOff);

	}
      if (checkEvent(UART_data))
	{
	  uint8_t command = get_uart_char();
	  switch(command){
	    case 'e':
	      SST25_erase(0,SIZE_FULL);
	      break;
	    case 'r':
	      send_data();
	      break;
	    default:
	      break;
	  }
	}

      if (TimeTick & measure)
	{

	  TimeTick = 0;
	  /* Do stuff */

	  MSP5701_measure_press (&press);
	  memcpy ((uint8_t*) (meas_buffer + meas_pointer), &press,
		  sizeof(press));
	  meas_pointer += sizeof(press);
	  if (meas_pointer >= SAMPLE_BUFFER_LENGTH - 1)
	    {
	      /* Store samples in external memory */
	      LED1_TOGGLE;
	      uint32_t length_written = storage_save_data (
		  &actual_record, meas_buffer, SAMPLE_BUFFER_LENGTH);
	      meas_pointer = 0;
	      if (length_written != SAMPLE_BUFFER_LENGTH)
		{
		  /* Run out of memory - stop measuring */
		  measure = 0;
		  meas_pointer = 0;
		  storage_save_data_info (&actual_record);
		  /* Turn off device */
		  POWER_OFF;
		}
	    }

	}
      else
	{
	  __WFI ();
	}

    }
  return 0;
}
