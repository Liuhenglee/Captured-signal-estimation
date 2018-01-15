/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  I2C GPIO Expander
 *
 */

#include "evmdm6437_i2c_gpio.h"
#include "evmdm6437_gpio.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_GPIO_init( )                                                       *
 *                                                                          *
 *      Initialize I2C GPIO expander                                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_I2C_GPIO_init()
{
    EVMDM6437_I2C_init( );
    EVMDM6437_I2C_GPIO_setupIntr( );

	/* Set the PLL1705 back to power on state */
	EVMDM6437_I2C_GPIO_setall( I2C_GPIO_GROUP_2, 0xff );
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_GPIO_getall( id, input )                                           *
 *                                                                          *
 *      Read all GPIOs in a particular group                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_I2C_GPIO_getall( Uint16 id, Uint16 *input )
{
    return EVMDM6437_I2C_read( id, ( Uint8* )input, 1 );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_GPIO_setall( id, output )                                          *
 *                                                                          *
 *      Set all GPIOs in a particular group.                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_I2C_GPIO_setall( Uint16 id, Uint16 output )
{
    return EVMDM6437_I2C_write( id, ( Uint8* )&output, 1 );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_GPIO_setOutput( id, number, output )                               *
 *                                                                          *
 *      id       <- I2C GPIO Expander ID                                    *
 *      number   <- GPIO PIN# [0:7]                                         *
 *      output   <- 0:LOW 1:HIGH                                            *
 *                                                                          *
 *      Returns:    0: Good                                                 *
 *                 -1: Error                                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_I2C_GPIO_setOutput( Uint16 id, Uint16 number, Uint8 output )
{
    Uint16 current_state, new_state;

    output = output & 1;
    number = ( 1 << ( number & 7 ) );

    if ( EVMDM6437_I2C_GPIO_getall( id, &current_state ) )
        return -1;

    if ( output == 0 )
        new_state = current_state & ~number;
    else
        new_state = current_state | number;

    if ( EVMDM6437_I2C_GPIO_setall( id, new_state ) )
        return -1;

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_GPIO_getInput( id, number )                                        *
 *                                                                          *
 *      id       <- I2C GPIO Expander ID                                    *
 *      number   <- GPIO PIN# [0:7]                                         *
 *                                                                          *
 *      Returns:    0: Input is LOW                                         *
 *                  1: Input is HIGH                                        *
 *                 -1: Error                                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_I2C_GPIO_getInput( Uint16 id, Uint16 number )
{
    Uint16 input = 0;

    if ( EVMDM6437_I2C_GPIO_getall( id, &input ) == 0 )
        return ( input >> number ) & 1;
    else
        return -1;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_GPIO_setupIntr( )                                                  *
 *                                                                          *
 *      Setup Interrupt.                                                    *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_I2C_GPIO_setupIntr( )
{
    _clrPinMux( 0, ( 1 << 12 ) );
    _setPinMux( 0, ( 0 << 12 ) );   // Set for GPIO[4]

    EVMDM6437_GPIO_init( );
    EVMDM6437_GPIO_setDir( GPIO04, GPIO_IN );

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_GPIO_checkIntr( )                                                  *
 *                                                                          *
 *      Check Interrupt.                                                    *
 *                                                                          *
 *      Returns:    0 - NO Interrupt                                        *
 *                  1 - Interrupt Occured                                   *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_I2C_GPIO_checkIntr( )
{
    return EVMDM6437_GPIO_getInput( GPIO04 );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_GPIO_waitForIntr( timeout )                                        *
 *                                                                          *
 *      Wait for Interrupt from I2C GPIOs, stops early on timeout.          *
 *                                                                          *
 *      Returns:    0 - NO Interrupt                                        *
 *                  1 - Interrupt Occured                                   *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_I2C_GPIO_waitForIntr( Int32 timeout )
{
    do
    {
        if ( EVMDM6437_I2C_GPIO_checkIntr( ) )
            return 1;

    } while ( timeout-- > 0 );

    return 0;
}
