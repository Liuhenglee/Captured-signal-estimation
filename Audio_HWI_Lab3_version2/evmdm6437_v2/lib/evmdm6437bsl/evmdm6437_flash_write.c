/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Flash Write
 *
 */

#include "evmdm6437_flash.h"

Int32 write_timeout = 0x00010000;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_write( src, dst, length )                                        *
 *      Write to Flash address [dst] the data at a non-Flash address [src]  *
 *      for [length] #bytes.                                                *
 *                                                                          *
 *      src     <- source address                                           *
 *      dest    <- destination address                                      *
 *      length  <- length in bytes                                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_FLASH_write( Uint32 src, Uint32 dst, Uint32 length )
{
    Uint32 i;
    Uint8* psrc8;

    #ifdef USE_16_BIT
        Uint16* pdst16;

        Uint8 oddlen;
        Uint16 word_to_write = 0;
        Uint8* lower_byte = ( Uint8* )( &word_to_write );
        Uint8* upper_byte = ( Uint8* )( lower_byte + 1 );

        /* Align to 16-bit Flash memory */
        if ( ( dst & 1 ) == 1 )
        {
            *lower_byte = *( Uint8* )( dst - 1 );
            *upper_byte = *( Uint8* )( src );
            EVMDM6437_FLASH_write( ( Uint32 )&word_to_write, dst - 1, 2 );
            dst = dst + 1;
            src = src + 1;
            length = length - 1;
        }

        /*
         *  Align to 8 or 16 bits
         */
        psrc8  = ( Uint8* )src;
        pdst16 = ( Uint16* )dst;
        oddlen = length & 1;
        length &= 0xFFFFFFFE;
    #endif

    #ifdef USE_8_BIT
        Uint8* pdst8;
        psrc8 = ( Uint8* )src;
        pdst8 = ( Uint8* )dst;
    #endif

    /* ---------------------------------------------------------------- *
     *                                                                  *
     *  SPANSION Flash                                                  *
     *                                                                  *
     * ---------------------------------------------------------------- */
    #ifdef USE_SPANSION
        if ( _FLASH_getMfg( ) == MFG_SPANSION )
        {
            #ifdef USE_16_BIT
                for ( i = 0 ; i < length ; i += 2 )
                {
                    /*
                     *  Read the 16-bit source data as 2 x 8-bit bytes.
                     *  This seems rather odd, but avoids any paging problems when
                     *  data is across multiple cache pages and the MMU is active.
                     */
                    *lower_byte = *psrc8++;
                    *upper_byte = *psrc8++;

                    /* Program one 16-bit word */
                    FLASH_CTL555 = FLASH_CMD_AA;
                    FLASH_CTL2AA = FLASH_CMD_55;
                    FLASH_CTL555 = FLASH_PROGRAM;
                    *pdst16 = word_to_write;

                    /* Wait for programming to complete */
                    if ( _FLASH_waitWhileProgramming( ( Uint32 )pdst16, word_to_write, write_timeout ) )
                        return -1;

                    pdst16++;
                }
            #endif
            #ifdef USE_8_BIT
                for ( i = 0 ; i < length ; i++ )
                {
                    /* Program one 8-bit byte */
                    FLASH_CTLAAA = FLASH_CMD_AA;
                    FLASH_CTL555 = FLASH_CMD_55;
                    FLASH_CTLAAA = FLASH_PROGRAM;
                    *pdst8 = *psrc8;

                    /* Wait for programming to complete */
                    if ( _FLASH_waitWhileProgramming( ( Uint32 )pdst8, *psrc8, write_timeout ) )
                        return -1;

                    pdst8++;
                    psrc8++;
                }
            #endif
        }

    #endif

    /* ---------------------------------------------------------------- *
     *                                                                  *
     *  Intel/Micron Flash                                              *
     *                                                                  *
     * ---------------------------------------------------------------- */
    #ifdef USE_INTEL_MICRON
        if ( ( _FLASH_getMfg( ) == MFG_INTEL )
          || ( _FLASH_getMfg( ) == MFG_MICRON ) )
        {
            #ifdef USE_16_BIT
                for ( i = 0 ; i < length ; i += 2 )
                {
                    /*
                     *  Read the 16-bit source data as 2 x 8-bit bytes.
                     *  This seems rather odd, but avoids any paging problems when
                     *  data is across multiple cache pages and the MMU is active.
                     */
                    *lower_byte = *psrc8++;
                    *upper_byte = *psrc8++;

                    /* Program one 16-bit word */
                    *pdst16 = FLASH_WORD_PROGRAM;
                    *pdst16 = word_to_write;

                    /* Wait for operation to complete */
                    if ( _FLASH_waitWhileBusy( write_timeout ) )
                        return -1;

                    pdst16++;
                }
            #endif
            #ifdef USE_8_BIT
                for ( i = 0 ; i < length ; i++ )
                {
                    /* Program one 8-bit byte */
                    *pdst8 = FLASH_WORD_PROGRAM;
                    *pdst8 = *psrc8;

                    /* Wait for operation to complete */
                    if ( _FLASH_waitWhileBusy( write_timeout ) )
                        return -1;

                    pdst8++;
                    psrc8++;
                }
            #endif
        }
    #endif

    #ifdef USE_16_BIT
        /* Align 16-bit SRC last */
        if ( oddlen == 1 )
        {
            *lower_byte = *psrc8;
            *upper_byte = ( *pdst16 ) >> 8;
            EVMDM6437_FLASH_write( ( Uint32 )&word_to_write, ( Uint32 )pdst16, 2 );
        }
    #endif

    return 0;
}
