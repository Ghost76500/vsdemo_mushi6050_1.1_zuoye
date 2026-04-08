/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "global.h"
#include <string.h>
#include <stdio.h>
#include "OLED.h"
#include "mpu6050.h"
#include "cycle_task.h"
#include "command.h"
#include "data_send.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float pitch, roll, yaw;
float temp;
char pitch_buf[16];
char roll_buf[16];
char yaw_buf[16];

uint8_t readBuffer[50];
float rx_yaw_deg = 0.0f;
float rx_yaw_rad = 0.0f;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void uart_send_data(UART_HandleTypeDef *huart)
{
  if (flag_data_transmission_task)
    {
        flag_data_transmission_task = 0; // 重置标志位
    
        // 将三个数据通过usart1 dma发送
        static char usart_buf[96];
    
        sprintf(usart_buf, "Data:%.2f,%.2f,%.2f\n", pitch, roll, yaw);
        HAL_UART_Transmit_DMA(huart, (uint8_t*)usart_buf, strlen(usart_buf));
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if (huart == &huart2){
		Command_Write(readBuffer, Size);
		HAL_UARTEx_ReceiveToIdle_IT(&huart2, readBuffer, sizeof(readBuffer));
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
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  // 初始化OLED
  HAL_Delay(20); // 等待OLED电源稳定
  OLED_Init();
  OLED_ShowString(0, 0, "OLED Init OK", OLED_8X16);
  OLED_Update();
  HAL_Delay(1000); // 显示1秒
  OLED_Clear();
  
  // 初始化MPU6050
  if(MPU_Init() == 0)
  {
    OLED_ShowString(0, 0, "MPU6050 OK", OLED_8X16);
    OLED_Update();
    HAL_Delay(300);
    OLED_Clear();
    OLED_ShowString(0, 0, "Calib Gyro...", OLED_8X16);
    OLED_Update();
    MPU_Calibrate_Gyro(800); // 约800*2ms≈1.6s静止平均
    OLED_Clear();
    OLED_ShowString(0, 0, "Calib Done", OLED_8X16);
  }
  else
  {
    OLED_ShowString(0, 0, "MPU6050 ERR", OLED_8X16);
  }
  OLED_Update();
  HAL_Delay(1000);
  OLED_Clear();

  // 【新手避坑神技】：在开启空闲中断前，先清除可能因为开机延时导致的 ORE(溢出) 错误！
  // 1. 清除 ORE 标志位 (STM32F1系列是通过读取 SR 寄存器后读取 DR 寄存器来清除的，HAL库提供了宏定义)
  __HAL_UART_CLEAR_OREFLAG(&huart2); 
  // 2. 强行将串口状态恢复为 READY
  huart2.RxState = HAL_UART_STATE_READY;
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, readBuffer, sizeof(readBuffer));
  uint8_t command[50];
  int commandLength = 0;
  
  HAL_TIM_Base_Start_IT(&htim4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
   while ((commandLength = Command_GetCommand(command)) > 0) {
      if (commandLength == 11) {
        // 将高8位和低8位拼接成 16 位的原始数据
        int16_t yaw_raw = (int16_t)(((uint16_t)command[7] << 8) | command[6]);
        // 按照维特智能的官方公式，将原始数据转换为实际角度 (度)
        rx_yaw_deg = (float)yaw_raw * 180.0f / 32768.0f;
        // 将角度转换为弧度 (用于后续可能的数学计算)
        rx_yaw_rad = rx_yaw_deg * 0.0174532925f;
      }
    }
    // 采样并更新姿态角缓存
    MPU_Update_Attitude();

    // 仅读取MPU6050姿态角缓存
    //MPU_Get_Pitch_Roll_Yaw(&pitch, &roll, &yaw);
    //temp = MPU_Get_Temperature();
    // 显示在OLED上
    
    OLED_Clear();
    
    // 仅显示串口接收得到的两个变量
    OLED_ShowString(0, 0, "YawDeg:", OLED_8X16);
    sprintf(pitch_buf, "%.2f", rx_yaw_deg);
    OLED_ShowString(56, 0, pitch_buf, OLED_8X16);

    OLED_ShowString(0, 16, "YawRad:", OLED_8X16);
    sprintf(roll_buf, "%.3f", rx_yaw_rad);
    OLED_ShowString(56, 16, roll_buf, OLED_8X16);
    
    OLED_Update();
    
    uart_send_data(&huart1);
    
    private_uart_data_read();
    private_uart_send_data(&huart1);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
#ifdef USE_FULL_ASSERT
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
