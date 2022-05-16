#ifndef _RFID_H_
#define _RFID_H_

#include <stdint.h>
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "Flash.h"

#define RFID_LENGTH        5
#define PASS_WORD_LENGTH   4
#define RFID_MAX_COUNT     100
#define RFID_INVALID_ID    0xFFFF
#define ADMIN_PASS_DEFAULT "1234"
#define DOOR_PASS_DEFAULT  "1234"
#define addr_backup ((uint32_t)0x0801FC00)

//#define FLASH_ADDRESS_START ((uint32_t)0x0801FC00)
#pragma pack(1)
typedef struct {
    uint16_t count;
    char admin_pass[PASS_WORD_LENGTH];
    char door_pass[PASS_WORD_LENGTH];
    uint8_t rfid[RFID_MAX_COUNT][RFID_LENGTH];
} rfid_store_t;

/* Get id of card in database. If card is not in database, return RFID_INVALID_ID */
uint16_t rfid_get_id(uint8_t *rfid);
/* Add card id to database */
uint8_t rfid_add(uint8_t *rfid);
uint8_t rfid_delete(uint8_t *rfid);
void rfid_store_data(void);
void rfid_restore_data(void);
void rfid_restore_data_buff(uint8_t* data_read, uint16_t size_buff);

#endif /* _RFID_H_ */
