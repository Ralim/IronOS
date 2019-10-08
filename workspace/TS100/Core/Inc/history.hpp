/*
 * history.hpp
 *
 *  Created on: 28 Oct, 2018
 *     Authors: Ben V. Brown, David Hilton
 */

#ifndef HISTORY_HPP_
#define HISTORY_HPP_

#include <stdint.h>

// max size = 127
template<class T, uint8_t SIZE>
struct history {
	static const uint8_t size = SIZE;
	T buf[size];
	int32_t sum;
	uint8_t loc;

	void update(T const val) {
		// step backwards so i+1 is the previous value.

		sum -= buf[loc];
		sum += val;
		buf[loc] = val;
		loc = (loc + 1) % size;
	}

	T operator[](uint8_t i) const {
		// 0 = newest, size-1 = oldest.
		i = (i + loc) % size;
		return buf[i];
	}

	T average() const {
		return sum / size;
	}
};

#endif  /* HISTORY_HPP_ */
