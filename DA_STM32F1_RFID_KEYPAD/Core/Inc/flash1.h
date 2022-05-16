#ifndef _FLASH_H_
#define _FLASH_H_

#include <stm32f1xx_hal.h>
#include <stdint.h>
#include <string.h>

void flash_write_data(uint32_t addr, uint8_t* data_in, uint16_t length);
void flash_read_data(uint32_t addr, uint8_t *data_out, uint16_t length);

#endif /* _FLASH_H_ */
