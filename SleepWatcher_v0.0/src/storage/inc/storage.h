/*
 * storage.h
 *
 *  Created on: 6 wrz 2014
 *      Author: bbogu_000
 */

#ifndef STORAGE_H_
#define STORAGE_H_

/* Structure for holding information about record data */
typedef struct {
	int32_t rec_index; // Id number of actual record
	uint32_t rec_ptr; // Pointer to data
	uint32_t rec_length; //Length of actual record
	uint32_t rec_struct_addr; // Address of this structure in external memory
} rec_s;

void storage_init(rec_s* record);
void storage_save_data_info(rec_s* record);
uint32_t storage_save_data(rec_s* record, uint8_t* buffer, uint32_t length);
void read_and_send(uint32_t length,uint32_t address, uint32_t record);
void send_data();




#endif /* STORAGE_H_ */
