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
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t msg[] = "ENTREZ une commande (LED<n> ON/OFF, CHENILLARD<n> ON/OFF/FREQUENCE<n>) : \r\n";
uint8_t msg2[] = "Erreur ! Commande inconnue.\r\n";
uint8_t msg_chenillard_on[] = "Chenillard demarre.\r\n";
uint8_t msg_chenillard_off[] = "Chenillard arrete.\r\n";
uint8_t msg_led_on[] = "LED allumee.\r\n";
uint8_t msg_led_off[] = "LED eteinte.\r\n";
uint8_t msg_chenillard_running_err[] = "Erreur ! Arretez le chenillard en cours avant de controler les LEDs.\r\n";
uint8_t msg_freq_set_base[] = "Frequence chenillard reglee sur la vitesse de base.\r\n";
uint8_t msg_freq_set_slower[] = "Frequence chenillard reglee sur une vitesse plus lente.\r\n";
uint8_t msg_freq_set_faster[] = "Frequence chenillard reglee sur une vitesse plus rapide.\r\n";
uint8_t msg_freq_invalid[] = "Erreur ! Frequence chenillard inconnue (1, 2 ou 3).\r\n";
uint8_t msg_freq_syntax_err[] = "Erreur ! Syntaxe de la commande de frequence : CHENILLARD FREQUENCE<n>.\r\n";

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
const char* CHENILLARD_ON_2 = "CHENILLARD2 ON";
const char* CHENILLARD_OFF_2 = "CHENILLARD2 OFF";
const char* CHENILLARD_ON_3 = "CHENILLARD3 ON";
const char* CHENILLARD_OFF_3 = "CHENILLARD3 OFF";
const char* CHENILLARD_FREQ = "CHENILLARD FREQUENCE";
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define MAX_BUFFER_SIZE 40
struct linear_buf_t {
  uint8_t current_index;
  uint8_t buffer[MAX_BUFFER_SIZE];
};
struct linear_buf_t linear_buf;
volatile uint8_t chenillard_running = 0;
volatile uint8_t current_chenillard = 0;
volatile uint32_t chenillard1_speed = 0; // Période pour le chenillard 1
volatile uint32_t chenillard2_speed = 0; // Période pour le chenillard 2
volatile uint32_t chenillard3_speed = 0; // Période pour le chenillard 3
volatile uint8_t tim1_active = 0;
volatile uint8_t tim2_active = 0;
volatile uint8_t tim3_active = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
void linear_buf_reset(struct linear_buf_t *lb);
int linear_buf_insert_char(struct linear_buf_t *lb, uint8_t c);
void process_command(char *command);
void stop_chenillard(void);
void chenillard1(TIM_HandleTypeDef *htim);
void chenillard2(TIM_HandleTypeDef *htim);
void chenillard3(TIM_HandleTypeDef *htim);
void start_chenillard(uint8_t chenillard_id);
void set_chenillard_speed(uint8_t chenillard_id, uint8_t frequency);
void start_timer(TIM_HandleTypeDef *htim, uint32_t period);
void stop_timer(TIM_HandleTypeDef *htim);
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
  MX_USB_DEVICE_Init();
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
        if (rx_char == '\r' || rx_char == '\n') {
          process_command((char*)linear_buf.buffer);
          linear_buf_reset(&linear_buf);
          HAL_UART_Transmit(&huart3, msg, strlen((char*)msg), HAL_MAX_DELAY);
        } else {
          if (linear_buf_insert_char(&linear_buf, rx_char) != 0) {
            HAL_UART_Transmit(&huart3, (uint8_t*)"Buffer overflow!\r\n", strlen("Buffer overflow!\r\n"), HAL_MAX_DELAY);
            linear_buf_reset(&linear_buf);
            HAL_UART_Transmit(&huart3, msg, strlen((char*)msg), HAL_MAX_DELAY);
          }
        }
      }
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
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
}

/* USER CODE BEGIN 4 */
volatile uint8_t chenillard1_step = 0;
volatile GPIO_PinState chenillard2_state = GPIO_PIN_SET;
volatile GPIO_PinState chenillard3_state = GPIO_PIN_SET;

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
  stop_timer(&htim1);
  stop_timer(&htim2);
  stop_timer(&htim3);
  tim1_active = 0;
  tim2_active = 0;
  tim3_active = 0;
  HAL_GPIO_WritePin(GPIOB, led1_Pin | led2_Pin | led3_Pin, GPIO_PIN_RESET);
  HAL_UART_Transmit(&huart3, msg_chenillard_off, strlen((char*)msg_chenillard_off), HAL_MAX_DELAY);
}

void start_chenillard(uint8_t chenillard_id) {
  stop_chenillard();
  current_chenillard = chenillard_id;
  chenillard_running = 1;
  HAL_UART_Transmit(&huart3, msg_chenillard_on, strlen((char*)msg_chenillard_on), HAL_MAX_DELAY);
}

void start_timer(TIM_HandleTypeDef *htim, uint32_t period) {
  __HAL_TIM_SET_AUTORELOAD(htim, period - 1);
  HAL_TIM_Base_Start_IT(htim);
}

void stop_timer(TIM_HandleTypeDef *htim) {
  HAL_TIM_Base_Stop_IT(htim);
}

void set_chenillard_speed(uint8_t chenillard_id, uint8_t frequency) {
  uint32_t period = 0;
  if (frequency == 1) {
    period = 500; // Vitesse de base (ajuster)
    HAL_UART_Transmit(&huart3, msg_freq_set_base, strlen((char*)msg_freq_set_base), HAL_MAX_DELAY);
  } else if (frequency == 2) {
    period = 1000; // Vitesse plus lente (ajuster)
    HAL_UART_Transmit(&huart3, msg_freq_set_slower, strlen((char*)msg_freq_set_slower), HAL_MAX_DELAY);
  } else if (frequency == 3) {
    period = 3000; // Vitesse plus rapide (ajuster)
    HAL_UART_Transmit(&huart3, msg_freq_set_faster, strlen((char*)msg_freq_set_faster), HAL_MAX_DELAY);
  } else {
    HAL_UART_Transmit(&huart3, msg_freq_invalid, strlen((char*)msg_freq_invalid), HAL_MAX_DELAY);
    return;
  }

  if (chenillard_id == 1) {
    chenillard1_speed = period;
    if (chenillard_running && current_chenillard == 1) {
      start_timer(&htim1, chenillard1_speed);
      tim1_active = 1;
    }
  } else if (chenillard_id == 2) {
    chenillard2_speed = period;
    if (chenillard_running && current_chenillard == 2) {
      start_timer(&htim2, chenillard2_speed);
      tim2_active = 1;
    }
  } else if (chenillard_id == 3) {
    chenillard3_speed = period;
    if (chenillard_running && current_chenillard == 3) {
      start_timer(&htim3, chenillard3_speed);
      tim3_active = 1;
    }
  }
}

void chenillard1(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM1) {
    switch (chenillard1_step) {
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
      case 4:
        HAL_GPIO_WritePin(GPIOB, led1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, led2_Pin | led3_Pin, GPIO_PIN_RESET);
        break;
    }
    chenillard1_step = ((chenillard1_step + 1) % 5);
  }
}

void chenillard2(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    HAL_GPIO_WritePin(GPIOB, led1_Pin | led3_Pin, chenillard2_state);
    HAL_GPIO_WritePin(GPIOB, led2_Pin, !chenillard2_state);
    chenillard2_state = !chenillard2_state;
  }
}

void chenillard3(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM3) {
    HAL_GPIO_WritePin(GPIOB, led1_Pin | led2_Pin | led3_Pin, chenillard3_state);
    chenillard3_state = !chenillard3_state;
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (chenillard_running) {
    if (htim->Instance == TIM1 && current_chenillard == 1 && tim1_active) {
      chenillard1(htim);
    } else if (htim->Instance == TIM2 && current_chenillard == 2 && tim2_active) {
      chenillard2(htim);
    } else if (htim->Instance == TIM3 && current_chenillard == 3 && tim3_active) {
      chenillard3(htim);
    }
  }
}

void process_command(char *command) {
  // 1. Handle LED ON/OFF commands (assumed to be working)
  if (strncmp(command, LED_ON_1, strlen(LED_ON_1)) == 0) {
    HAL_GPIO_WritePin(GPIOB, led1_Pin, GPIO_PIN_SET);
    HAL_UART_Transmit(&huart3, msg_led_on, strlen((char*)msg_led_on), HAL_MAX_DELAY);
    return;
  } else if (strncmp(command, LED_OFF_1, strlen(LED_OFF_1)) == 0) {
    HAL_GPIO_WritePin(GPIOB, led1_Pin, GPIO_PIN_RESET);
    HAL_UART_Transmit(&huart3, msg_led_off, strlen((char*)msg_led_off), HAL_MAX_DELAY);
    return;
  } else if (strncmp(command, LED_ON_2, strlen(LED_ON_2)) == 0) {
    HAL_GPIO_WritePin(GPIOB, led2_Pin, GPIO_PIN_SET);
    HAL_UART_Transmit(&huart3, msg_led_on, strlen((char*)msg_led_on), HAL_MAX_DELAY);
    return;
  } else if (strncmp(command, LED_OFF_2, strlen(LED_OFF_2)) == 0) {
    HAL_GPIO_WritePin(GPIOB, led2_Pin, GPIO_PIN_RESET);
    HAL_UART_Transmit(&huart3, msg_led_off, strlen((char*)msg_led_off), HAL_MAX_DELAY);
    return;
  } else if (strncmp(command, LED_ON_3, strlen(LED_ON_3)) == 0) {
    HAL_GPIO_WritePin(GPIOB, led3_Pin, GPIO_PIN_SET);
    HAL_UART_Transmit(&huart3, msg_led_on, strlen((char*)msg_led_on), HAL_MAX_DELAY);
    return;
  } else if (strncmp(command, LED_OFF_3, strlen(LED_OFF_3)) == 0) {
    HAL_GPIO_WritePin(GPIOB, led3_Pin, GPIO_PIN_RESET);
    HAL_UART_Transmit(&huart3, msg_led_off, strlen((char*)msg_led_off), HAL_MAX_DELAY);
    return;
  }

  // 2. Handle CHENILLARD ON/OFF commands
  else if (strncmp(command, CHENILLARD_ON_1, strlen(CHENILLARD_ON_1)) == 0) {
    start_chenillard(1);
    set_chenillard_speed(1, 1); // Démarrer à la vitesse de base
    return;
  } else if (strncmp(command, CHENILLARD_OFF_1, strlen(CHENILLARD_OFF_1)) == 0) {
    stop_chenillard();
    return;
  } else if (strncmp(command, CHENILLARD_ON_2, strlen(CHENILLARD_ON_2)) == 0) {
    start_chenillard(2);
    set_chenillard_speed(2, 1); // Démarrer à la vitesse de base
    return;
  } else if (strncmp(command, CHENILLARD_OFF_2, strlen(CHENILLARD_OFF_2)) == 0) {
    stop_chenillard();
    return;
  } else if (strncmp(command, CHENILLARD_ON_3, strlen(CHENILLARD_ON_3)) == 0) {
    start_chenillard(3);
    set_chenillard_speed(3, 1); // Démarrer à la vitesse de base
    return;
  } else if (strncmp(command, CHENILLARD_OFF_3, strlen(CHENILLARD_OFF_3)) == 0) {
    stop_chenillard();
    return;
  }

  else if (strncmp(command, CHENILLARD_FREQ, strlen(CHENILLARD_FREQ)) == 0) {
    if (strlen(command) > strlen(CHENILLARD_FREQ)) {
      uint8_t freq_char = command[strlen(CHENILLARD_FREQ)];
      uint8_t frequency = freq_char - '0'; // Convert char to integer

      if (frequency >= 1 && frequency <= 3) {
        if (chenillard_running) {
          set_chenillard_speed(current_chenillard, frequency);
        } else {
          HAL_UART_Transmit(&huart3, msg_chenillard_running_err, strlen((char*)msg_chenillard_running_err), HAL_MAX_DELAY);
        }
      } else {
        HAL_UART_Transmit(&huart3, msg_freq_invalid, strlen((char*)msg_freq_invalid), HAL_MAX_DELAY);
      }
    } else {
      HAL_UART_Transmit(&huart3, msg_freq_syntax_err, strlen((char*)msg_freq_syntax_err), HAL_MAX_DELAY);
    }
    return;
  }

  // 4. Handle invalid commands
  else {
    HAL_UART_Transmit(&huart3, msg2, strlen((char*)msg2), HAL_MAX_DELAY);
    return;
  }
}

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void) {
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
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
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
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
