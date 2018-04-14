/*
 * FRToSI2C.hpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */

#ifndef FRTOSI2C_HPP_
#define FRTOSI2C_HPP_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

class FRToSI2C {
public:
	FRToSI2C(I2C_HandleTypeDef* i2chandle);
	void MasterTxCpltCallback(); //Normal Tx Callback
	void MemRxCpltCallback(); //Callback from memory read cycles
	void MemTxCpltCallback(); //Callback from memory write cycles

	void Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize,
				uint8_t *pData, uint16_t Size);
	void Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize,
				uint8_t *pData, uint16_t Size);

	void Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size);


private:
	I2C_HandleTypeDef* i2c;
};

#endif /* FRTOSI2C_HPP_ */
