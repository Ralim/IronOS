/*
 * expMovingAverage.h
 *
 *  Created on: 8 Oct 2019
 *      Author: ralim
 */

#ifndef INC_EXPMOVINGAVERAGE_H_
#define INC_EXPMOVINGAVERAGE_H_

// max size = 127
template<class T, uint8_t weighting>
struct expMovingAverage {
	int32_t sum;
	void update(T const val) {
		sum = ((val * weighting) + (sum * (256 - weighting))) / 256;
	}

	T average() const {
		return sum;
	}
};

#endif /* INC_EXPMOVINGAVERAGE_H_ */
