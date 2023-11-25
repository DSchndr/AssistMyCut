/*
 * hardware_functions.h
 *
 *  Created on: Nov 24, 2023
 *      Author: Dennis
 */

#ifndef INC_HARDWARE_FUNCTIONS_H_
#define INC_HARDWARE_FUNCTIONS_H_
#include "stm32g0xx_hal.h"
#include <stdbool.h>

volatile int adcConversionComplete;

double MotorCurrent;
double BatteryVoltage;
double InternalTemperature;
double NTCTemperature;

void startADCDMA();
void setLed(int led);
void setMotorDir(bool forward);
void stopMotor();
bool isChargerPresent();
void setCharger(bool enable);
void ledBlink(int amount, int time, int led);
void checkMotorOverCurrent();
bool isBatteryFull();
void setMotorSpeed(int speed);


void chargerCount();
void motorSpinUpDown();


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);

#endif /* INC_HARDWARE_FUNCTIONS_H_ */
