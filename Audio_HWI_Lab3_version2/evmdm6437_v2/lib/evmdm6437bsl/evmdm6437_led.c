/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  LED implementation
 *
 */

#include "evmdm6437_led.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _LED_init( )                                                            *
 *                                                                          *
 *      Clear LEDs                                                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_LED_init( )
{
    /* Turn OFF all LEDs */
    return EVMDM6437_I2C_GPIO_setall( LED_I2C_ADDR, 0x000F );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _LED_on( number )                                                       *
 *                                                                          *
 *      number <- LED# [0:3]                                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_LED_on( Uint16 number )
{
    Uint16 led_state;
    Uint16 led_bit_on;

    if ( number > 3 )
        return 1;

    led_bit_on = 1 << number;

    /*
     *  Get then set LED
     */
    if ( EVMDM6437_I2C_GPIO_getall( LED_I2C_ADDR, &led_state ) )
        return -1;

    led_state = led_state & ( ~led_bit_on );

    if ( EVMDM6437_I2C_GPIO_setall( LED_I2C_ADDR, led_state ) )
        return -1;

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _LED_off( number )                                                      *
 *                                                                          *
 *      number <- LED# [0:3]                                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_LED_off( Uint16 number )
{
    Uint16 led_state;
    Uint16 led_bit_off;

    if ( number > 3 )
        return 1;

    led_bit_off = 1 << number;

    /*
     *  Get then set LED
     */
    if ( EVMDM6437_I2C_GPIO_getall( LED_I2C_ADDR, &led_state ) )
        return -1;

    led_state =  led_state | led_bit_off;

    if ( EVMDM6437_I2C_GPIO_setall( LED_I2C_ADDR, led_state ) )
        return -1;

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _LED_toggle( number )                                                   *
 *                                                                          *
 *      number <- LED# [0:3]                                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_LED_toggle( Uint16 number )
{
    Uint16 led_state;
    Uint16 new_led_state;
    Uint16 led_bit_toggle;

    if ( number > 3 )
        return 1;

    led_bit_toggle = 1 << ( number & 0x03 );

    /*
     *  Get then set LED
     */
    if ( EVMDM6437_I2C_GPIO_getall( LED_I2C_ADDR, &led_state ) )
        return -1;

    if ( ( led_state & led_bit_toggle ) == 0 )
        new_led_state = led_state | led_bit_toggle;     /* Turn OFF */
    else
        new_led_state = led_state & ~led_bit_toggle;    /* Turn ON */

    if ( EVMDM6437_I2C_GPIO_setall( LED_I2C_ADDR, new_led_state ) )
        return -1;

    return 0;
}
