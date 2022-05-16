
#include "stm32f1xx_hal.h"
#include "main.h"

#define __KEYPAD_4X4    					1             

		//HARAP DIBACA//
//Konfigurasi pada CUBE MX
//PB12-PB15 ==> GPIO_INPUT
//PA8-PA11	==> GPIO_OUTPUT

// Deklarasikan Port sambungan Keypad 
#if __KEYPAD_4X4
// inisalisasi Keypad  

#define R1_PORT r1_GPIO_Port
#define R1_PIN r1_Pin

#define R2_PORT r2_GPIO_Port
#define R2_PIN r2_Pin

#define R3_PORT r3_GPIO_Port
#define R3_PIN r3_Pin

#define R4_PORT r4_GPIO_Port
#define R4_PIN r4_Pin

#define C1_PORT c1_GPIO_Port
#define C1_PIN c1_Pin

#define C2_PORT c2_GPIO_Port
#define C2_PIN c2_Pin

#define C3_PORT c3_GPIO_Port
#define C3_PIN c3_Pin

#define C4_PORT c4_GPIO_Port
#define C4_PIN c4_Pin

//


void keypad_init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure; 
	/********************** Init GPIO Keypad *************************/
 	// aktivasi Kolom bit
	GPIO_InitStructure.Pin = C1_PIN|C2_PIN|C3_PIN|C4_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// aktivasi Baris bit
	GPIO_InitStructure.Pin = R1_PIN|R2_PIN|R3_PIN|R4_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	HAL_GPIO_WritePin(GPIOA,R1_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA,R2_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA,R3_PIN,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA,R4_PIN,GPIO_PIN_SET);
/********************** End GPIO LCD ********************/
}


char keypad_get_key_value(void)
{     char key;
			key=' ';
      HAL_GPIO_WritePin(GPIOA,R1_PIN,GPIO_PIN_RESET);
			HAL_Delay(10);
      if(HAL_GPIO_ReadPin(C1_PORT,C1_PIN)==0) key = '1'; 
      else if(HAL_GPIO_ReadPin(C2_PORT,C2_PIN)==0) key ='2';
      else if(HAL_GPIO_ReadPin(C3_PORT,C3_PIN)==0) key ='3';
      else if(HAL_GPIO_ReadPin(C4_PORT,C4_PIN)==0) key ='A';
      HAL_GPIO_WritePin(GPIOA,R1_PIN,GPIO_PIN_SET);
      
			HAL_GPIO_WritePin(GPIOA,R2_PIN,GPIO_PIN_RESET);
			HAL_Delay(10);
      if(HAL_GPIO_ReadPin(C1_PORT,C1_PIN)==0) key = '4'; 
      else if(HAL_GPIO_ReadPin(C2_PORT,C2_PIN)==0) key ='5';
      else if(HAL_GPIO_ReadPin(C3_PORT,C3_PIN)==0) key ='6';
      else if(HAL_GPIO_ReadPin(C4_PORT,C4_PIN)==0) key ='B';
      HAL_GPIO_WritePin(GPIOA,R2_PIN,GPIO_PIN_SET);
		
 			HAL_GPIO_WritePin(GPIOA,R3_PIN,GPIO_PIN_RESET);
			HAL_Delay(10);
      if(HAL_GPIO_ReadPin(C1_PORT,C1_PIN)==0) key = '7'; 
      else if(HAL_GPIO_ReadPin(C2_PORT,C2_PIN)==0) key ='8';
      else if(HAL_GPIO_ReadPin(C3_PORT,C3_PIN)==0) key ='9';
      else if(HAL_GPIO_ReadPin(C4_PORT,C4_PIN)==0) key ='C';
      HAL_GPIO_WritePin(GPIOA,R3_PIN,GPIO_PIN_SET);
	
 			HAL_GPIO_WritePin(GPIOA,R4_PIN,GPIO_PIN_RESET);
			HAL_Delay(10);
      if(HAL_GPIO_ReadPin(C4_PORT,C1_PIN)==0) key = '*'; 
      else if(HAL_GPIO_ReadPin(C2_PORT,C2_PIN)==0) key ='0';
      else if(HAL_GPIO_ReadPin(C3_PORT,C3_PIN)==0) key ='#';
      else if(HAL_GPIO_ReadPin(C4_PORT,C4_PIN)==0) key ='D';
      HAL_GPIO_WritePin(GPIOA,R4_PIN,GPIO_PIN_SET);
      
			return key;
}

#endif
