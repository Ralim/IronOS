/*
 * Analog.c
 *
 *  Created on: 20 Sep 2016
 *      Author: ralim
 *      Contains the functions related to reading and scaling the adc pins
 *      This is used for temperature and battery voltage sense
 */
#include "Analog.h"

//Reads the dc input and returns it as X10 voltage (ie 236 = 23.6V)
//Seems unstable below 9.5V input
uint16_t readDCVoltage(uint16_t divFactor) {
	uint16_t reading = 0;
	for (u8 i = 0; i < 10; i++) {
		reading += ADC_GetConversionValue(ADC2);
	}
	reading /= divFactor; //take the average and convert to X10 voltage
	return reading; //return the read voltage
}

//This reads the thermocouple in the tip
//This allows us to read it in X10 mode
//Returns temperature in C X10 mode
int16_t readTipTemp() {
	static uint32_t rollingAverage[16];
	static uint8_t rIndex = 0;

	/*The head has a thermocouple inline with the heater
	 This is read by turning off the heater
	 Then read the output of the op-amp that is connected across the connections
	 */
	uint32_t ad_sum = 0;
	uint32_t max = 0, min;
	uint32_t ad_value, avg_data;
	uint32_t timer = getIronTimer();
	setIronTimer(0); //set the remaining time to zero
	HEAT_OFF(); //heater must be off
	delayMs(5); //wait for the heater to time out
	uint8_t gMeas_cnt = 9; //how many measurements to make
	max = ad_sum = min = Get_ADC1Value(0);

	while (gMeas_cnt > 0) {
		ad_value = Get_ADC1Value(0);
		ad_sum += ad_value;
		if (ad_value > max)
			max = ad_value;
		if (ad_value < min)
			min = ad_value;

		gMeas_cnt--;
	}
	setIronTimer(timer);
	ad_sum = ad_sum - max - min; //remove the two outliers
	avg_data = ad_sum / 8; //take the average
	rollingAverage[rIndex] = avg_data;
	rIndex = (rIndex + 1) % 16;
	return (rollingAverage[0] + rollingAverage[1] + rollingAverage[2]
			+ rollingAverage[3] + rollingAverage[4] + rollingAverage[5]
			+ rollingAverage[6] + rollingAverage[7] + rollingAverage[8]
			+ rollingAverage[9] + rollingAverage[10] + rollingAverage[11]
			+ rollingAverage[12] + rollingAverage[13] + rollingAverage[14]
			+ rollingAverage[15]) / 16; //get the average

}

/*******************************************************************************
 Function:
 Description:Reads the temperature of the on board temp sensor for calibration
 http://www.analog.com/media/en/technical-documentation/data-sheets/TMP35_36_37.pdf
 Output: The onboardTemp in C X 10
 *******************************************************************************/
int readSensorTemp(void) {
	static uint32_t rollingAverage[4];
	static uint8_t rIndex = 0;
	u32 ad_sum = 0;
	u32 max, min;
	u32 ad_value, avg_data, slide_data;

	u8 gMeas_cnt = 9;
	ad_sum = min = max = Get_ADC1Value(1);
	while (gMeas_cnt > 0) {
		ad_value = Get_ADC1Value(1);
		ad_sum += ad_value;
		if (ad_value > max)
			max = ad_value;
		if (ad_value < min)
			min = ad_value;
		gMeas_cnt--;
	}
	ad_sum = ad_sum - max - min;
	avg_data = ad_sum / 8;
	//^ Removes the two outliers from the data spread

	rollingAverage[rIndex] = avg_data; //store this result
	rIndex = (rIndex + 1) % 4; //move the index
	slide_data = (rollingAverage[0] + rollingAverage[1] + rollingAverage[2]
			+ rollingAverage[3]) / 4; //get the average
	return (250 + (3300 * slide_data / 4096) - 750);
	//(25 + ((10*(33*gSlide_data)/4096)-75));
	//^ Convert the reading to C

}

volatile uint16_t ADC1ConvertedValue[2];
//returns the latest reading from ADC1 that was buffered using DMA
uint16_t Get_ADC1Value(uint8_t i) {
	return ADC1ConvertedValue[i];
}
//This returns the calibrated temperature reading of the iron temp
//inputs : calibration value / wether to take a new reading or not
uint16_t readIronTemp(uint16_t calibration_temp, uint8_t read,
		uint16_t setPointTemp) {
	static uint16_t calTemp = 0;
	static uint16_t lastVal = 0;
	static uint16_t lastSetTemp;
	if (setPointTemp != 0xFFFF)
		lastSetTemp = setPointTemp;
	if (calibration_temp != 0)
		calTemp = calibration_temp;

	if (read) {
		int16_t compensation = 80 + 150 * (lastSetTemp - 1000) / 3000;
		int16_t tipTemp = readTipTemp();
		int16_t ColdJTemp = readSensorTemp();
		if (lastSetTemp == 1000)
			compensation -= 10;

		if (lastSetTemp != 0) {
			if (tipTemp > (compensation + calTemp))
				tipTemp -= compensation;
		}
		if (ColdJTemp > 400)
			ColdJTemp = 400;
		lastVal = (tipTemp * 1000 + 807 * ColdJTemp - calTemp * 1000) / 807;

	}

	return lastVal;

}
