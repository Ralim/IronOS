/*
 * ThermoModel.cpp
 *
 *  Created on: 1 May 2021
 *      Author: Ralim
 */
#include "TipThermoModel.h"
#include "Utils.h"
#include "configuration.h"

#ifdef TEMP_uV_LOOKUP_S60
const int32_t uVtoDegC[] = {
    //
    //
    0,     0,   //
    133,   10,  //
    261,   20,  //
    385,   30,  //
    505,  40,  //
    622,  50,  //
    736,  60,  //
    848,  70,  //
    958,  80,  //
    1067,  90,  //
    1175,  100, //
    1283,  110, //
    1390,  120, //
    1497,  130, //
    1604,  140, //
    1713,  150, //
    1822,  160, //
    1932,  170, //
    2044,  180, //
    2157,  190, //
    2272,  200, //
    2388,  210, //
    2507,  220, //
    2627,  230, //
    2750,  240, //
    2875,  250, //
    3001,  260, //
    3130,  270, //
    3261,  280, //
    3394,  290, //
    3529,  300, //
    3666,  310, //
    3804,  320, //
    3944,  330, //
    4085,  340, //
    4228,  350, //
    4371,  360, //
    4515,  370, //
    4659,  380, //
    4803,  390, //
    4948,  400, //
    5091, 410, //
    5234, 420, //
    5374, 430, //
    5514, 440, //
    5652, 450, //
    5786, 460, //
    5917, 470, //
    6045, 480, //
    6168, 490, //
    6287, 500, //

};
#endif


const int uVtoDegCItems = sizeof(uVtoDegC) / (2 * sizeof(uVtoDegC[0]));

uint32_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return Utils::InterpolateLookupTable(uVtoDegC, uVtoDegCItems, tipuVDelta); }
