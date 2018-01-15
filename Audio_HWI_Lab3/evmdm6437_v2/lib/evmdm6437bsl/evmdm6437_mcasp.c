/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  MCASP implementation
 *
 */

#include "evmdm6437_mcasp.h"

static MCASP_Handle mcasp_table[4] = {
    ( MCASP_Handle )&MCASP_MODULE_0,
};

static MCASP_Handle mcasp;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _MCASP_setRGBLCTL( )                                                    *
 *      Set GBLCTL bits                                                     *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void _MCASP_setRGBLCTL( Uint32 bit )
{
    mcasp->regs->RGBLCTL |= bit;
    while ( ( mcasp->regs->RGBLCTL & bit ) != bit );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _MCASP_setXGBLCTL( )                                                    *
 *      Set GBLCTL bits                                                     *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void _MCASP_setXGBLCTL( Uint32 bit )
{
    mcasp->regs->XGBLCTL |= bit;
    while ( ( mcasp->regs->XGBLCTL & bit ) != bit );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _MCASP_open( id )                                                       *
 *      Setup MCASP                                                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_MCASP_open( Uint16 id )
{
    if ( id > 0 )
        return -1;

    mcasp = mcasp_table[id];

    /* ---------------------------------------------------------------- *
     *                                                                  *
     *  AIC33 is in MASTER mode.                                        *
     *      BCLK & WCLK come from AIC33                                 *
     *      DIN is used by write16/write32                              *
     *      DOUT is usec by read16/read32                               *
     *                                                                  *
     * ---------------------------------------------------------------- */
    mcasp->regs->GBLCTL  = 0;       // Reset
    mcasp->regs->RGBLCTL = 0;       // Reset RX
    mcasp->regs->XGBLCTL = 0;       // Reset TX
    mcasp->regs->PWRDEMU = 1;       // Free-running

    /* RX */
    mcasp->regs->RMASK      = 0xffffffff; // No padding used
    mcasp->regs->RFMT       = 0x00008078; // MSB 16bit, 1-delay, no pad, CFGBus
    mcasp->regs->AFSRCTL    = 0x00000110; // 2TDM, 1bit Rising, EXT FS, word
    mcasp->regs->ACLKRCTL   = 0x00000080; // Rising EXT CLK, div-by-1
    mcasp->regs->AHCLKRCTL  = 0x00008033; // INT CLK
    mcasp->regs->RTDM       = 0x00000003; // Slots 0,1
    mcasp->regs->RINTCTL    = 0x00000000; // Not used
    mcasp->regs->RCLKCHK    = 0x00FF0008; // 255-MAX 0-MIN, div-by-256

    /* TX */
    mcasp->regs->XMASK      = 0xffffffff; // No padding used
    mcasp->regs->XFMT       = 0x00008078; // MSB 16bit, 1-delay, no pad, CFGBus
    mcasp->regs->AFSXCTL    = 0x00000110; // 2TDM, 1bit Rising edge EXT FS, word
    mcasp->regs->ACLKXCTL   = 0x00000080; // ASYNC, Rising EXT CLK, div-by-1
    mcasp->regs->AHCLKXCTL  = 0x00008033; // INT CLK
    mcasp->regs->XTDM       = 0x00000003; // Slots 0,1
    mcasp->regs->XINTCTL    = 0x00000000; // Not used
    mcasp->regs->XCLKCHK    = 0x00FF0008; // 255-MAX 0-MIN, div-by-256

    mcasp->regs->SRCTL0     = 0x000D;     // MCASP0.AXR0[0] --> DIN
    mcasp->regs->SRCTL1     = 0x000E;     // MCASP0.AXR0[1] <-- DOUT
    mcasp->regs->PFUNC      = 0;          // All MCASPs
    mcasp->regs->PDIR       = 0x00000001; // All inputs except AXR0[0]

    mcasp->regs->DITCTL     = 0x00000000; // Not used
    mcasp->regs->DLBCTL     = 0x00000000; // Not used
    mcasp->regs->AMUTE      = 0x00000000; // Not used

    _MCASP_setXGBLCTL( GBLCTL_XHCLKRST_ON );// HS Clk
    _MCASP_setRGBLCTL( GBLCTL_RHCLKRST_ON );// HS Clk

    _MCASP_setXGBLCTL( GBLCTL_XCLKRST_ON ); // Clk
    _MCASP_setRGBLCTL( GBLCTL_RCLKRST_ON ); // Clk

    mcasp->regs->XSTAT = 0x0000ffff;        // Clear all
    mcasp->regs->RSTAT = 0x0000ffff;        // Clear all

    _MCASP_setXGBLCTL( GBLCTL_XSRCLR_ON );  // Serialize
    _MCASP_setRGBLCTL( GBLCTL_RSRCLR_ON );  // Serialize

    /* Write a 0, so that no underrun occurs after releasing the state machine */
    mcasp->regs->XBUF0 = 0;
  //mcasp->regs->RBUF0 = 0;

    _MCASP_setXGBLCTL( GBLCTL_XSMRST_ON );  // State Machine
    _MCASP_setRGBLCTL( GBLCTL_RSMRST_ON );  // State Machine

    _MCASP_setXGBLCTL( GBLCTL_XFRST_ON );   // Frame Sync
    _MCASP_setRGBLCTL( GBLCTL_RFRST_ON );   // Frame Sync

    /* Start by sending a dummy write */
    while( ! ( mcasp->regs->SRCTL0 & 0x10 ) );  // Check for Tx ready
    mcasp->regs->XBUF0 = 0;

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _MCASP_close( id )                                                      *
 *      Close McASP                                                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_MCASP_close( Uint16 id )
{
    if ( id > 3 )
        return -1;
    else
        mcasp = mcasp_table[id];

    /* Serializers */
    mcasp->regs->SRCTL0 = 0;
    mcasp->regs->SRCTL1 = 0;
    mcasp->regs->SRCTL2 = 0;
    mcasp->regs->SRCTL3 = 0;
    mcasp->regs->GBLCTL = 0; // Reset
    return 0;
}
