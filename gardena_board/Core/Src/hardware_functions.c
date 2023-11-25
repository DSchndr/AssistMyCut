/*
 * hardware_functions.c
 *
 *  Created on: Nov 24, 2023
 *      Author: Dennis
 */

#include "hardware_functions.h"
#include <math.h> //we need maths here
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++CHECK REFERENCE MANUAL++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define TMPSENSOR_AVGSLOPE 2.5 /* mV/°C */
#define TMPSENSOR_V25  0.76 /* V (at 25 °C)  */

#define TMPSENSOR_ADCMAX 4095.0 /* 12-bit ADC maximum value (12^2)-1)  */
#define TMPSENSOR_ADCREFVOL  3.3 /* Typical reference voltage, V  */
#define TMPSENSOR_ADCVREFINT  1.212 /* Internal reference voltage, V  */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++CHECK REFERENCE MANUAL++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define OVERSAMPLE 16

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim14;

volatile uint16_t adcResultsDMA[5];
const int adcChannelCount = sizeof(adcResultsDMA)/sizeof(adcResultsDMA[0]);
volatile int adcConversionComplete = 0;
volatile int adcConversionRunning = 0;

bool isMotorRunning;
int 	motor_speed;
bool motorDir;
void setLed(int led) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	if(led&1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	if(led&2) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	if(led&4) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
}
void setMotorDir(bool forward) {
	motorDir = forward;
	if(forward) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	}
}

void stopMotor() { //has to be turned off when the switch is off too (which io?), else that shitty chinesium mosfet driver dies (rip board #1, may your magic stm32 smoke rest well)
	TIM14->CCR1 = 0;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	isMotorRunning = false;
}

bool isChargerPresent() { //PA12
	return !HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_12);
}

//this is really bad, but right now i only have to care about one haha
void checkMotorOverCurrent() {
	if(adcConversionComplete) {
		adcConversionComplete = 0;
		//when its at full speed it measures like 3.8v
		//if( ((MotorCurrent > 3.2 && motorDir)||(MotorCurrent < 3.2 && !motorDir))  && motor_speed >= 20) stopMotor();
	} else if(!adcConversionRunning) startADCDMA();
}
bool isBatteryFull() {
	//check charger state, else voltage
	if(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_3)) return true;
	if(adcConversionComplete) {
		adcConversionComplete = 0;
		if(BatteryVoltage >= 4.1 ) return true;
	} else if(!adcConversionRunning) startADCDMA();
	return false;
}

void setMotorSpeed(int speed){
	TIM14->CCR1 = speed;
	motor_speed = speed; //read datasheet to know if ccr1 is readable, debugger can do that at least
	if(speed>0) isMotorRunning = true;
}

//remove me, test function
void motorSpinUpDown() {
	isMotorRunning = true;
	  for(int i = 20; i<=100; i++){
		  checkMotorOverCurrent();
		  setMotorSpeed(i);
		  HAL_Delay(200);
	  }
	  for(int i = 100; i>20; i--){
		  checkMotorOverCurrent();
		  setMotorSpeed(i);
		  HAL_Delay(200);
	  }
}
//rewrite me to nonblocking
void chargerCount() {
	for (int i = 1; i<=4; i*=2) {
		setLed(i);
		HAL_Delay(1000);
	}
}

//sgm41511, need implement sw i2c comms r2d2
void setCharger(bool enable){
	if(enable) {
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);

	}
	else {
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	}

}

void ledBlink(int amount, int time, int led) {
	for(int i = 0; i<amount; i++) {
		setLed(led);
		HAL_Delay(time);
		setLed(0);
		HAL_Delay(time);
	}
}

void startADCDMA() {
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcResultsDMA, adcChannelCount);
	adcConversionRunning = 1;
}

double adcValToVolts(uint16_t val) {
	return ((double)val / 4095 * 3.3);
}

void setMotorCurrent(uint16_t val) { //0v = 1A
	//I have no fucking idea how they measure that, it seems like this is the voltage after Q5 (mosfet between gnd and h bridge), so it derives the current from the backemf?
	// Ripple Counting it is! https://www.eletimes.com/brushed-dc-motor-driver-with-speed-control-and-stall-detection-reference-design :)
	// Imotor = (Varmature - Vbemf) / Rarmature
	// So Imotor = (Vbat - (val / 4095 * 4.2) ) / R???

	// maybe chengfang motor FK-280PAV-3744? so i would assume 2ohm as Rarmature
	//http://en.chengfang-motor.com/prod_view.aspx?TypeId=70&Id=188&FId=t3:70:3

	//The datasheet from that extremely shitty mosfet manuf site is
	// https://www.din-tek.jp/article/download?partNumber=DTQ6304
	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	/*++++++++++ TODO: ADD VOLTAGE DIVIDER FORMULA ++++++++++*/
	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	//mcu 1k goes to q5 drain, 1k to gnd
	// turn off q5 and measure backemf voltage over adc? has to be fast enough
	MotorCurrent = ((double)val/OVERSAMPLE)/4095*4.2*2; //current-ly voltage
}

//12.5 cycles seemed fine
#define nominal_resistance 10000       //Nominal resistance at 25⁰C
#define nominal_temeprature 25   // temperature for nominal resistance (almost always 25⁰ C)
#define beta 3950  // The beta coefficient or the B value of the thermistor (usually 3000-4000) check the datasheet for the accurate value.
#define Rref 10000   //Value of  resistor used for the voltage divider
void setNTCTemperature(uint16_t val) {
	//Ntc to gnd res: 10k res to 3.3v res:10k res to mcu: 1k
	//almost correct, reports 26c instead of like 23c but thats fine ig
	double temperature;
	double average;
	average = 10000 / (4095/((double)val/OVERSAMPLE) - 1); //todo: implement averaging | set oversample rate
	temperature = average / nominal_resistance;           // (R/Ro)
	temperature = log(temperature);                       // ln(R/Ro)
	temperature /= beta;                                  // 1/B * ln(R/Ro)
	temperature += 1.0 / (nominal_temeprature + 273.15);  // + (1/To)
	temperature = 1.0 / temperature;                      // Invert
	temperature -= 273.15;                                // convert absolute temp to C

	NTCTemperature = temperature;
}
void setBatteryVoltage(uint16_t val) { //3.3v when full, ? when empty
	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	/*++++++++++ TODO: ADD VOLTAGE DIVIDER FORMULA ++++++++++*/
	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	//to mcu 1k div gnd: 10k div bat: 10k
	double tmp = 0;
	//dont fucking ask me, for some reason having the motor or charger on causes it to report EXACTLY 1V more
	// might be something that the ground goes through, at least on board #1 something burned and the stm32 had no gnd connection anymore to the rest (except if the charger was connected)
	// could be the transistor logic under the battery
	if (isMotorRunning || isChargerPresent()) tmp = 1;
	BatteryVoltage = ((double)(val/OVERSAMPLE) / 4095 * 4.2)*2.0 - tmp; //*2.02 on bat perfect val
}
void setInternalTemperature(uint16_t temperature, uint16_t vref) {
	//https://github.com/Egoruch/Internal-Temperature-Sensor-STM32-HAL/blob/main/Core/Src/tmpsensor.c
	//returns like 11c which is plain wrong rn
	double intref_vol = (TMPSENSOR_ADCMAX*TMPSENSOR_ADCVREFINT)/((double)vref/OVERSAMPLE);
	double sensor_vol = ((double)temperature/OVERSAMPLE) * intref_vol/TMPSENSOR_ADCMAX;
	InternalTemperature = (sensor_vol - TMPSENSOR_V25) *1000.0/TMPSENSOR_AVGSLOPE + 30.0;
	//InternalTemperature = (((temperature * (TMPSENSOR_ADCMAX*TMPSENSOR_ADCVREFINT)/vref)/TMPSENSOR_ADCMAX) - TMPSENSOR_V25) *1000.0/TMPSENSOR_AVGSLOPE + 25.0;
}

//do not use this since its... blocking
void readADCs_blocking() {
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcResultsDMA, adcChannelCount);
	while(adcConversionComplete == 0) {

	}
	adcConversionComplete = 0;
}

//callback after adc has DMAed its data
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
	adcConversionComplete = 1;
	adcConversionRunning = 0;
	setBatteryVoltage(adcResultsDMA[0]);
	setMotorCurrent(adcResultsDMA[1]);
	setNTCTemperature(adcResultsDMA[2]);
	setInternalTemperature(adcResultsDMA[3],adcResultsDMA[4]);
}
