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

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

int main(void) {

	SystemCoreClockUpdate();

	GPIOInit();

	/* Enable Hysteresis to eliminate debouncing. */

	LPC_IOCON ->PIO0_2 |= (0x01 << 5);

	GPIOSetDir(LED1_PORT, LED1_PIN, OUTPUT);
	GPIOSetDir(LED2_PORT, LED2_PIN, OUTPUT);
	GPIOSetDir(POWER_PORT, POWER_PIN, OUTPUT);
	GPIOSetDir(BTN_PORT, BTN_PIN, INPUT);
	GPIOSetDir(DETECT_PORT, DETECT_PIN, INPUT);

	/* level sensitive,single edge trigger (don't care), active low. */
	GPIOSetInterrupt(BTN_PORT, BTN_PIN, 1, 0, 0);
	GPIOIntEnable(BTN_PORT, BTN_PIN);
	/* level sensitive,single edge trigger (don't care), active high. */
	GPIOSetInterrupt(DETECT_PORT, DETECT_PIN, 1, 0, 1);
	GPIOIntEnable(DETECT_PORT, DETECT_PIN);



	// TODO: insert code here

	// Force the counter to be placed into memory
	volatile static int i = 0;
	// Enter an infinite loop, just incrementing a counter
	while (1) {
		i++;
	}
	return 0;
}
