// * dipMonitor.c from DSP/BIOS WORKSHOP.	MAR 2008 T.Cartier

// - 'readDipSwitches reads DIP switches 0 and 1 on the EVM6437 
// - 'initDipSwitches performs the initial setup of the switch states
// - the use of the 'hw' vars test if the physical switch was thrown
//   if so, sw0 or sw1 is updated. This allows sw0/1 to be changed in the watch
//   window and not be immediatly overwritten by the EVM DIP switch setting
// - I2C access are very slow, so a minimum number of them are employed

// ========= INCLUDES =========================================================
#include "myWorkcfg.h"		// access to config file headers
#include <evmdm6437_i2c.h>			// access to I2C API for LED/DIP usage
#include <evmdm6437_i2c_gpio.h>		// access to LED/DIP addresses on I2C
#include <CLK.h>					// access to BIOS CLocK API

// ========= EXTERNAL SYMBOLS =================================================
extern short sw0;					// access to global in audio.c
extern short sw1;					// access to global in audio.c

// ========= GLOBAL VARIABLES =================================================
short hw_sw0;						// state of SW0 on EVM
short hw_sw1;						// state of SW1 on EVMshort sw2;							// sw2 will be used in lab-7 as load value 
short hw_sw2;						// state of SW2 on EVM
short hw_sw3;						// state of SW3 on EVM
short sw2;							// sw2 will be used in lab-7 as load value
short sw3;							// sw3 will be used in lab-7 as load value

// ========= initSwitch() - call from main to init the values of pFe, pCs =====
void initDipSwitches(void){			// get initial state of all 4 DIP switches
	unsigned char dips;				// holds DIP switch state from I2C
	
	EVMDM6437_I2C_GPIO_init();						// master init of I2C 
	EVMDM6437_I2C_read(I2C_GPIO_GROUP_0, &dips, 1);	// read all 4 DIP switches
	sw0 = hw_sw0 = !(dips>>4&1); 					// set state of sw0
	sw1 = hw_sw1 = !(dips>>5&1); 					// set state of sw1
	sw2 = hw_sw2 = !(dips>>6&1);					// set state of sw2
	sw3 = hw_sw3 = !(dips>>7&1);					// set state of sw3
}	 

// ========= readSwitch() - Read DIP Switches during BIOS IDL =================
void readDipSwitches(void){			// test sw0,1 every 1/2 sec. update if new
	unsigned char dips;				// holds DIP switch state from I2C	
	static int t0=0, t1=0;			// last time DIPs updated, current time

    t1 = CLK_getltime();			// get current time
 	if ((t1 - t0) >= 500) {			// if 1/2 second or more has passed...
		t0 = t1;					// update prior time value

		EVMDM6437_I2C_read( I2C_GPIO_GROUP_0, &dips, 1 );		// get all DIPs
		if(hw_sw0==(dips>>4&1)){sw0 = hw_sw0 = !(dips>>4&1);}	// DIP0 test
		if(hw_sw1==(dips>>5&1)){sw1 = hw_sw1 = !(dips>>5&1);} 	// DIP1 test
	}   
}

