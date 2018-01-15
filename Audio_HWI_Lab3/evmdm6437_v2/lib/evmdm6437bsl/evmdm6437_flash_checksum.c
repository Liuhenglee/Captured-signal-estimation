/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Compute 32-bit checksum
 *
 */

#include "evmdm6437_flash.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_checksum( start, length )                                        *
 *      Compute the checksum from ( start ) to ( start + length ).  The     *
 *      checksum is a 32-bit cumulative sum of each 8-bit byte in the       *
 *      range.                                                              *
 *                                                                          *
 *      start  <- starting address                                          *
 *      length <- length in bytes                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Uint32 EVMDM6437_FLASH_checksum( Uint32 start, Uint32 length )
{
    Uint32 i;
    Uint32 checksum32 = 0;
    Uint8* ptr = ( Uint8* )start;

    /*
     *  Set to Read Mode
     */
    #ifdef USE_SPANSION
        if ( _FLASH_getMfg( ) == MFG_SPANSION )
            FLASH_BASE_PTR8 = FLASH_RESET;
    #endif

    #ifdef USE_INTEL_MICRON
        if ( ( _FLASH_getMfg( ) == MFG_INTEL )
          || ( _FLASH_getMfg( ) == MFG_MICRON ) )
            FLASH_BASE_PTR8 = FLASH_READ_ARRAY;
    #endif

    /*
     *  Compute Checksum by adding each byte up
     */
    for ( i = 0 ; i < length ; i++ )
        checksum32 += *ptr++;

    return checksum32;
}
