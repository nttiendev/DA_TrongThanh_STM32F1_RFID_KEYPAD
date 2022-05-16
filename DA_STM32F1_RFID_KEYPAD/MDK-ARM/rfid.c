#include "rfid.h"

rfid_store_t rfid_store;

/* Get id of card in database. If card is not in database, return RFID_INVALID_ID */
uint16_t rfid_get_id(uint8_t *rfid)
{
    /* Check match with all card id in database */
    for(uint16_t i = 0; i < rfid_store.count; i++){
        if(memcmp(rfid, rfid_store.rfid[i], RFID_LENGTH) == 0){
            return i;
        }
    }
    return RFID_INVALID_ID;
}

/* Add card id to database */
uint8_t rfid_add(uint8_t *rfid){
    if(rfid_store.count >= RFID_MAX_COUNT){
        /* Database is full */
        return 0;
    }
    
    /* Copy card id to buffer */
    memcpy(rfid_store.rfid[rfid_store.count], rfid, RFID_LENGTH);
    rfid_store.count++;
    return 1;
}

uint8_t rfid_delete(uint8_t *rfid)
{
    uint16_t id = rfid_get_id(rfid);
    /* Card id is available */
    if(id != RFID_INVALID_ID){
        memcpy(rfid_store.rfid[id], rfid_store.rfid[--rfid_store.count], RFID_LENGTH);
        return 1;
    }
    /* Couldn't delete */
    return 0;
}

void rfid_store_data(void){
    Hal_FlashErase(addr_backup);
    Hal_FlashWrite_Array(addr_backup, (uint8_t *)&rfid_store, sizeof(rfid_store));
}

void rfid_restore_data(void){
    Hal_FlashRead_Array(addr_backup, (uint8_t *)&rfid_store, sizeof(rfid_store));
    /* First time after boot, set default value */
    if(rfid_store.count >= RFID_MAX_COUNT){
        rfid_store.count = 0;
        memcpy(rfid_store.admin_pass, ADMIN_PASS_DEFAULT, PASS_WORD_LENGTH);
        memcpy(rfid_store.door_pass, DOOR_PASS_DEFAULT, PASS_WORD_LENGTH);
    }
}

void rfid_restore_data_buff(uint8_t* data_read, uint16_t size_buff){
    Hal_FlashRead_Array(addr_backup, data_read, size_buff);
    /* First time after boot, set default value */
    if(rfid_store.count >= RFID_MAX_COUNT){
        rfid_store.count = 0;
        memcpy(rfid_store.admin_pass, ADMIN_PASS_DEFAULT, PASS_WORD_LENGTH);
        memcpy(rfid_store.door_pass, DOOR_PASS_DEFAULT, PASS_WORD_LENGTH);
    }
}
