/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../Components/Inc/bmp2_config.h"
#include "../../Components/Inc/bmp2.h"
#include "../../Components/Inc/bmp2_defs.h"
#include "../../Components/Inc/LCD.h"
#include "../../Components/Inc/pid_controller_config.h"
#include <stdlib.h>
#include <stdio.h>
#include "arm_math.h"
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

int temp_read_int;
int temp_fractional;
int temp_receivedValue_int;
int temp_receivedValue_fractional;
float temp_read, error, receivedValue, pid_out;
char json_msg[128];
float pwm_duty;
#define RX_BUFFER_SIZE 128
char rxBuffer[RX_BUFFER_SIZE];
char lastRxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxIndex = 0;
volatile uint8_t dataReceivedFlag = 0;
static uint32_t prev_encoder_val;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */



/**
 * @brief This function sets desired width modulation.
 * @param[in] htim, pointer to a timer instance, .
 * @param[in] pwm_power Desired width [%] in PWM Signal.
 */
void set_pwm_power (TIM_HandleTypeDef *htim, float pwm_power){
	if (pwm_power == 0){
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 0);
	}
	uint32_t Counter_period = htim->Init.Period;
	uint32_t pwm_val = (Counter_period*pwm_power)/100.0f;
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, (uint32_t)pwm_val);
	}
/**
 * @brief This function adjusts destined temperature via encoder.
 * @param[in] Destined temperature.
 * @return [-].
 */

float encoder_destined_set(float Destined_temperature) {
    uint32_t encoder_val = htim3.Instance->CNT;
    int32_t diff = encoder_val - prev_encoder_val;



    // Handle wrap-around cases
    if ((encoder_val == 0 && prev_encoder_val == 80) || diff > 0) {
        Destined_temperature -= 0.5f;
    } else if ((encoder_val == 80 && prev_encoder_val == 0) || diff < 0) {
        Destined_temperature += 0.5f;
    }

    if (Destined_temperature <= 25.0f){
    	Destined_temperature = 25.0f;
    }
    else if (Destined_temperature >= 60.0f){
    	Destined_temperature = 60.0f;
    }


    // Update previous encoder value for the next call
    prev_encoder_val = encoder_val;

    temp_receivedValue_int = (int)Destined_temperature;
    temp_receivedValue_fractional = (int)((Destined_temperature - temp_receivedValue_int) * 1000);

    LCD_goto_line(1);
    LCD_printf("Set:%d.%03d[C]       ",  temp_receivedValue_int, temp_receivedValue_fractional);


    return Destined_temperature;
}


// Inside the HAL_TIM_PeriodElapsedCallback function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2){
    // Read the temperature with a frequency of 4 Hz. 
		temp_read = BMP2_ReadTemperature_degC(&bmp2dev_1);
		receivedValue = encoder_destined_set(receivedValue);
		temp_read_int = (int)temp_read;
		temp_fractional = (int)((temp_read - temp_read_int) * 1000);
		// Write data to LCD
		LCD_goto_line(0);
		LCD_printf("Actual:%d.%03d[C]", temp_read_int, temp_fractional);
		// Jakiś smiszny debugging dla odczytywania wartości zadaniej
		// opisać komentarze #TODO @Bartek
		if (dataReceivedFlag == 1){
			dataReceivedFlag = 0;  // Resetuj flagę

			// Konwersja stringa na float
			receivedValue = atof(rxBuffer);
			temp_receivedValue_int = (int)receivedValue;
			temp_receivedValue_fractional = (int)((receivedValue - temp_receivedValue_int) * 1000);

			// Wyświetl odebraną wiadomość
			LCD_goto_line(1);
			LCD_printf("Set:%d.%03d[C]       ",  temp_receivedValue_int, temp_receivedValue_fractional);

			// Resetuj rxBuffer
			memset(rxBuffer, 0, RX_BUFFER_SIZE);

		}
		pwm_duty = PID_GetOutput(&hpid1, receivedValue, temp_read);
	    set_pwm_power(&htim2, pwm_duty);
	    int msg_len = sprintf(json_msg, "{\"temperature\": %.2f, \"error\": %.2f, \"pwm_power\": %.2f, \"destined\": %.2f}\r\n",
        temp_read,receivedValue-temp_read,pwm_duty,receivedValue);
			HAL_UART_Transmit(&huart3, (uint8_t*)json_msg, msg_len, 1000);

		HAL_GPIO_WritePin(GPIO_Fan_GPIO_Port, GPIO_Fan_Pin, temp_read > (receivedValue + 4));

	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART3)  // Sprawdź, czy przerwanie pochodzi z USART3
    {
        if (rxBuffer[rxIndex] == '\n')  // Sprawdź, czy odebrano znak końca linii
        {
            dataReceivedFlag = 1;  // Ustaw flagę o odebraniu pełnej wiadomości
            rxBuffer[rxIndex] = '\0';  // Zamień znak końca linii na znak końca łańcucha
            rxIndex = 0;  // Resetuj indeks bufora
        }
        else
        {
            if (++rxIndex >= RX_BUFFER_SIZE)  // Inkrementuj indeks i sprawdź przepełnienie
            {
                rxIndex = 0;  // Resetuj indeks bufora
            }
        }
        HAL_UART_Receive_IT(&huart3, (uint8_t*)&rxBuffer[rxIndex], 1);  // Ponownie włącz przerwanie
    }
}


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
  MX_USART3_UART_Init();
  MX_SPI4_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  // Inicjalizacja komponentów zewnętrznych
  BMP2_Init(&bmp2dev_1);
  LCD_init();
  memset(lastRxBuffer, 0, RX_BUFFER_SIZE);  // Inicjalizacja lastRxBuffer
  HAL_UART_Receive_IT(&huart3, (uint8_t*)&rxBuffer[rxIndex], 1);  // Inicjalizacja przerwania odbioru UART
  //Zmiana priorytetu przerwań, #TODO debugging.
  HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
  HAL_NVIC_SetPriority(TIM2_IRQn, 6, 0); // Przykładowy niższy priorytet
  //PID regulator tune
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  ARM_PID_Init(2.5f,0.0f,53.639f);
  PID_Init(&hpid1);
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
  while (1)
  {
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
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
