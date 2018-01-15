/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  DIP Switches
 *
 */

#include "evmdm6437_dip.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _DIP_init( )                                                            *
 *                                                                          *
 *      Initialize the DIP switch ( connected to the I2C GPIOs )            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_DIP_init( )
{
    return EVMDM6437_I2C_GPIO_init( );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _DIP_get( dip_num )                                                     *
 *                                                                          *
 *      Get the DIP switch value                                            *
 *                                                                          *
 *      dip_num => [DIP_0:DIP_3] - DIP switches                             *
 *                 [JP1_JUMPER]  - JP1                                      *
 *                 [SW7_SWITCH]  - SW7                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_DIP_get( Int32 dip_num )
{
    Uint16 dip_state;

    switch( dip_num )
    {
        case DIP_0:
        case DIP_1:
        case DIP_2:
        case DIP_3:
        case JP1_JUMPER:
        case SW7_SWITCH:
            /* Read DIP switch */
            if ( EVMDM6437_I2C_GPIO_getall( I2C_GPIO_GROUP_0, &dip_state ) )
                return -1;
            break;

        default:
            return -1;
    }

    /* Return 0 or 1 */
    return ( dip_state >> ( 4 + dip_num ) ) & 1;
}
