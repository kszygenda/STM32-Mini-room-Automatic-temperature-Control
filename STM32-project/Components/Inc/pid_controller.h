#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

#include "arm_math.h"

void ARM_PID_Init(float32_t KP,float32_t KI,float32_t KD);
uint32_t Calculate_PID_out(float setpoint, float measured);

#endif // PID_CONTROLLER_H_
