/*
 * PID.c
 *
 *  Created on: 20 Sep 2016
 *      Author: ralim
 */

#include "PID.h"
#define MAXPIDOUTPUT 50000
//MAXPIDOUTPUT is the maximum time duration we can support for the irons output, as anything longer than this will be cut off by the next PID iteration

pidSettingsType pidSettings;

//This function computes the new value for the ON time of the system
//This is the return value from this function
int32_t computePID(uint16_t setpoint) {
	static int32_t ITerm = 0; //Used to store the integral error
	static int16_t lastError = 0;
	uint16_t currentReading = readIronTemp(0, 1, setpoint); //get the current temp of the iron
	int16_t error = (int16_t) setpoint - (int16_t) currentReading; //calculate the error term
	int16_t DInput = (error - lastError) / 10; //compute the input to the D term

	ITerm += ((error) / 20);
	if (ITerm > 300)
		ITerm = 0; //Prevents this accumulating too much during inital heatup
	else if (ITerm < 0)
		ITerm = 0; //Cap at 0 as we can't force cool
	if (DInput > 1000)
		DInput = 0;
	int32_t output = (pidSettings.kp * error) + (ITerm * pidSettings.ki)
			- (pidSettings.kd * DInput);
	if (output > MAXPIDOUTPUT)
		output = MAXPIDOUTPUT;
	else if (output < 0)
		output = 0;
	lastError = error;
	return output;

}
/*Sets up the pid values to  defaults*/
void setupPID(void) {
	pidSettings.kp = 8;
	pidSettings.ki = 1;
	pidSettings.kd = 0; //Not using D atm

}
