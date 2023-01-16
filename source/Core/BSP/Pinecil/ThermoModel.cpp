/*
 * ThermoModel.cpp
 *
 *  Created on: 1 May 2021
 *      Author: Ralim
 */
#include "TipThermoModel.h"
#include "Utils.h"
#include "configuration.h"

#ifdef TEMP_uV_LOOKUP_HAKKO
const int32_t uVtoDegC[] = {
    //
    // uv -> temp in C
    0,     0,   //
    266,   10,  //
    522,   20,  //
    770,   30,  //
    1010,  40,  //
    1244,  50,  //
    1473,  60,  //
    1697,  70,  //
    1917,  80,  //
    2135,  90,  //
    2351,  100, //
    2566,  110, //
    2780,  120, //
    2994,  130, //
    3209,  140, //
    3426,  150, //
    3644,  160, //
    3865,  170, //
    4088,  180, //
    4314,  190, //
    4544,  200, //
    4777,  210, //
    5014,  220, //
    5255,  230, //
    5500,  240, //
    5750,  250, //
    6003,  260, //
    6261,  270, //
    6523,  280, //
    6789,  290, //
    7059,  300, //
    7332,  310, //
    7609,  320, //
    7889,  330, //
    8171,  340, //
    8456,  350, //
    8742,  360, //
    9030,  370, //
    9319,  380, //
    9607,  390, //
    9896,  400, //
    10183, 410, //
    10468, 420, //
    10750, 430, //
    11029, 440, //
    11304, 450, //
    11573, 460, //
    11835, 470, //
    12091, 480, //
    12337, 490, //
    12575, 500, //

};
#endif

const int uVtoDegCItems = sizeof(uVtoDegC) / (2 * sizeof(uVtoDegC[0]));

uint32_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return Utils::InterpolateLookupTable(uVtoDegC, uVtoDegCItems, tipuVDelta); }
