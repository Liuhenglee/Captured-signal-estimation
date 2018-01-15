/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Flash Read
 *
 */

#include "evmdm6437_flash.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_read( src, dst, length )                                         *
 *      Read from Flash address ( src ) to the data at non-Flash address    *
 *      ( dst ) for ( length ) bytes.                                       *
 *                                                                          *
 *      src     <- source address                                           *
 *      dest    <- destination address                                      *
 *      length  <- length in bytes                                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_FLASH_read( Uint32 src, Uint32 dst, Uint32 length )
{
    Uint32 i;

    #ifdef USE_16_BIT
        Uint16* psrc16 = ( Uint16* )src;
        Uint16* pdst16 = ( Uint16* )dst;
    #endif
    #ifdef USE_8_BIT
        Uint8* psrc8 = ( Uint8* )src;
        Uint8* pdst8 = ( Uint8* )dst;
    #endif


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
     *  Read Data to Buffer
     */
    #ifdef USE_16_BIT
        for ( i = 0 ; i < length ; i += 2 )
            *pdst16++ = *psrc16++;
    #endif
    #ifdef USE_8_BIT
        for ( i = 0 ; i < length ; i++ )
            *pdst8++ = *psrc8++;
    #endif

    return 0;
}
