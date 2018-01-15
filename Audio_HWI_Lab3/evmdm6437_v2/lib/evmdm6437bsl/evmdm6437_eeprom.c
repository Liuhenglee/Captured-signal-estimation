/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  I2C EEPROM interface
 *
 */

#include "evmdm6437_eeprom.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _EEPROM_init( )                                                         *
 *                                                                          *
 *      Initialize the I2C EEPROM                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_EEPROM_init( )
{
    return EVMDM6437_I2C_init( );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _EEPROM_read( src, dst, length )                                        *
 *                                                                          *
 *      Read from the I2C EEPROM                                            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_EEPROM_read( Uint32 src, Uint32 dst, Uint32 length )
{
    Uint8 addr[2];
    Uint8 *input = ( Uint8* )dst;

    addr[0] = src >> 8;             // HIGH address
    addr[1] = src & 0xFF;           // LOW address

    /* Send 16-bit address */
    if ( EVMDM6437_I2C_write( EEPROM_I2C_ADDR, addr, 2 ) )
        return -1;

    /* Wait for EEPROM to process */
    _waitusec( 1000 );

    /* Read data */
    if ( EVMDM6437_I2C_read ( EEPROM_I2C_ADDR, input, length ) )
        return -1;

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _EEPROM_write( src, dst, length )                                       *
 *                                                                          *
 *      Write to the I2C EEPROM                                             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_EEPROM_write( Uint32 src, Uint32 dst, Uint32 length )
{
    Uint16 i;
    Uint8 cmd[64 + 2];
    Uint8* psrc8 = ( Uint8* )src;
    Uint8* pdst8 = &cmd[2];

    cmd[0] = dst >> 8;              // HIGH address
    cmd[1] = dst & 0xFF;            // LOW address

    /* Fill in data */
    for ( i = 0 ; i < length ; i++ )
        *pdst8++ = *psrc8++;

    /* Send 16-bit address and data */
    if ( EVMDM6437_I2C_write( EEPROM_I2C_ADDR, cmd, 2 + length ) )
        return -1;

    /* Wait for EEPROM to process */
    _waitusec( 1000 );

    return 0;
}
