/*
 * ThermoModel.cpp
 *
 *  Created on: 1 May 2021
 *      Author: Ralim
 */
#include "TipThermoModel.h"
#include "configuration.h"
#include "Utils.h"

#ifdef TEMP_uV_LOOKUP_MHP30
const uint16_t uVtoDegC[] = {
    //
    //
    0,     0,   //
    397,   10,  //
    798,   20,  ///
    1203,  30,  //
    1612,  40,  //
    2023,  50,  //
    2436,  60,  //
    3225,  79,  //
    4013,  98,  //
    4756,  116, //
    5491,  134, //
    5694,  139, //
    6339,  155, //
    7021,  172, //
    7859,  193, //
    8619,  212, //
    9383,  231, //
    10153, 250, //
    10930, 269, //
    11712, 288, //
    12499, 307, //
    13290, 326, //
    14084, 345, //
    14881, 364, //
    15680, 383, //
    16482, 402, //
    17285, 421, //
    18091, 440, //
    18898, 459, //

};
#endif

const int uVtoDegCItems = sizeof(uVtoDegC) / (2 * sizeof(uint16_t));


uint32_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return Utils::InterpolateLookupTable(uVtoDegC, uVtoDegCItems, tipuVDelta); }
