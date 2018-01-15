/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  PLL Setup
 *
 */

#include "evmdm6437_pll.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _enablePll1( clock_source, pll_mult )                                   *
 *                                                                          *
 *      clock_source    <- 0: Onchip Oscillator                             *
 *                         1: External Clock                                *
 *                                                                          *
 *      pll_mult        <- 21: 22x Multiplier * 27MHz Clk = 594 MHz         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_enablePll1( Uint16 clock_source, Uint16 pll_mult )
{
    volatile Uint32* pll_ctl    = ( volatile Uint32* )&PLL1_PLLCTL;
    volatile Uint32* pll_pllm   = ( volatile Uint32* )&PLL1_PLLM;
    volatile Uint32* pll_cmd    = ( volatile Uint32* )&PLL1_PLLCMD;
    volatile Uint32* pll_stat   = ( volatile Uint32* )&PLL1_PLLSTAT;
    volatile Uint32* pll_div1   = ( volatile Uint32* )&PLL1_PLLDIV1;
    volatile Uint32* pll_div2   = ( volatile Uint32* )&PLL1_PLLDIV2;
    volatile Uint32* pll_div3   = ( volatile Uint32* )&PLL1_PLLDIV3;

    int power_up_pll = ( ( *pll_ctl & 0x0002 ) >> 1 );

    /*
     *  Step 0 - Ignore request if the PLL is already set as is
     */
    if ( ( ( *pll_ctl & 0x0100 ) >> 8 ) == clock_source )
    {
        if ( *pll_pllm == ( pll_mult & 0x3F ) )
        {
            return 0;
        }
    }

    /*
     *  Step 1 - Set clock mode
     */
    if ( power_up_pll == 1 )
    {
        if ( clock_source == 0 )
            *pll_ctl &= ~0x0100;    // Onchip Oscillator
        else
            *pll_ctl |= 0x0100;     // External Clock
    }

    /*
     *  Step 2 - Set PLL to bypass
     *         - Wait for PLL to stabilize
     */
    *pll_ctl &= ~0x0021;
    _wait( 150 );

    /*
     *  Step 3 - Reset PLL
     */
    *pll_ctl &= ~0x0008;

    /*
     *  Step 4 - Disable PLL
     *  Step 5 - Powerup PLL
     *  Step 6 - Enable PLL
     *  Step 7 - Wait for PLL to stabilize
     */
    if ( power_up_pll == 1 )
    {
        *pll_ctl |= 0x0010;         // Disable PLL
        *pll_ctl &= ~0x0002;        // Power up PLL
        *pll_ctl &= ~0x0010;        // Enable PLL
        _wait( 150 );               // Wait for PLL to stabilize
    }
    else
        *pll_ctl &= ~0x0010;        // Enable PLL

    /*
     *  Step 8 - Load PLL multiplier
     */
    *pll_pllm = pll_mult & 0x3F;

    /*
     *  Step 9 - Load PLL dividers ( must be in a 1/3/6 ratio )
     *           1:DSP, 2:SCR,EMDA,VPSS, 3:Peripherals
     */
    *pll_div1 = 0x8000 | 0;         // Divide-by-1
    *pll_div2 = 0x8000 | 2;         // Divide-by-3
    *pll_div3 = 0x8000 | 5;         // Divide-by-6
    *pll_cmd |= 0x0001;             // Set phase alignment
    while( ( *pll_stat & 1 ) != 0 );// Wait for phase alignment
    _wait( 2000 );

    /*
     *  Step 10 - Wait for PLL to reset ( 2000 cycles )
     *  Step 11 - Release from reset
     */
    *pll_ctl |= 0x0008;
    _wait( 2000 );

    /*
     *  Step 12 - Wait for PLL to re-lock ( 2000 cycles )
     *  Step 13 - Switch out of BYPASS mode
     */
    *pll_ctl |= 0x0001;
    _wait( 2000 );

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _enablePll2( clock_source, pll_mult, vpss_div, ddr2_div )               *
 *                                                                          *
 *      clock_source    <- 0: Onchip Oscillator                             *
 *                         1: External Clock                                *
 *                                                                          *
 *      pll_mult        <- PLL Multiplier                                   *
 *                         23: 24x Multiplier * 27MHz Clk = 648 MHz         *
 *                                                                          *
 *      vpss_div        <- VPSS divider ( For PLL2 )                        *
 *                         11: 648 MHz Clk / 12x Divider = 54 MHz           *
 *                                                                          *
 *      ddr2_div        <- DDR2 divider ( For PLL2 )                        *
 *                         1: 648 MHz Clk / 4x Divider = 162 MHz            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_enablePll2( Uint16 clock_source, Uint16 pll_mult,
                            Uint16 vpss_div, Uint16 ddr2_div )
{
    volatile Uint32* pll_ctl    = ( volatile Uint32* )&PLL2_PLLCTL;
    volatile Uint32* pll_pllm   = ( volatile Uint32* )&PLL2_PLLM;
    volatile Uint32* pll_cmd    = ( volatile Uint32* )&PLL2_PLLCMD;
    volatile Uint32* pll_stat   = ( volatile Uint32* )&PLL2_PLLSTAT;
    volatile Uint32* pll_div1   = ( volatile Uint32* )&PLL2_PLLDIV1;
    volatile Uint32* pll_div2   = ( volatile Uint32* )&PLL2_PLLDIV2;

    int power_up_pll = ( ( *pll_ctl & 0x0002 ) >> 1 );

    /*
     *  Step 0 - Ignore request if the PLL is already set as is
     */
    if ( ( ( *pll_ctl & 0x0100 ) >> 8 ) == clock_source )
        if ( *pll_pllm == ( pll_mult & 0x3F ) )
            if ( ( *pll_div2 & 0x1F ) == ( vpss_div & 0x1F ) )
                if ( ( *pll_div1 & 0x1F ) == ( ddr2_div & 0x1F ) )
                    return 0;

    /*
     *  Step 1 - Set clock mode
     */
    if ( power_up_pll == 1 )
    {
        if ( clock_source == 0 )
            *pll_ctl &= ~0x0100;    // Onchip Oscillator
        else
            *pll_ctl |= 0x0100;     // External Clock
    }

    /*
     *  Step 2 - Set PLL to bypass
     *         - Wait for PLL to stabilize
     */
    *pll_ctl &= ~0x0021;
    _wait( 150 );

    /*
     *  Step 3 - Reset PLL
     */
    *pll_ctl &= ~0x0008;

    /*
     *  Step 4 - Disable PLL
     *  Step 5 - Powerup PLL
     *  Step 6 - Enable PLL
     *  Step 7 - Wait for PLL to stabilize
     */
    if ( power_up_pll == 1 )
    {
        *pll_ctl |= 0x0010;         // Disable PLL
        *pll_ctl &= ~0x0002;        // Power up PLL
        *pll_ctl &= ~0x0010;        // Enable PLL
        _wait( 150 );               // Wait for PLL to stabilize
    }
    else
        *pll_ctl &= ~0x0010;        // Enable PLL

    /*
     *  Step 8 - Load PLL multiplier
     */
    *pll_pllm = pll_mult & 0x3F;

    /*
     *  Step 9 - Load PLL dividers ( must be in a 1/3/6 ratio )
     *           1:DDR2, 2:VPSS-VPBE
     */
    *pll_div1 = 0x8000 | ( ddr2_div & 0x1F );
    *pll_div2 = 0x8000 | ( vpss_div & 0x1F );
    *pll_cmd |= 0x0001;             // Set phase alignment
    while( ( *pll_stat & 1 ) != 0 );// Wait for phase alignment
    _wait( 2000 );

    /*
     *  Step 10 - Wait for PLL to reset ( 2000 cycles )
     *  Step 11 - Release from reset
     */
    *pll_ctl |= 0x0008;
    _wait( 2000 );

    /*
     *  Step 12 - Wait for PLL to re-lock ( 2000 cycles )
     *  Step 13 - Switch out of BYPASS mode
     */
    *pll_ctl |= 0x0001;
    _wait( 2000 );

    return 0;
}
