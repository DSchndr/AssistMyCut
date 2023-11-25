/*
 * assistcut.c
 *
 *  Created on: Nov 24, 2023
 *      Author: Dennis
 */
#include "assistcut.h"
#include "hardware_functions.h"
#include <stdbool.h>

void init_assistcut() {

}

void loop_assistcut() {
	/*if(NTCTemperature > 50 || NTCTemperature < 10) {
		while(true){
			stopMotor();
			setCharger(false);
			ledBlink(1, 100, 2);
		}
	}*/
	  if(isChargerPresent()) { //can probably be moved in hardware_functions
		  stopMotor();
		  if(isBatteryFull()) { //check if pa3 blinks with 3hz -> charge failure
			  setCharger(false);
			  while(true) {
				  ledBlink(1, 2000, 7); //unplug me, i am full ffs
			  }
			  return;
		  }
		  setCharger(true);
		  chargerCount();
		  return;
	  } else {
		  setCharger(false);
	  }
	  HAL_Delay(100);
	  setLed(1);
	  setMotorDir(true);
	  motorSpinUpDown();
	  HAL_Delay(100);
	  setLed(2);
	  setMotorDir(false);
	  motorSpinUpDown();
	  stopMotor();
	  ledBlink(4, 1000, 7);
}
