#include "flash.h"

uint8_t lengthPage;

static void flash_lock()
{
	HAL_FLASH_Lock();
}

static void flash_unlock()
{
	HAL_FLASH_Unlock();
}

static void flash_erase(uint32_t addr)
{
  while((FLASH->SR&FLASH_SR_BSY));
  FLASH->CR |= FLASH_CR_PER; //Page Erase Set
  FLASH->AR = addr; //Page Address
  FLASH->CR |= FLASH_CR_STRT; //Start Page Erase
  while((FLASH->SR&FLASH_SR_BSY));
	FLASH->CR &= ~FLASH_SR_BSY;
  FLASH->CR &= ~FLASH_CR_PER; //Page Erase Clear
}

void flash_write_data(uint32_t addr, uint8_t* data_in, uint16_t length)
{
    uint16_t write_length = (length + 1) / 2;
    int i;
    
    flash_unlock();
	flash_erase(addr);
    
    FLASH->CR |= FLASH_CR_PG;
    int var = 0;
    for(i = 0; i < write_length; i++)
    {
        while((FLASH->SR&FLASH_SR_BSY));
        var = (int)data_in[i];
        *(__IO uint16_t*)(addr + i*2) = var;
    }
    while((FLASH->SR&FLASH_SR_BSY)){};
    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->CR |= FLASH_CR_LOCK;
        
    flash_lock();
}

void flash_read_data(uint32_t addr, uint8_t *data_out, uint16_t length)
{
	uint16_t *psrc, *pdest;
    uint16_t read_length = (length + 1) / 2;
    
    psrc = (uint16_t *) addr;
    pdest = (uint16_t *) data_out;
    while(read_length--){
        *pdest++ = *psrc++;
    }
}
