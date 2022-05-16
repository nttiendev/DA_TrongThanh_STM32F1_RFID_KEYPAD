#ifndef _Flash_H
#define _Flash_H
#include <stdint.h>

#pragma pack(1)
typedef struct{
	uint8_t no;
	uint8_t ssid[10];
	uint8_t pass[10];
}name;
#pragma pack(1)

void Hal_FlashErase(uint32_t Address);

// Write into flash memorry
void Hal_FlashWrite_Array(uint32_t Address,uint8_t *Array, uint16_t length);

// Read from flash memorry
void Hal_FlashRead_Array(uint32_t Address, uint8_t *Array, uint16_t length);

#endif
