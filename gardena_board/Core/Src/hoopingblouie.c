/*
 * hoopingblouie.c
 *
 *  Created on: Nov 24, 2023
 *      Author: Dennis
 */

#include "hoopingblouie.h" // you know what i am up to with that board :)
#include "stm32g0xx_hal.h"
#include <stdbool.h>
#include "hardware_functions.h"
//#include "driver_i2c_tm1637.h" //todo

uint32_t seed = 0;

//generate Pseudo random number
uint32_t randomNumber() {
	//current tick + last adc conversion data + timer14 count, at least better than sonys implementation
	seed += HAL_GetTick() * *(uint16_t *)0x40012440 * *(uint16_t *)0x40002024;
	return seed;
}

void init_hoopingblouie() {

}

void loop_hoopingblouie() {
	setMotorSpeed(randomNumber()/(0xFFFFFFFF * 100)); //todo map to 0-100
}

