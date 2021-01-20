/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "LCD.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Manual 0
#define Auto 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
LCD_STRUCT lcd_setup;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t adcBuffer[2];
uint8_t Rx_Buffer[100];
uint8_t Mode = Auto, HEAT_STATE = 0, LAMP_STATE = 0;
float Temp, Lux;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Parse_AT_Command(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	// Initializing LCD
	lcd_setup.RS_PORT =  RS_GPIO_Port;
	lcd_setup.RS_PIN = RS_Pin;
	
	lcd_setup.RW_PORT =  RW_GPIO_Port;
	lcd_setup.RW_PIN = RW_Pin;
	
	lcd_setup.EN_PORT =  E_GPIO_Port;
	lcd_setup.EN_PIN = E_Pin;

	lcd_setup.D4_PORT =  D4_GPIO_Port;
	lcd_setup.D4_PIN = D4_Pin;
	
	lcd_setup.D5_PORT =  D5_GPIO_Port;
	lcd_setup.D5_PIN = D5_Pin;
	
	lcd_setup.D6_PORT =  D6_GPIO_Port;
	lcd_setup.D6_PIN = D6_Pin;
	
	lcd_setup.D7_PORT =  D7_GPIO_Port;
	lcd_setup.D7_PIN = D7_Pin;
	
	lcd_setup.Port_Count = 4; // 8 or 4 Pins
	lcd_setup.No_Of_Lines = 2; // 1 or 2 Lines
	lcd_setup.Font = 8;//8 or 10// 5*10 or 5*8 Font
	lcd_setup.Blink = 0;// Turn Blinking Cursor On or Off
	lcd_setup.Cursor = 0;// Turn Cursor On or Off
	
	
	LCD_Init(&lcd_setup);
	
	// RUN UART in interrupt mode
	HAL_UART_Receive_IT(&huart1, Rx_Buffer, 100);

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {	
			// check the incoming command
			Parse_AT_Command();

			// START ADC DMA
		  // Didn't use continious mode and dma circular beause Proteus Couldn't simulate
			HAL_ADC_Start_DMA(&hadc1,adcBuffer, 2);
			
			// check for Temperature in Auto Mode
			if(Mode == Auto && Temp < 20.0)
			{
				HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, GPIO_PIN_SET);
				HEAT_STATE = 1;
			}
			else if(Mode == Auto && Temp > 24.0)
			{
				HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, GPIO_PIN_RESET);
				HEAT_STATE = 0;
			}

			// check for LUX in Auto Mode
			if(Mode == Auto && Lux < 400.0)
			{
				HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, GPIO_PIN_SET);
				LAMP_STATE = 1;
			}
			else if(Mode == Auto && Lux > 500.0)
			{
				HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, GPIO_PIN_RESET);
				LAMP_STATE = 0;
			}
			
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// Decode the incomig command
void Parse_AT_Command(void)
{
	char URAT_OutPut[17];
	// Checks Command only after enter(\r\n) pressed
	if(Rx_Buffer[strlen((const char*)Rx_Buffer) - 1] == '\n' || Rx_Buffer[strlen((const char*)Rx_Buffer) - 1] == '\r')
	{
		// Stop Receiving from UART
		HAL_UART_AbortReceive_IT(&huart1);
		// Using strncmp to compare input commad with setted commands
		if(strncmp((const char*)Rx_Buffer,"AT_GetMode",strlen("AT_GetMode")) == 0)
		{
			if(Mode == Auto)
				HAL_UART_Transmit(&huart1,(unsigned char*)"Auto\r\n", strlen("Auto\r\n"),HAL_MAX_DELAY);
			else
				HAL_UART_Transmit(&huart1,(unsigned char*)"Manual\r\n", strlen("Manual\r\n"),HAL_MAX_DELAY);
		}
		else if(strncmp((const char*)Rx_Buffer,"AT_GetIllumination",strlen("AT_GetIllumination")) == 0)
		{	
			sprintf(URAT_OutPut,"%u\r\n",(uint16_t)Lux);
			HAL_UART_Transmit(&huart1,(unsigned char*)URAT_OutPut, strlen(URAT_OutPut),HAL_MAX_DELAY);
		}
		else if(strncmp((const char*)Rx_Buffer,"AT_GetTemperature",strlen("AT_GetTemperature")) == 0)
		{
			sprintf(URAT_OutPut,"%u\r\n",(uint16_t)Temp);
			HAL_UART_Transmit(&huart1,(unsigned char*)URAT_OutPut, strlen(URAT_OutPut),HAL_MAX_DELAY);
		}
		else if(strncmp((const char*)Rx_Buffer,"AT_GetStats_Heater",strlen("AT_GetStats_Heater")) == 0)
		{
			if ( HEAT_STATE == 1)
				HAL_UART_Transmit(&huart1,(unsigned char*)"On\r\n", strlen("On\r\n"),HAL_MAX_DELAY);
			else
				HAL_UART_Transmit(&huart1,(unsigned char*)"Off\r\n", strlen("Off\r\n"),HAL_MAX_DELAY);
		}
		else if(strncmp((const char*)Rx_Buffer,"AT_GetStats_Lamp",strlen("AT_GetStats_Lamp")) == 0)
		{
			if ( LAMP_STATE == 1)
				HAL_UART_Transmit(&huart1,(unsigned char*)"On\r\n", strlen("On\r\n"),HAL_MAX_DELAY);
			else
				HAL_UART_Transmit(&huart1,(unsigned char*)"Off\r\n", strlen("Off\r\n"),HAL_MAX_DELAY);
		}
		else if(strncmp((const char*)Rx_Buffer,"AT_SetMode_Auto",strlen("AT_SetMode_Auto")) == 0)
		{
			Mode = Auto;
			HAL_UART_Transmit(&huart1,(unsigned char*)"Mode Set to Auto\r\n", strlen("Mode Set to Auto\r\n"),HAL_MAX_DELAY);				
		}
		else if(strncmp((const char*)Rx_Buffer,"AT_SetMode_Manual",strlen("AT_SetMode_Manual")) == 0)
		{
			Mode = Manual;
			HAL_UART_Transmit(&huart1,(unsigned char*)"Mode Set to Manual\r\n", strlen("Mode Set to Manual\r\n"),HAL_MAX_DELAY);
		}
		else if(strncmp((const char*)Rx_Buffer,"AT_TurnOff_Heater",strlen("AT_TurnOff_Heater")) == 0)
		{
			// Before changing output checks if mode is manual
			if(Mode == Manual)
			{
				HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, GPIO_PIN_RESET);
				HEAT_STATE = 0;
				HAL_UART_Transmit(&huart1,(unsigned char*)"Heater Turned Off\r\n", strlen("Heater Turned Off\r\n"),HAL_MAX_DELAY);
			}
			else
				HAL_UART_Transmit(&huart1,(unsigned char*)"MODE IS NOT MANUAL\r\n", strlen("MODE IS NOT MANUAL\r\n"),HAL_MAX_DELAY);
		}
		else if(strncmp((const char*)Rx_Buffer,"AT_TurnOff_Lamp",strlen("AT_TurnOff_Lamp")) == 0)
		{
			// Before changing output checks if mode is manual
			if(Mode == Manual)
			{
			HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, GPIO_PIN_RESET);
			LAMP_STATE = 0;
			HAL_UART_Transmit(&huart1,(unsigned char*)"Lamp Turned Off\r\n", strlen("Lamp Turned Off\r\n"),HAL_MAX_DELAY);
			}
			else
				HAL_UART_Transmit(&huart1,(unsigned char*)"MODE IS NOT MANUAL\r\n", strlen("MODE IS NOT MANUAL\r\n"),HAL_MAX_DELAY);
		}
		else if(strncmp((const char*)Rx_Buffer,"AT_TurnOn_Heater",strlen("AT_TurnOn_Heater")) == 0)
		{
			// Before changing output checks if mode is manual
			if(Mode == Manual)
			{
			HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, GPIO_PIN_SET);
			HEAT_STATE = 1;
			HAL_UART_Transmit(&huart1,(unsigned char*)"Heater Turned On\r\n", strlen("Heater Turned On\r\n"),HAL_MAX_DELAY);
			}
			else
				HAL_UART_Transmit(&huart1,(unsigned char*)"MODE IS NOT MANUAL\r\n", strlen("MODE IS NOT MANUAL\r\n"),HAL_MAX_DELAY);
		}
		else if(strncmp((const char*)Rx_Buffer,"AT_TurnOn_Lamp",strlen("AT_TurnOn_Lamp")) == 0)
		{
			// Before changing output checks if mode is manual
			if(Mode == Manual)
			{
			HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, GPIO_PIN_SET);
			LAMP_STATE = 1;
			HAL_UART_Transmit(&huart1,(unsigned char*)"Lamp Turned On\r\n", strlen("Lamp Turned On\r\n"),HAL_MAX_DELAY);
			}
			else
				HAL_UART_Transmit(&huart1,(unsigned char*)"MODE IS NOT MANUAL\r\n", strlen("MODE IS NOT MANUAL\r\n"),HAL_MAX_DELAY);
		}
		else
		{
			// In case of not valid input
			HAL_UART_Transmit(&huart1,(unsigned char*)"UNKNOWN COMMAND\r\n", strlen("UNKNOWN COMMAND\r\n"),HAL_MAX_DELAY);
		}
		
		// Empty Buffer
		for(int i =0 ; i < 100; i++)
			Rx_Buffer[i] = 0;
		// ReStart Receiving from UART
		HAL_UART_Receive_IT(&huart1, Rx_Buffer, 100);
	}
}

// ReRun UART if Buffer is Full
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
		HAL_UART_Receive_IT(&huart1, Rx_Buffer, 100);
}

// Print Data On LCD in each ADC Convert Complete
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	char LCD_String[17];
	// convert adc value to temp
	Temp = adcBuffer[1] * 330 / 4095;
	// convert adc value to lux
	Lux = 630 / pow(((4504.5 / adcBuffer[0]) - 1.1),1.25);
	
	sprintf(LCD_String,"T = %5u DEG", (uint16_t)Temp);
	LCD_SetCursor(&lcd_setup,0,0);
	LCD_PutString(&lcd_setup,LCD_String);

	sprintf(LCD_String,"I = %5u LUX", (uint16_t)Lux);
	LCD_SetCursor(&lcd_setup,1,0);
	LCD_PutString(&lcd_setup,LCD_String);	
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
