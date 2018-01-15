/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Flash Erase
 *
 */

#include "evmdm6437_flash.h"

Int32 erase_timeout = 0x08000000;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_erase( start, length )                                           *
 *      Erase Flash containing address ( start ) to ( start + length ).     *
 *      Flash can only erase entire blocks containing the range.            *
 *                                                                          *
 *      start  <- starting address                                          *
 *      length <- length in bytes                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_FLASH_erase( Uint32 start, Uint32 length )
{
    Uint32 addr;
    Uint32 end;
    #ifdef USE_8_BIT
        volatile Uint8*  addr8;
    #endif
    #ifdef USE_16_BIT
        volatile Uint16* addr16;
    #endif

    end = start + length - 1;       // Calculate end of range
    start &= 0xFFFFFFFE;            // Align to 16-bit words

    /* ---------------------------------------------------------------- *
     *                                                                  *
     *  SPANSION Flash                                                  *
     *                                                                  *
     * ---------------------------------------------------------------- */
    #ifdef USE_SPANSION
        if ( _FLASH_getMfg( ) == MFG_SPANSION )
        {
            /*
             *  Erase each Flash block in the range
             */
            for ( addr = start ; addr <= end ; addr = _FLASH_nextPage( addr ) )
            {
                #ifdef USE_16_BIT
                    addr16 = ( volatile Uint16* )addr;
                    *addr16 = FLASH_RESET;              // Reset Flash
                    FLASH_CTL555 = FLASH_CMD_AA;
                    FLASH_CTL2AA = FLASH_CMD_55;
                    FLASH_CTL555 = FLASH_ERASE;         // Erase sector command
                    FLASH_CTL555 = FLASH_CMD_AA;
                    FLASH_CTL2AA = FLASH_CMD_55;
                    *addr16 = FLASH_ERASE_SECTOR;       // Erase sector confirm

                    if ( _FLASH_waitWhileErasing( addr, erase_timeout ) )
                    {
                        *addr16 = FLASH_RESET;
                        return -1;                      // Timeout/Error occured
                    }
                #endif

                #ifdef USE_8_BIT
                    addr8 = ( volatile Uint8* )addr;
                    *addr8 = FLASH_RESET;               // Reset Flash
                    FLASH_CTLAAA = FLASH_CMD_AA;
                    FLASH_CTL555 = FLASH_CMD_55;
                    FLASH_CTLAAA = FLASH_ERASE;         // Erase sector command
                    FLASH_CTLAAA = FLASH_CMD_AA;
                    FLASH_CTL555 = FLASH_CMD_55;
                    *addr8 = FLASH_ERASE_SECTOR;        // Erase sector confirm

                    if ( _FLASH_waitWhileErasing( addr, erase_timeout ) )
                    {
                        *addr8 = FLASH_RESET;
                        return -1;                      // Timeout/Error occured
                    }
                #endif
            }
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
            /*
             *  Erase each Flash block in the range
             */
            for ( addr = start ; addr <= end ; addr = _FLASH_nextPage( addr ) )
            {
                #ifdef USE_16_BIT
                    addr16 = ( volatile Uint16* )addr;
                    *addr16 = FLASH_READ_ARRAY;         // Reset Flash
                    *addr16 = FLASH_BLOCK_ERASE;        // Erase block command
                    *addr16 = FLASH_ERASE_PRG_RESUME;   // Erase block confirm

                    if ( _FLASH_waitWhileBusy( erase_timeout ) )
                    {
                        *addr16 = FLASH_READ_ARRAY;
                        return -1;                      // Timeout/Error occured
                    }

                    *addr16 = FLASH_READ_ARRAY;
                #endif

                #ifdef USE_8_BIT
                    addr8 = ( volatile Uint8* )( addr );
                    *addr8 = FLASH_READ_ARRAY;          // Reset Flash
                    *addr8 = FLASH_BLOCK_ERASE;         // Erase block command
                    *addr8 = FLASH_ERASE_PRG_RESUME;    // Erase block confirm

                    if ( _FLASH_waitWhileBusy( erase_timeout ) )
                    {
                        *addr8 = FLASH_READ_ARRAY;
                        return -1;                      // Timeout/Error occured
                    }

                    *addr8 = FLASH_READ_ARRAY;
                #endif
            }
        }
    #endif

    return 0;
}
