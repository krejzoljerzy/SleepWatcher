/*
 * SST25VF064C.h
 *
 *  Created on: 6 wrz 2014
 *      Author: bbogu_000
 */

#ifndef SST25VF064C_H_
#define SST25VF064C_H_

#define SIZE_4 		4
#define SIZE_32 	32
#define SIZE_64 	64
#define SIZE_FULL 	0xff


void SST25_read(uint8_t* buffer, uint32_t address, uint32_t length);

uint8_t SST25_read_status(void);

uint8_t SST25_is_busy(void);

uint32_t  SST25_write(uint8_t* buffer, uint32_t address, uint32_t length);

void SST25_write_enable(void);

void SST25_write_disable(void);

void SST25_read_JEDEC(uint8_t* buffer);

void SST25_erase( uint32_t address, uint8_t type);

void SST25_init(void);

void SST25_clear_BPs(void);


#endif /* SST25VF064C_H_ */
