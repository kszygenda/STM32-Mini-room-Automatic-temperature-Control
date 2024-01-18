#include "../Inc/pid_controller.h"

static arm_pid_instance_f32 PID;
static float error;
static float32_t pid_out;
static uint32_t pwm_duty;
void ARM_PID_Init(float32_t KP,float32_t KI,float32_t KD) {
    PID.Kp = KP;
    PID.Ki = KI;
    PID.Kd = KD;
    arm_pid_init_f32(&PID, 1);
}

uint32_t Calculate_PID_out(float setpoint, float measured) {
    // Funkcja oblicza aktualny error, potem pid_out a na koniec konwersja na PWM
    error = setpoint - measured;  // Calculate the error
    pid_out = arm_pid_f32(&PID, error);  // Calculate the PID output
    pwm_duty = (uint32_t)pid_out;  // Convert to PWM
    if (pwm_duty > 45){
    	// Limit the duty cycle to 100
    	pwm_duty = 45;
    }
    if (pwm_duty < 0){
    	pwm_duty = 0;
    }// Limit the duty cycle to 0
    return pwm_duty;
}
