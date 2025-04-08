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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t msg[] = "ENTREZ une commande (LED<n> ON/OFF, CHENILLARD<n> ON/OFF/INT ON/OFF) : \r\n";
uint8_t msg2[] = "Erreur ! Commande inconnue.\r\n";
uint8_t msg_chenillard_int_on[] = "Chenillard en mode interruption. Envoyer 'n' pour le prochain pas.\r\n";
uint8_t msg_chenillard_int_off[] = "Chenillard mode interruption OFF.\r\n";
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
const char* LED_ON_1 = "LED1 ON";
const char* LED_OFF_1 = "LED1 OFF";
const char* LED_ON_2 = "LED2 ON";
const char* LED_OFF_2 = "LED2 OFF";
const char* LED_ON_3 = "LED3 ON";
const char* LED_OFF_3 = "LED3 OFF";
const char* CHENILLARD_ON_1 = "CHENILLARD1 ON";
const char* CHENILLARD_OFF_1 = "CHENILLARD1 OFF";
const char* CHENILLARD_INT_ON_1 = "CHENILLARD1 INT ON";
const char* CHENILLARD_INT_OFF_1 = "CHENILLARD1 INT OFF";
const char* CHENILLARD_ON_2 = "CHENILLARD2 ON";
const char* CHENILLARD_OFF_2 = "CHENILLARD2 OFF";
const char* CHENILLARD_INT_ON_2 = "CHENILLARD2 INT ON";
const char* CHENILLARD_INT_OFF_2 = "CHENILLARD2 INT OFF";
const char* CHENILLARD_ON_3 = "CHENILLARD3 ON";
const char* CHENILLARD_OFF_3 = "CHENILLARD3 OFF";
const char* CHENILLARD_INT_ON_3 = "CHENILLARD3 INT ON";
const char* CHENILLARD_INT_OFF_3 = "CHENILLARD3 INT OFF";
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
#define MAX_BUFFER_SIZE 20
struct linear_buf_t {
  uint8_t current_index;
  uint8_t buffer[MAX_BUFFER_SIZE];
};
struct linear_buf_t linear_buf;
volatile uint8_t chenillard_running = 0;
volatile uint8_t current_chenillard = 0;
volatile uint8_t chenillard_int_mode = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
void linear_buf_reset(struct linear_buf_t *lb);
int linear_buf_insert_char(struct linear_buf_t *lb, uint8_t c);
void process_command(char *command);
void stop_chenillard(void);
void chenillard1_next_step(void);
void chenillard2_next_step(void);
void chenillard3_next_step(void);
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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

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
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  linear_buf_reset(&linear_buf);
  HAL_UART_Transmit(&huart3, msg, strlen((char*)msg), HAL_MAX_DELAY);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t rx_char;
  while (1)
  {
    if (HAL_UART_GetState(&huart3) == HAL_UART_STATE_READY) {
      if (HAL_UART_Receive(&huart3, &rx_char, 1, 0) == HAL_OK) {
        if (chenillard_running && chenillard_int_mode && (rx_char == 'n' || rx_char == 'N')) {
          // Exécuter un pas du chenillard en mode interruption manuelle
          if (current_chenillard == 1) {
            chenillard1_next_step();
          } else if (current_chenillard == 2) {
            chenillard2_next_step();
          } else if (current_chenillard == 3) {
            chenillard3_next_step();
          }
          continue; // Retour au début de la boucle pour attendre le prochain caractère
        } else if (rx_char == '\r' || rx_char == '\n') {
          process_command((char*)linear_buf.buffer);
          linear_buf_reset(&linear_buf);
          HAL_UART_Transmit(&huart3, msg, strlen((char*)msg), HAL_MAX_DELAY); // Réafficher l'invite
        } else {
          if (linear_buf_insert_char(&linear_buf, rx_char) != 0) {
            HAL_UART_Transmit(&huart3, (uint8_t*)"Buffer overflow!\r\n", strlen("Buffer overflow!\r\n"), HAL_MAX_DELAY);
            linear_buf_reset(&linear_buf);
            HAL_UART_Transmit(&huart3, msg, strlen((char*)msg), HAL_MAX_DELAY); // Réafficher l'invite
          }
        }
      }
    }

    // Exécution automatique des chenillards (si pas en mode interruption)
    if (chenillard_running && !chenillard_int_mode) {
      if (current_chenillard == 1) {
        static uint32_t last_tick1 = 0;
        if (HAL_GetTick() - last_tick1 >= 200) {
          chenillard1_next_step();
          last_tick1 = HAL_GetTick();
        }
      } else if (current_chenillard == 2) {
        static uint32_t last_tick2 = 0;
        if (HAL_GetTick() - last_tick2 >= 300) {
          chenillard2_next_step();
          last_tick2 = HAL_GetTick();
        }
      } else if (current_chenillard == 3) {
        static uint32_t last_tick3 = 0;
        if (HAL_GetTick() - last_tick3 >= 500) {
          chenillard3_next_step();
          last_tick3 = HAL_GetTick();
        }
      }
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief  System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                              RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void linear_buf_reset(struct linear_buf_t *lb) {
  lb->current_index = 0;
  memset(lb->buffer, 0, sizeof(lb->buffer));
}

int linear_buf_insert_char(struct linear_buf_t *lb, uint8_t c) {
  if (lb->current_index >= MAX_BUFFER_SIZE - 1) {
    return -1;
  }
  lb->buffer[lb->current_index++] = c;
  lb->buffer[lb->current_index] = '\0';
  return 0;
}

void stop_chenillard(void) {
  chenillard_running = 0;
  current_chenillard = 0;
  chenillard_int_mode = 0;
  HAL_GPIO_WritePin(GPIOB, led1_Pin | led2_Pin | led3_Pin, GPIO_PIN_RESET);
}

void chenillard1_next_step(void) {
  static uint8_t step = 0;
  switch (step) {
    case 0:
      HAL_GPIO_WritePin(GPIOB, led1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB, led2_Pin | led3_Pin, GPIO_PIN_RESET);
      break;
    case 1:
      HAL_GPIO_WritePin(GPIOB, led2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB, led1_Pin | led3_Pin, GPIO_PIN_RESET);
      break;
    case 2:
      HAL_GPIO_WritePin(GPIOB, led3_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB, led1_Pin | led2_Pin, GPIO_PIN_RESET);
      break;
    case 3:
      HAL_GPIO_WritePin(GPIOB, led2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB, led1_Pin | led3_Pin, GPIO_PIN_RESET);
      break;
    default:
      step = 0;
      break;
  }
  step = (step + 1) % 4;
}

void chenillard2_next_step(void) {
  static uint8_t state = 0;
  HAL_GPIO_WritePin(GPIOB, led1_Pin | led3_Pin, (state == 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, led2_Pin, (state == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  state = 1 - state;
}

void chenillard3_next_step(void) {
  HAL_GPIO_TogglePin(GPIOB, led1_Pin);
  HAL_GPIO_TogglePin(GPIOB, led2_Pin);
  HAL_GPIO_TogglePin(GPIOB, led3_Pin);
}

void process_command(char *command) {
  if (strcmp(command, LED_ON_1) == 0) {
    stop_chenillard();
    HAL_GPIO_WritePin(GPIOB, led1_Pin, GPIO_PIN_SET);
  } else if (strcmp(command, LED_OFF_1) == 0) {
    stop_chenillard();
    HAL_GPIO_WritePin(GPIOB, led1_Pin, GPIO_PIN_RESET);
  } else if (strcmp(command, LED_ON_2) == 0) {
    stop_chenillard();
    HAL_GPIO_WritePin(GPIOB, led2_Pin, GPIO_PIN_SET);
  } else if (strcmp(command, LED_OFF_2) == 0) {
    stop_chenillard();
    HAL_GPIO_WritePin(GPIOB, led2_Pin, GPIO_PIN_RESET);
  } else if (strcmp(command, LED_ON_3) == 0) {
    stop_chenillard();
    HAL_GPIO_WritePin(GPIOB, led3_Pin, GPIO_PIN_SET);
  } else if (strcmp(command, LED_OFF_3) == 0) {
    stop_chenillard();
    HAL_GPIO_WritePin(GPIOB, led3_Pin, GPIO_PIN_RESET);
  } else if (strcmp(command, CHENILLARD_ON_1) == 0) {
    stop_chenillard();
    chenillard_running = 1;
    current_chenillard = 1;
    chenillard_int_mode = 0;
  } else if (strcmp(command, CHENILLARD_OFF_1) == 0) {
    stop_chenillard();
  } else if (strcmp(command, CHENILLARD_INT_ON_1) == 0) {
    stop_chenillard();
    chenillard_running = 1;
    current_chenillard = 1;
    chenillard_int_mode = 1;
    HAL_UART_Transmit(&huart3, msg_chenillard_int_on, strlen((char*)msg_chenillard_int_on), HAL_MAX_DELAY);
  } else if (strcmp(command, CHENILLARD_INT_OFF_1) == 0) {
    stop_chenillard();
    HAL_UART_Transmit(&huart3, msg_chenillard_int_off, strlen((char*)msg_chenillard_int_off), HAL_MAX_DELAY);
  } else if (strcmp(command, CHENILLARD_ON_2) == 0) {
    stop_chenillard();
    chenillard_running = 1;
    current_chenillard = 2;
    chenillard_int_mode = 0;
  } else if (strcmp(command, CHENILLARD_OFF_2) == 0) {
    stop_chenillard();
  } else if (strcmp(command, CHENILLARD_INT_ON_2) == 0) {
    stop_chenillard();
    chenillard_running = 1;
    current_chenillard = 2;
    chenillard_int_mode = 1;
    HAL_UART_Transmit(&huart3, msg_chenillard_int_on, strlen((char*)msg_chenillard_int_on), HAL_MAX_DELAY);
  } else if (strcmp(command, CHENILLARD_INT_OFF_2) == 0) {
    stop_chenillard();
    HAL_UART_Transmit(&huart3, msg_chenillard_int_off, strlen((char*)msg_chenillard_int_off), HAL_MAX_DELAY);
  } else if (strcmp(command, CHENILLARD_ON_3) == 0) {
    stop_chenillard();
    chenillard_running = 1;
    current_chenillard = 3;
    chenillard_int_mode = 0;
  } else if (strcmp(command, CHENILLARD_OFF_3) == 0) {
    stop_chenillard();
  } else if (strcmp(command, CHENILLARD_INT_ON_3) == 0) {
    stop_chenillard();
    chenillard_running = 1;
    current_chenillard = 3;
    chenillard_int_mode = 1;
    HAL_UART_Transmit(&huart3, msg_chenillard_int_on, strlen((char*)msg_chenillard_int_on), HAL_MAX_DELAY);
  } else if (strcmp(command, CHENILLARD_INT_OFF_3) == 0) {
    stop_chenillard();
    HAL_UART_Transmit(&huart3, msg_chenillard_int_off, strlen((char*)msg_chenillard_int_off), HAL_MAX_DELAY);
  } else {
    HAL_UART_Transmit(&huart3, msg2, strlen((char*)msg2), HAL_MAX_DELAY);
  }
}

volatile uint8_t chenillard1_step = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // This callback will not be used for the chenillard effect in this implementation.
}
/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
   */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

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
  * where the assert_param error has occurred.
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
