/*
 * storage.c
 *
 *  Created on: 6 wrz 2014
 *      Author: bbogu_000
 */
#include "stdint.h"
#include "SST25VF064C.h"
#include "storage.h"
#include "string.h"
#include "uart.h"
#define STORAGE_INFO_ADDRESS 0
#define STORAGE_DATA_ADDRESS 0x1000
static uint8_t buffer[256];

rec_s previous_structure;
rec_s read_structure;

void storage_init(rec_s* record) {

	/* Read actual storage from external memory */
	/* Read flash until empty slot found */
	uint32_t pointer = 0;
	do {
		if(record->rec_struct_addr!=(uint32_t)-1){
		    /*We have a good pointer */
		    pointer=record->rec_struct_addr+sizeof(*record);
		}
		SST25_read((uint8_t*) record, STORAGE_INFO_ADDRESS + pointer,
				sizeof(*record));
		record->rec_struct_addr = pointer;
		pointer += sizeof(*record);

	} while (record->rec_index != -1
			&& STORAGE_INFO_ADDRESS + pointer < STORAGE_DATA_ADDRESS); /* -1 means all FFs */

	if (STORAGE_INFO_ADDRESS + pointer - sizeof(*record) >= STORAGE_DATA_ADDRESS) {
		/* There's no place for new data - clear all memory and assign first as first */
		SST25_erase(0, SIZE_FULL);
		record->rec_index = 1;
		record->rec_length = 0;
		record->rec_ptr = STORAGE_DATA_ADDRESS;
		record->rec_struct_addr = STORAGE_INFO_ADDRESS;

	} else if (pointer - sizeof(*record) == 0) {
		/* First structure */
		record->rec_index = 1;
		record->rec_length = 0;
		record->rec_ptr = STORAGE_DATA_ADDRESS;
		record->rec_struct_addr = STORAGE_INFO_ADDRESS;
	} else {
		/* Found slot */
		/* Read previous structure and iterate data */

		SST25_read((uint8_t*)&previous_structure, record->rec_struct_addr - sizeof(*record),sizeof(*record));
		record->rec_index=previous_structure.rec_index+1;
		record->rec_length=0;
		record->rec_ptr = previous_structure.rec_ptr + previous_structure.rec_length;
		record->rec_struct_addr = previous_structure.rec_struct_addr+sizeof(previous_structure);

	}

}

void storage_save_data_info(rec_s* record) {
	SST25_write((uint8_t*) record, record->rec_struct_addr, sizeof(*record));
}

uint32_t storage_save_data(rec_s* record, uint8_t* buffer, uint32_t length) {

	uint32_t length_written = SST25_write(buffer, record->rec_ptr+record->rec_length, length);
	record->rec_length += length_written;
	return length_written;
}

void send_data(){
  /* Parse structure in flash */
  uint64_t pointer=0;
  SST25_read((uint8_t*)&read_structure, STORAGE_INFO_ADDRESS + pointer,sizeof(read_structure));
  while(read_structure.rec_index!=-1){

      if (read_structure.rec_length!=0){
	  read_and_send(read_structure.rec_length,read_structure.rec_ptr,read_structure.rec_index);
      }
      /* Get next structure */
      pointer+=sizeof(read_structure);
      SST25_read((uint8_t*)&read_structure, STORAGE_INFO_ADDRESS + pointer,sizeof(read_structure));
  }
}

void read_and_send(uint32_t length,uint32_t address, uint32_t record){
  uint32_t data_sent=0;
  uint16_t packet_length=0;
  UARTSend("Record_",sizeof("Record_"));
  uint8_t byte_array[4];
  byte_array[0]=(record>>24)&0xFF;
  byte_array[1]=(record>>16)&0xFF;
  byte_array[2]=(record>>8)&0xFF;
  byte_array[3]=(record)&0xFF;
  UARTSend(byte_array,4);
  while (length>0){
      if(length>=256){
	  packet_length=256;
      }else{
	  packet_length=length;
      }
      SST25_read(buffer,address+data_sent,packet_length);
      UARTSend(buffer,packet_length);

      data_sent+=packet_length;
      length-=packet_length;
  }
  UARTSend("\r\n",2);
}

