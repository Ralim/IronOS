#pragma once
/**
 * Configuration.h
 * Define default settings for TS80 and TS100
 * 
 */

//===========================================================================
//============================= Default Settings ============================
//===========================================================================
/**
 * Default soldering temp is 320.0 C
 * Temperature the iron sleeps at - default 150.0 C
 */
#define SOLDERING_TEMP           300
#define SLEEP_TEMP               150
#define BOOST_TEMP               420
#define BOOST_MODE_ENABLED       0          // 0: Disable 1: Enable

/**
 * Blink the temperature on the cooling screen when its > 50C 
 */
#define COOLING_TEMP_BLINK       1          // 0: Disable 1: Enable

/**
 * How many seconds/minutes we wait until going to sleep/shutdown.
 * Values -> SLEEP_TIME * 10; i.e. 5*10 = 50 Seconds! 
 */
#define SLEEP_TIME                5         // x10 Seconds
#define SHUTDOWN_TIME            10         // Minutes

/**
 * Auto start off for safety.
 * Pissible values are:  
 *  0 - none 
 *  1 - Soldering Temperature
 *  2 - Sleep Temperature 
 *  3 - Sleep Off Temperature 
 */
#define AUTO_START_MODE           0         // Default none

/**
 * OLED Orientation
 * 0: Right 1:Left 2:Automatic
 */
#define ORIENTATION_MODE            1         // Default Right
#define IN_LEFT_HANDED_MODE         0         // Default false -  0:FALSE 1:TRUE

#define REVERSE_BUTTON_TEMP_CHANGE  1         // Change/reverse the plus and minus button assigment
#define TEMP_CHANGE_SHORT_STEP      1         // Temp change short step +1
#define TEMP_CHANGE_LONG_STEP       10        // Temp change long step +10
#define TEMP_CHANGE_SHORT_STEP_MAX  50        // Temp change short step +1
#define TEMP_CHANGE_LONG_STEP_MAX   100       // Temp change long step +10


/**
 * OLED Orientation Sensitivity on Automatic mode!
 * Motion Sensitivity <0=Off 1=Least Sensitive 9=Most Sensitive>
 */
#define SENSITIVITY               7         // Default 7

/**
 * Detailed soldering screen
 * Detailed idle screen (off for first time users)
 */
#define DETAILED_SOLDERING        0         // 0: Disable 1: Enable - Default 0
#define DETAILED_IDLE             0         // 0: Disable 1: Enable - Default 0


#define CUT_OUT_SETTING           0         // default to no cut-off voltage (or 18W for TS80)
#define TEMPERATURE_INF           0         // default to 0
#define DESCRIPTION_SCROLL_SPEED  0         // 0: Slow 1: Fast - default to slow
#define POWER_LIMIT_ENABLE        0         // Default to no power limit

#ifdef MODEL_TS100
  #define VOLTAGE_DIV             467       // 467 - Default divider from schematic
  #define CALIBRATION_OFFSET      900       // 900 - Default adc offset in uV
  #define PID_POWER_LIMIT         70        // Sets the max pwm power limit
  #define POWER_LIMIT             65        // 30 watts default power limit
  #define MAX_POWER_LIMIT         65        //
  
  /**
   *  TIP_GAIN =  TIP_GAIN/1000 == uV per deg C constant of the tip
   */
  #define OP_AMP_Rf               750*1000  // 750  Kilo-ohms -> From schematic, R1
  #define OP_AMP_Rin              2370      // 2.37 Kilo-ohms -> From schematic, R2
  #define TIP_GAIN                405

#endif

#ifdef MODEL_TS80
  #define VOLTAGE_DIV             780       // Default divider from schematic
  #define PID_POWER_LIMIT         24        // Sets the max pwm power limit
  #define CALIBRATION_OFFSET      900       // the adc offset in uV
  #define POWER_LIMIT             24        // 24 watts default power limit
  #define MAX_POWER_LIMIT         30        // 

  /**
   *  TIP_GAIN =  TIP_GAIN/1000 == uV per deg C constant of the tip
   */
  #define OP_AMP_Rf               180*1000  //  180  Kilo-ohms -> From schematic, R6
  #define OP_AMP_Rin              2000      //  2.0  Kilo-ohms -> From schematic, R3
  #define TIP_GAIN                115
#endif