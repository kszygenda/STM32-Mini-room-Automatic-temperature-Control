#include "../Inc/pid_controller.h"

static arm_pid_instance_f32 PID;
static float error;
static float32_t pid_out;
static uint32_t pwm_duty;

static uint32_t last_pwm_duty;
_Bool rises;
/**
 * @brief This function Initializes arm_pid controller with given parameters.
 * @param KP Kp gain of PID controller.
 * @param KI Ki gain of PID controller.
 * @param KD Kd gain of PID controller.
 */
void ARM_PID_Init(float32_t KP,float32_t KI,float32_t KD) {
    PID.Kp = KP;
    PID.Ki = KI;
    PID.Kd = KD;
    arm_pid_init_f32(&PID, 1);
}

/**
 * @brief This function returns value of controller output.
 * @param setpoint Current destined value for closed loop system.
 * @param measured Current measured system output.
 * @return Controller output.
 */
uint32_t Calculate_PID_out(float setpoint, float measured) {
//    // Funkcja oblicza aktualny error, potem pid_out a na koniec konwersja na PWM
//    error = setpoint - measured;  // Calculate the error
////    pid_out = arm_pid_f32(&PID, error);  // Calculate the PID output
////    pwm_duty = (uint32_t)pid_out;  // Convert to PWM
//
//    if (error < 0.4f && rises == 1){
//    	pwm_duty = 0;
//    	rises = 0;
//    }
//    if (error > -0.4f && rises == 0){
//    	pwm_duty = 15;
//    	rises = 1;
//    }
//    if (measured < setpoint){
//
//    }

//    last_pwm_duty = 0;

    if(measured < setpoint + 0.05) {
    	last_pwm_duty = (int)setpoint*0.25;
    }
    else if(measured > setpoint + 0.08) {
        last_pwm_duty = (int)setpoint*0.08;
    }
    return last_pwm_duty;





//    if (pwm_duty > 45){
//    	// Limit the duty cycle to 100
//    	pwm_duty = 45;
//    }
//    if (pwm_duty < setpoint*0.2){
//    	pwm_duty = (int) setpoint*0.17;
//    }// Limit the duty cycle to 0
    return pwm_duty;
}
