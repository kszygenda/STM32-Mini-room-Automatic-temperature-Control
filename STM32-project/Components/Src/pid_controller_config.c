/**
  ******************************************************************************
  * @file    pid_controller_config.c
  * @author  AW           Adrian.Wojcik@put.poznan.pl
  * @version 1.0
  * @date    06 Sep 2021 
  * @brief   Simple PID controller implementation.
  *          Configuration file.
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "../Inc/pid_controller_config.h"

/* Typedef -------------------------------------------------------------------*/

/* Define --------------------------------------------------------------------*/

/* Macro ---------------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
PID_HandleTypeDef hpid1 = {
  .Kp = 0.9f, .Ki = 45.0f, .Kd = 0.1f,
  .N = 3.7823f, .Ts = (1.0f / 9000.0f),
	.LimitUpper = 20.0f, .LimitLower = 0.0f
};

/* Private function prototypes -----------------------------------------------*/

/* Private function ----------------------------------------------------------*/

/* Public function -----------------------------------------------------------*/
