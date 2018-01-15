/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Flash Setup
 *
 */

#include "evmdm6437_flash.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_init( )                                                          *
 *      Setup Flash                                                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_FLASH_init( )
{
    Uint32 acfg2;

    _resetEMIF( 2 );

    EVMDM6437_FLASH_getTotalPages( 1 );

    /*------------------------------------------------------------------*
     *                                                                  *
     *  Flash Timing Parameters                                         *
     *                                                                  *
     *      EMIF.CLK freq   = PLL1/6 = 594/6 = 99 MHz                   *
     *      EMIF.CLK period = 1/99 MHz = 10.1 ns                        *
     *                                                                  *
     *------------------------------------------------------------------*/
    #ifdef USE_SPANSION
        if ( _FLASH_getMfg( ) == MFG_SPANSION )
        {
            acfg2 = 0
                | ( 0 << 31 )           // selectStrobe
                | ( 0 << 30 )           // extWait
                | ( 0 << 26 )           // writeSetup      //   0 ns
                | ( 5 << 20 )           // writeStrobe     //  50 ns
                | ( 0 << 17 )           // writeHold       //   0 ns
                | ( 1 << 13 )           // readSetup       //   0 ns
                | ( 8 << 7 )            // readStrobe      //  80 ns
                | ( 3 << 4 )            // readHold        //  30 ns
                | ( 3 << 2 )            // turnAround      //  10 ns
                | ( 0 << 0 );           // asyncSize       // 8-bit bus

            _setupEMIF( EMIF_CS2, acfg2, EMIF_NORMAL_MODE );

            FLASH_BASE_PTR8 = FLASH_RESET;
        }
    #endif

    #ifdef USE_INTEL_MICRON
        if ( ( _FLASH_getMfg( ) == MFG_INTEL )
          || ( _FLASH_getMfg( ) == MFG_MICRON ) )
        {
            acfg2 = 0
                | ( 0 << 31 )           // selectStrobe
                | ( 0 << 30 )           // extWait
                | ( 0 << 26 )           // writeSetup      //   0 ns
                | ( 4 << 20 )           // writeStrobe     //  35 ns
                | ( 0 << 17 )           // writeHold       //   0 ns
                | ( 4 << 13 )           // readSetup       //  30 ns
                | ( 12 << 7 )           // readStrobe      // 120 ns
                | ( 0 << 4 )            // readHold        //   0 ns
                | ( 1 << 2 )            // turnAround      //  10 ns
                | ( 0 << 0 );           // asyncSize       // 8-bit bus

            _setupEMIF( EMIF_CS2, acfg2, EMIF_NORMAL_MODE );

            _FLASH_unlockBlocks( FLASH_BASE, FLASH_END_OR_RANGE );

            FLASH_BASE_PTR8 = FLASH_READ_ARRAY;
        }
    #endif

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_unlockBlocks( )                                                  *
 *                                                                          *
 *      Flash unlock blocks ( Intel/Micron Flash )                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#ifdef USE_INTEL_MICRON
    Int16 _FLASH_unlockBlocks( Uint32 start, Uint32 length )
    {
        Uint32 end = start + length - 1;
        Uint32 addr;
        volatile Uint16 *pblock;

        if ( ( _FLASH_getMfg( ) == MFG_INTEL )
          || ( _FLASH_getMfg( ) == MFG_MICRON ) )
        {
            for ( addr = start ; addr <= end ; addr = _FLASH_nextPage( addr ) )
            {
                #if ( 0 )
                    pblock = ( volatile Uint16* )addr;
                    *pblock = FLASH_CONFIG_BLOCK_LOCK_BITS;      // Lock Setup
                    *pblock = FLASH_CLEAR_LOCK_BITS;             // Unlock Confirm
                    *pblock = FLASH_READ_ARRAY;                  // Read Mode
                #else
                    do /* Optional unlock + check */
                    {
                        pblock = ( volatile Uint16* )addr;
                        *pblock = FLASH_CONFIG_BLOCK_LOCK_BITS;  // Lock Setup
                        *pblock = FLASH_CLEAR_LOCK_BITS;         // Lock Confirm
                        *pblock = FLASH_READ_ID;                 // Read Device ID
                        pblock = ( volatile Uint16* )( addr + 2 );// Read Status
                    } while ( ( *pblock & 0x03 ) != 0 );
                    *pblock = FLASH_READ_ARRAY;                  // Read Mode
                    *pblock = FLASH_CLEAR_STATUS;                // Clear status reg
                #endif
            }
        }
        return 0;
    }
#endif

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_waitWhileBusy( )                                                 *
 *                                                                          *
 *      Wait for the Busy bit ( Intel/Micron Flash )                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#ifdef USE_INTEL_MICRON
    Int16 _FLASH_waitWhileBusy( Int32 timeout )
    {
        if ( ( _FLASH_getMfg( ) == MFG_INTEL )
          || ( _FLASH_getMfg( ) == MFG_MICRON ) )
        {
            /* Read Status Register */
            FLASH_BASE_PTR8 = FLASH_READ_STATUS;

            while ( timeout-- > 0 )
            {
                /*
                 *  The most important bit in the status is the busy bit.
                 *  When asserted the operation is done.
                 */
                #ifdef USE_16_BIT
                    if ( ( FLASH_BASE_PTR16 & 0x0080 )
                      || ( FLASH_BASE_PTR16 & 0x8000 ) )
                    {
                        /*
                         *  Check for any errors
                         */
                        if ( ( ( FLASH_BASE_PTR16 & 0x007F ) != 0 )
                          || ( ( FLASH_BASE_PTR16 & 0x7F00 ) != 0 ) )
                        {
                            /* Bad, operation finished w/ errors */
                            FLASH_BASE_PTR16 = FLASH_READ_ARRAY;
                            return -1;
                        }
                        else
                        {
                            /* Good, operation is finished w/o errors */
                            FLASH_BASE_PTR16 = FLASH_READ_ARRAY;
                            return 0;
                        }
                    }
                #endif
                #ifdef USE_8_BIT
                    if ( FLASH_BASE_PTR8 & 0x80 )
                    {
                        /*
                         *  Check for any errors
                         */
                        if ( ( FLASH_BASE_PTR8 & 0x7F ) != 0 )
                        {
                            /* Bad, operation finished w/ errors */
                            FLASH_BASE_PTR8 = FLASH_READ_ARRAY;
                            return -1;
                        }
                        else
                        {
                            /* Good, operation is finished w/o errors */
                            FLASH_BASE_PTR8 = FLASH_READ_ARRAY;
                            return 0;
                        }
                    }
                #endif
            }

            /* Timeout occured */
            FLASH_BASE_PTR8 = FLASH_READ_ARRAY;
        }
        return 1;
    }
#endif

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_waitWhileErasing( )                                              *
 *                                                                          *
 *      Wait while erasing ( SPANSION Flash )                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#ifdef USE_SPANSION
    Int16 _FLASH_waitWhileErasing( Uint32 addr, Int32 timeout )
    {
        Uint8* pdata = ( Uint8* )addr;

        if ( _FLASH_getMfg( ) == MFG_SPANSION )
        {
            while ( timeout-- > 0 )
                if ( *pdata == 0xFF )
                    /* Good, erase completed */
                    return 0;

            /* Timeout occured */
            FLASH_BASE_PTR8 = FLASH_RESET;
            return 1;
        }
        return -1;
    }
#endif

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_waitWhileProgramming( )                                          *
 *                                                                          *
 *      Wait while programming ( SPANSION Flash )                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#ifdef USE_SPANSION
    #ifdef USE_16_BIT
        Int16 _FLASH_waitWhileProgramming( Uint32 addr, Uint16 data, Int32 timeout )
        {
            Uint16* pdata = ( Uint16* )addr;

            if ( _FLASH_getMfg( ) == MFG_SPANSION )
            {
                while ( timeout-- > 0 )
                    if ( *pdata == data )
                        /* Good, programming completed */
                        return 0;

                /* Timeout occured */
                FLASH_BASE_PTR8 = FLASH_RESET;
                return 1;
            }
            return -1;
        }
    #endif
    #ifdef USE_8_BIT
        Int16 _FLASH_waitWhileProgramming( Uint32 addr, Uint8 data, Int32 timeout )
        {
            Uint8* pdata = ( Uint8* )addr;

            if ( _FLASH_getMfg( ) == MFG_SPANSION )
            {
                while ( timeout-- > 0 )
                    if ( *pdata == data )
                        /* Good, programming completed */
                        return 0;

                /* Timeout occured */
                FLASH_BASE_PTR8 = FLASH_RESET;
                return 1;
            }
            return -1;
        }
    #endif
#endif
