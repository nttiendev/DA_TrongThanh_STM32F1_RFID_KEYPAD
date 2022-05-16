#include "Flash.h"
#include "stm32f1xx.h"

// Erase flash memorry
void Hal_FlashErase(uint32_t Address)
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef Erase;
	Erase.Banks = 1;
	Erase.NbPages = 1;
	Erase.PageAddress = Address;
	Erase.TypeErase = FLASH_TYPEERASE_PAGES;
	uint32_t PageError;
	HAL_FLASHEx_Erase(&Erase,&PageError);
	HAL_FLASH_Lock();
}
// Write data of rray into flash memorry
void Hal_FlashWrite_Array(uint32_t Address,uint8_t *Array, uint16_t length)
{
	HAL_FLASH_Unlock();
	uint16_t *ptr = (uint16_t*)Array;
	for(uint16_t i=0;i<(length+1)/2;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Address + 2*i, *ptr);
		ptr++;
	}
	HAL_FLASH_Lock();
}

// Read data of rray from flash memorry
void Hal_FlashRead_Array(uint32_t Address, uint8_t *Array, uint16_t length)
{
	uint16_t *ptr = (uint16_t *)Array;
	for(uint16_t i=0;i<(length+1)/2;i++)
	{
		*ptr = *(__IO uint16_t*)(Address + 2*i);ptr++;
	}
}
