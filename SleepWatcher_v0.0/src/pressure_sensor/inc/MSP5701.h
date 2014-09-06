/*
 * MSP5701.h
 *
 *  Created on: 6 wrz 2014
 *      Author: bbogu_000
 */

#ifndef MSP5701_H_
#define MSP5701_H_

/*
 * MSP5701.c
 *
 *  Created on: 6 wrz 2014
 *      Author: bbogu_000
 */





void MSP5701_reset(void);

void MSP5701_init(void);

void MSP5701_read(void* buffer, uint8_t command, uint8_t length);

void MSP5701_write(uint8_t command);

void MSP5701_measure_temp(int32_t* temp);





#endif /* MSP5701_H_ */
