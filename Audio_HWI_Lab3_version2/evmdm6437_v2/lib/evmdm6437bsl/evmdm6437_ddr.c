/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  DDR Setup
 *
 */

#include "evmdm6437_ddr.h"
#include "evmdm6437_psc.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _DDR_init( freq )                                                       *
 *                                                                          *
 *      Setup DDR2 Memory                                                   *
 *                                                                          *
 *      Using 2 x MT47H32M16 - ( 8Mb x 16 bits x 4 banks ) @ 162 MHz        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_DDR_init( Uint32 freq )
{
    volatile Uint32 dummy_read;
    volatile Uint32 pch_nch;
    Uint32 refresh_rate;

    Uint32 evmdm6437_ddr_init_fn = ( Uint32 )&EVMDM6437_DDR_init;
    if ( ( evmdm6437_ddr_init_fn > ( DDR_BASE ) )
      && ( evmdm6437_ddr_init_fn < ( DDR_BASE + DDR_SIZE ) ) )
        return 1;

    /*
     *  This Code supports only one freq of 162 MHz => 6.17 ns
     *  If another freq is desired, modify SDTIMR & SDTIMR2 fields.
     */
    freq = 162;

    /*
     *  Step 1 - Setup PLL2
     *  Step 2 - Enable DDR2 PHY
     */
    EVMDM6437_PSC_changeState( 13, PSC_ENABLE );

    /*
     *  Step 3 - DDR2 Initialization
     */
    DDR_DDRPHYCR = 0                // DDR PHY Control Register
        | ( 0x1400190 << 6 )        // Magic number
        | ( 0 << 5 )                // DLL release
        | ( 0 << 4 )                // DLL powered up
        | ( 5 << 0 );               // Read latency ( CAS + RT - 1 )

    DDR_SDBCR = 0                   // DDR Bank Config
        | ( 0 << 23 )               // Boot unlock  = No
        | ( 2 << 19 )               // Reserved     = ( 2 )
        | ( 0 << 18 )               // Drive Strength= Normal
        | ( 3 << 16 )               // Reserved     = ( 3 )
        | ( 1 << 15 )               // Modify SDBCR = Yes
        | ( 0 << 14 )               // Bus width    = 32-bit
        | ( 4 << 9 )                // CAS latency  = 4
        | ( 2 << 4 )                // Bank Setup   = 4 banks
        | ( 2 << 0 );               // Page Size    = 1024-word / 10 column bits

    DDR_SDTIMR = 0                  // DDR Timing Register
        | ( 17 << 25 )              // tRFC = ( 105 ns / 6.17 ns ) - 1
        | ( 2 << 22 )               // tRP  = (  15 ns / 6.17 ns ) - 1
        | ( 2 << 19 )               // tRCD = (  15 ns / 6.17 ns ) - 1
        | ( 2 << 16 )               // tWR  = (  15 ns / 6.17 ns ) - 1
        | ( 6 << 11 )               // tRAS = (  40 ns / 6.17 ns ) - 1
        | ( 8 << 6 )                // tRC  = (  55 ns / 6.17 ns ) - 1
        | ( 1 << 3 )                // tRRD = ( 7.5 ns / 6.17 ns ) - 1
        | ( 1 << 0 );               // tWTR = ( 7.5 ns / 6.17 ns ) - 1

    DDR_SDTIMR2 = 0                 // DDR Timing Register
        | ( 18 << 16 )              // tXSNR= ( 115 ns / 6.17 ns ) - 1
        | ( 199 << 8 )              // tXSRD= ( 200 - 1 ) cycles
        | ( 1 << 5 )                // tRTP = ( 7.5 ns / 6.17 ns ) - 1
        | ( 2 << 0 );               // tCKE = ( 3 - 1 ) cycles

    DDR_SDBCR &= ~0x00008000;       // SDBCR cannot be modified

    refresh_rate = ( freq * 78 ) / 10;

    DDR_SDRCR = 0
        | ( 0 << 31 )               // Exits self-refresh
        | ( 0 << 30 )               // Disable MCLK stopping
        | refresh_rate;             // Refresh Control = 7.8 usec * freq

    /*
     *  Step 4 - Dummy Read
     */
    dummy_read = *( volatile Uint32* )DDR_BASE;

    /*
     *  Step 5 - Soft Reset ( SYNCRESET followed by ENABLE ) of DDR2 PHY
     */
    EVMDM6437_PSC_changeState( 13, PSC_SYNCRESET );
    EVMDM6437_PSC_changeState( 13, PSC_ENABLE );

    /*
     *  Step 6 - Enable VTP calibration
     *  Step 7 - Wait for VTP calibration ( 33 VTP cycles )
     */
    DDR_VTPIOCR = 0x201F;
    DDR_VTPIOCR = 0xA01F;
    _wait( 1500 );

    /*
     *  Step 8 - Enable access to DDR VTP reg
     *  Step 9 - Reat P & N channels
     *  Step 10 - Set VTP fields PCH & NCH
     */
    DDR_DDRVTPER = 1;
    pch_nch = DDR_DDRVTPR & 0x3FF;
    DDR_VTPIOCR = 0xA000 | pch_nch;

    /*
     *  Step 11 - Disable VTP calibaration
     *          - Disable access to DDR VTP register
     */
    DDR_VTPIOCR &= ~0x2000;
    DDR_DDRVTPER = 0;
    return 0;
}
