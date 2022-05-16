/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rfid.h"
#include "rc522.h"
#include "lcd_txt.h"
#include "keypad_4x4.h"
#include "Flash.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
    MODE_ENTERPASS = 0,
    MODE_CURRENPASS = 1,
    MODE_NEWPASS = 2,
    MODE_ADMINPASS = 3,
    MODE_ADDRFID = 4,
    MODE_DELERFID = 5,
    MODE_MENU_ADMIN = 6,
    MODE_BACKUP_DATA = 7
} mode_door;

extern rfid_store_t rfid_store;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t i=0;  
uint8_t key=0;
uint8_t pass[PASS_WORD_LENGTH]={' ',' ',' ',' '};
uint8_t pass1[PASS_WORD_LENGTH]={' ',' ',' ',' '};
uint8_t p_none[PASS_WORD_LENGTH]={' ',' ',' ',' '};
uint8_t str[5];
uint8_t serNum[5];
mode_door mode = MODE_ENTERPASS;
uint8_t flag_backup = 0;

#define OPEN_DOOR() do{ HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, GPIO_PIN_SET);}while(0)
#define CLOSE_DOOR() do{ HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, GPIO_PIN_RESET);}while(0)

void TT_enterPass(char key, uint8_t lengpass, uint8_t *pass, char *password, uint8_t *passIndex, uint8_t *pass1)
{
    static uint8_t pass_flag = 0;
	lcd_gotoxy(1,0);
	lcd_write(1,*(pass1 + 0));
	lcd_write(1,*(pass1 + 1));
	lcd_write(1,*(pass1 + 2));
	lcd_write(1,*(pass1 + 3));

	if (pass_flag == 3)
    {
        lcd_gotoxy(1,1);
        lcd_puts("Watting 30s");
        HAL_Delay(30000);
        lcd_gotoxy(1,1);
        lcd_puts("           ");
        lcd_gotoxy(1,0);
        pass_flag = 0;
    }
	else if(key != ' ' && key != '*' && key != '#')
    {
		if(key == 'D')
		{
			if(*passIndex > 0)
			{
				(*passIndex)--;
				*(pass + *passIndex)=' ';
				*(pass1 + *passIndex)=' ';//*****************************thua
				HAL_Delay(200);
			}
		}
		else
		{
			*(pass + *passIndex) = key;
			*(pass1 + *passIndex) = '*';//*****************************thua
			(*passIndex)++;
			*passIndex= (*passIndex >= lengpass)? lengpass : *passIndex;  
			HAL_Delay(200);
		}
		if(*passIndex==lengpass)
		{
			*passIndex=0;
			uint8_t check = 0;
			for(uint8_t j=0; j<lengpass; j++)
			{
				if(*(password + j) == *(pass + j))
				{
					check++;
				}
			}
			if(check == lengpass)
			{
                pass_flag = 0; //co bao nhap sai so lan mat khau
                OPEN_DOOR();
				HAL_GPIO_WritePin(led_GPIO_Port, led_Pin,GPIO_PIN_RESET); //chan relay cua
                HAL_GPIO_WritePin(loa_GPIO_Port, loa_Pin,GPIO_PIN_SET); //chan relay cua
				lcd_gotoxy(1,0);
				lcd_puts("   --Unlock--");
				HAL_Delay(200);
				for(uint8_t j=0; j<lengpass; j++)
				{
					*(pass + j) = ' ';
					*(pass1 + j) = ' ';//*****************************thua
				}
                HAL_GPIO_WritePin(led_GPIO_Port, led_Pin,GPIO_PIN_SET);
                HAL_GPIO_WritePin(loa_GPIO_Port, loa_Pin,GPIO_PIN_RESET); //chan relay cua
                HAL_Delay(5000);
				lcd_gotoxy(1,0);
				lcd_puts("             ");
                CLOSE_DOOR();
			}
			else
			{
                pass_flag++;
//				HAL_GPIO_WritePin(loa_GPIO_Port,loa_Pin,GPIO_PIN_SET); //chan loa
				lcd_gotoxy(1,0);
				lcd_puts("wrong password!");
				HAL_Delay(700);
				for(uint8_t j=0; j<lengpass; j++)
				{
					*(pass + j) = ' ';
					*(pass1 + j) = ' ';//*****************************thua
				}
				lcd_gotoxy(1,0);
				lcd_puts("               ");
//				HAL_GPIO_WritePin(loa_GPIO_Port,loa_Pin,GPIO_PIN_RESET); //chan loa
			}
		}
	}
}

void TT_newPass(uint8_t key, uint8_t lengpass, uint8_t *pass, char *password, uint8_t *passIndex, mode_door *mode)
{
	lcd_gotoxy(1,0);
	for(uint8_t j = 0; j < lengpass; j++)
	{
		lcd_write(1,*(pass + j));
	}
	
	if(key != ' ' && key != '*' && key != '#'){
		if(key == 'D')
		{
			if(*passIndex > 0)
			{
				(*passIndex)--;
				*(pass + *passIndex)=' ';
				HAL_Delay(200);
			}
		}
		else
		{
			*(pass + *passIndex) = key;
			(*passIndex)++;
			*passIndex= (*passIndex >= lengpass)? lengpass : *passIndex;  
			HAL_Delay(200);
		}
		if(*passIndex==lengpass)
		{
			*passIndex=0;
			uint8_t check = 0;
			for(uint8_t j=0; j<lengpass; j++)
			{
				*(password + j) = *(pass + j);
				check++;
			}
			if(check == lengpass)
			{
				lcd_gotoxy(1,0);
				lcd_puts("Success!");
				HAL_Delay(2000);
				lcd_gotoxy(1,0);
				lcd_puts("        ");
				for(uint8_t j=0; j<lengpass; j++)
				{
					*(pass + j) = ' ';
				}
				*mode = MODE_ENTERPASS;
				lcd_clear();
				lcd_gotoxy(0,0);
				lcd_puts("DOOR PASSWORD");
			}
		}
	}
}

void TT_currenPass(uint8_t key, uint8_t lengpass, uint8_t *pass, char *password, uint8_t *passIndex, mode_door *mode, uint8_t *pass1)
{
	lcd_gotoxy(1,0);
	for(uint8_t j = 0; j < lengpass; j++)
	{
		lcd_write(1,*(pass1 + j));
	}
	
	if(key != ' ' && key != '*' && key != '#'){
		if(key == 'D')
		{
			if(*passIndex > 0)
			{
				(*passIndex)--;
				*(pass + *passIndex)=' ';
				*(pass1 + *passIndex)=' ';//*****************************thua
				HAL_Delay(200);
			}
		}
		else
		{
			*(pass + *passIndex) = key;
			*(pass1 + *passIndex) = '*';//*****************************thua
			(*passIndex)++;
			*passIndex= (*passIndex >= lengpass)? lengpass : *passIndex;  
			HAL_Delay(200);
		}
		if(*passIndex==lengpass)
		{
			*passIndex=0;
			uint8_t check = 0;
			for(uint8_t j=0; j<lengpass; j++)
			{
				if(*(password + j) == *(pass + j))
				{
					check++;
				}
			}
			if(check == lengpass)
			{
				*mode = MODE_NEWPASS;
				for(uint8_t j=0; j<lengpass; j++)
				{
					*(pass + j) = ' ';
					*(pass1 + j) = ' ';//*****************************thua
				}
				lcd_clear();
				lcd_gotoxy(0,0);
				lcd_puts("NEW PASSWORD: ");		
			}
			else
			{
				lcd_gotoxy(1,0);
				lcd_puts("wrong password!");
				HAL_Delay(2000);
				lcd_gotoxy(1,0);
				lcd_puts("               ");
				for(uint8_t j=0; j<lengpass; j++)
				{
					*(pass + j) = ' ';
					*(pass1 + j) = ' ';//*****************************thua
				}
			}	
		}
	}
}

void TT_adminPass(uint8_t key, uint8_t lengpass, uint8_t *pass, char *pass_ad, uint8_t *passIndex, mode_door *mode, uint8_t *pass1)
{
	lcd_gotoxy(1,0);
	for(uint8_t j = 0; j < lengpass; j++)
	{
		lcd_write(1,*(pass1 + j));
	}
	
	if(key != ' ' && key != '*' && key != '#'){
		if(key == 'D')
		{
			if(*passIndex > 0)
			{
				(*passIndex)--;
				*(pass + *passIndex)=' ';
				*(pass1 + *passIndex)=' ';//*****************************thua
				HAL_Delay(200);
			}
		}
		else
		{
			*(pass + *passIndex) = key;
			*(pass1 + *passIndex) = '*';//*****************************thua
			(*passIndex)++;
			*passIndex= (*passIndex >= lengpass)? lengpass : *passIndex;  
			HAL_Delay(200);
		}
		if(*passIndex==lengpass)
		{
			*passIndex=0;
			uint8_t check = 0;
			for(uint8_t j=0; j<lengpass; j++)
			{
				if(*(pass_ad + j) == *(pass + j))
				{
					check++;
				}
			}
			if(check == lengpass)
			{
				*mode = MODE_MENU_ADMIN;
				for(uint8_t j=0; j<lengpass; j++)
				{
					*(pass + j) = ' ';
					*(pass1 + j) = ' ';//*****************************thua
				}
                lcd_clear();
                lcd_gotoxy(0,0);
                lcd_puts("ADMIN MENU: ");
                lcd_gotoxy(1,0);
                lcd_puts("Add Del Chan Bkp");
			}
			else
			{
				lcd_gotoxy(1,0);
				lcd_puts("wrong password!");
				HAL_Delay(2000);
				lcd_gotoxy(1,0);
				lcd_puts("               ");
				for(uint8_t j=0; j<lengpass; j++)
				{
					*(pass + j) = ' ';
					*(pass1 + j) = ' ';//*****************************thua
				}
			}	
		}
	}
}

void TT_menu_admin(uint8_t key, mode_door *mode, uint8_t* flag_backup)
{
    if(key == 'A')
    {
        *mode = MODE_ADDRFID;
        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_puts("ADD NEW RFID: ");
        lcd_gotoxy(1,0);
        lcd_puts("watting card...");
    }
    else if(key == 'D')
    {
       *mode = MODE_DELERFID;
        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_puts("DELETE RFID: ");
        lcd_gotoxy(1,0);
        lcd_puts("watting card...");
    }
    else if(key == 'C')
    {   
        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_puts("CURREN PASSWORD: ");//CURREN PASS
        lcd_gotoxy(1,0);
        *mode = MODE_CURRENPASS;
        memcpy(pass, p_none, 4);
        memcpy(pass1, p_none, 4);
        i = 0; 
    }
    else if(key == 'B')
    {
        *mode = MODE_BACKUP_DATA;
        *flag_backup = 0;
    }     
}

void TT_backup_data(uint8_t* flag_backup, mode_door* mode)
{
    if(*flag_backup == 0)
    {
        rfid_store_data();
        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_puts("BACKUP DATA");
        lcd_gotoxy(1,0);
        lcd_puts("watting...");
        HAL_Delay(1000);
        lcd_gotoxy(1,0);
        lcd_puts("          ");
        lcd_gotoxy(1,0);
        lcd_puts("OK");
        HAL_Delay(2000);
        *flag_backup = 1;
        *mode = MODE_ENTERPASS;
        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_puts("DOOR PASSWORD");
        lcd_gotoxy(1,0);
        
    }
}
void TT_add_rfid(rfid_store_t* rfid_store, mode_door* mode)
{
    uint8_t str[5] = {0};
    if(TM_MFRC522_Request(PICC_REQIDL,str) == MI_OK) //dao ko PHAT HIEN THE
    {
        if(TM_MFRC522_Anticoll(str) == MI_OK)
        {
            if(rfid_get_id(str) == RFID_INVALID_ID)
            {
                /*them the truong hop khong ton tai the do*/
                rfid_add(str);
                *mode = MODE_ENTERPASS;
                lcd_gotoxy(1,0);
                lcd_puts("               ");
                lcd_gotoxy(1,0);
                lcd_puts("0k");
                HAL_Delay(2000);
                lcd_clear();
                lcd_gotoxy(0,0);
                lcd_puts("DOOR PASSWORD");
                lcd_gotoxy(1,0);
                
            }
            else
            {
                *mode = MODE_ENTERPASS;
                lcd_gotoxy(1,0);
                lcd_puts("               ");
                lcd_gotoxy(1,0);
                lcd_puts("already exist");
                HAL_Delay(2000);  
                lcd_clear();
                lcd_gotoxy(0,0);
                lcd_puts("DOOR PASSWORD");
                lcd_gotoxy(1,0);            
            }
        } 
    }
}

void TT_dele_rfid(rfid_store_t* rfid_store, mode_door* mode)
{
    uint8_t str[5] = {0};
    if(TM_MFRC522_Request(PICC_REQIDL,str) == MI_OK) //dao ko PHAT HIEN THE
    {
        if(TM_MFRC522_Anticoll(str) == MI_OK)
        {
            if(rfid_get_id(str) != RFID_INVALID_ID)
            {
                /*them the truong hop khong ton tai the do*/
                rfid_delete(str);
                *mode = MODE_ENTERPASS;
                lcd_gotoxy(1,0);
                lcd_puts("               ");
                lcd_gotoxy(1,0);
                lcd_puts("0k");
                HAL_Delay(2000);
                lcd_clear();
                lcd_gotoxy(0,0);
                lcd_puts("DOOR PASSWORD");
                lcd_gotoxy(1,0);
                
            }
            else
            {
                *mode = MODE_ENTERPASS;
                lcd_gotoxy(1,0);
                lcd_puts("               ");
                lcd_gotoxy(1,0);
                lcd_puts("invalid");
                HAL_Delay(2000);
                lcd_clear();
                lcd_gotoxy(0,0);
                lcd_puts("DOOR PASSWORD");
                lcd_gotoxy(1,0);
            }
        } 
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    rfid_restore_data();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	TM_MFRC522_Init();
	lcd_init();
    HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_SET); //chan led
    HAL_GPIO_WritePin(loa_GPIO_Port, loa_Pin, GPIO_PIN_RESET); //chan loa
    lcd_clear();
    lcd_puts("DOOR PASSWORD");
	lcd_gotoxy(1,0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    key = keypad_get_key_value();
    // thiet lap che do
    if(key == '*' && mode != 0)
    {
        mode = MODE_ENTERPASS;
        i=0;
        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_puts("DOOR PASSWORD");
        lcd_gotoxy(1,0);
    }
    else if(key == '#')
    {
        mode = MODE_ADMINPASS;
        i=0;
        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_puts("ADMIN PASSWORD: ");//ADMIN PASS
        lcd_gotoxy(1,0);
    }

    switch(mode)
    {
        case MODE_ENTERPASS:
            TT_enterPass(key, PASS_WORD_LENGTH, pass, rfid_store.door_pass, &i, pass1);
            if(TM_MFRC522_Request(PICC_REQIDL,str) == MI_OK) //dao ko PHAT HIEN THE
            {
                if(TM_MFRC522_Anticoll(str) == MI_OK)
                {
                    if(rfid_get_id(str) != RFID_INVALID_ID)
                    {
                        OPEN_DOOR();
                        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
                        HAL_GPIO_WritePin(loa_GPIO_Port,loa_Pin,GPIO_PIN_SET); //loa
                        lcd_gotoxy(1,0);
                        lcd_puts ("   --Unlock--");
                        HAL_Delay(200);
                        HAL_GPIO_WritePin(led_GPIO_Port,led_Pin,GPIO_PIN_SET);//led
                        HAL_GPIO_WritePin(loa_GPIO_Port,loa_Pin,GPIO_PIN_RESET);//loa
                        HAL_Delay(5000); 
                        lcd_gotoxy(1,0);
                        lcd_puts ("              ");
                        CLOSE_DOOR();
                        memcpy(pass, p_none, 4);
                        memcpy(pass1, p_none, 4);
                        i = 0; 
                    }
                }
             }
            break;
        case MODE_NEWPASS:
            TT_newPass(key, PASS_WORD_LENGTH, pass, rfid_store.door_pass, &i, &mode);
            break;
        case MODE_CURRENPASS:
            TT_currenPass(key,PASS_WORD_LENGTH, pass, rfid_store.door_pass, &i, &mode, pass1);
            break;
        case MODE_ADMINPASS:
            TT_adminPass(key,PASS_WORD_LENGTH, pass, rfid_store.admin_pass, &i, &mode, pass1);
            break;
        case MODE_ADDRFID:
            TT_add_rfid(&rfid_store, &mode);
            break;
        case MODE_DELERFID:
            TT_dele_rfid(&rfid_store, &mode);
            break;
        case MODE_MENU_ADMIN:
            TT_menu_admin(key, &mode, &flag_backup);
            break;
        case MODE_BACKUP_DATA:
            TT_backup_data(&flag_backup, &mode);
            break;
        default:
            break;
    }

    HAL_Delay(100);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, r1_Pin|r2_Pin|r3_Pin|SDA_Pin
                          |LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin
                          |r4_Pin|relay_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, loa_Pin|LCD_RS_Pin|LCD_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : led_Pin */
  GPIO_InitStruct.Pin = led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(led_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : r1_Pin r2_Pin r3_Pin r4_Pin */
  GPIO_InitStruct.Pin = r1_Pin|r2_Pin|r3_Pin|r4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SDA_Pin LCD_D4_Pin LCD_D5_Pin LCD_D6_Pin
                           LCD_D7_Pin relay_Pin */
  GPIO_InitStruct.Pin = SDA_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin
                          |LCD_D7_Pin|relay_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : c4_Pin c3_Pin c2_Pin c1_Pin */
  GPIO_InitStruct.Pin = c4_Pin|c3_Pin|c2_Pin|c1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : loa_Pin LCD_RS_Pin LCD_EN_Pin */
  GPIO_InitStruct.Pin = loa_Pin|LCD_RS_Pin|LCD_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
