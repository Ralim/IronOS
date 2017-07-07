/*
 * PID.c
 *
 *  Created on: 20 Sep 2016
 *      Author: ralim
 */

#include "PID.h"
#define MAXPIDOUTPUT 50000
//This function computes the new value for the ON time of the system
//This is the return value from this function
int32_t computePID(uint16_t setpoint) {
	int32_t ITerm = 0;
	static int16_t lastReading = 0;
	uint16_t currentReading = readIronTemp(0, 1,setpoint); //get the current temp of the iron
	int16_t error = (int16_t) setpoint - (int16_t) currentReading; //calculate the error term
	ITerm += (pidSettings.ki * error);
	if (ITerm > MAXPIDOUTPUT/2)
		ITerm = MAXPIDOUTPUT/2;
	else if (ITerm < 0)
		ITerm = 0; //cap at 0 since we cant force the iron to cool itself :)

	int16_t DInput = (currentReading - lastReading); //compute the input to the D term
	int32_t output = (pidSettings.kp * error) + (ITerm)
			- (pidSettings.kd * DInput);
	if (output > MAXPIDOUTPUT)
		output = MAXPIDOUTPUT;
	else if (output < 0)
		output = 0;

	lastReading = currentReading; //storing values for next iteration of the loop
	return output;

}
/*Sets up the pid values*/
void setupPID(void) {
	pidSettings.kp = 15;
	pidSettings.ki = 2;
	pidSettings.kd = 3;

}
