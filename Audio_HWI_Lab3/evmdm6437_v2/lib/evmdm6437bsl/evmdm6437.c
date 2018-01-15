/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Board Setup
 *
 */
#include "evmdm6437.h"
#include "evmdm6437_i2c_gpio.h"

/*
 *  For GEL startup code
 */
#include "evmdm6437_ddr.h"
#include "evmdm6437_pll.h"
#include "evmdm6437_psc.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  EVMDM6437_startupGEL( )                                                 *
 *                                                                          *
 *      Same setup that the GEL file uses                                   *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_startupGEL( )
{
    Uint32 evmdm6437_ddr_init_fn;

    EVMDM6437_PSC_enableAll( );             // Turn on all Power
    EVMDM6437_enablePll1( 0, 21 );          // [DSP @ 594MHz][Core @ 1.20V]
    EVMDM6437_enablePll2( 0, 23, 11, 1 );   // [VPSS @ 54MHz][DDR @ 162MHz]

    evmdm6437_ddr_init_fn = ( Uint32 )&EVMDM6437_DDR_init;
    if ( ! ( ( evmdm6437_ddr_init_fn >= DDR_BASE )
          && ( evmdm6437_ddr_init_fn < ( DDR_BASE + DDR_SIZE ) ) ) )
        EVMDM6437_DDR_init( 162 );          // Setup DDR @ 162MHz

    EVMDM6437_FLASH_init( );                // Setup Flash
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  EVMDM6437_init( )                                                       *
 *                                                                          *
 *      Setup I2C & I2C GPIO                                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_init( )
{
    /*
     *  Setup Pin Mux for board default
     */
    EVMDM6437_setupPinMux( );

    /*
     *  PSC, PLL, DDR, & AEMIF are already configured in the GEL file
     *  However the function _startupGEL can be called to perform the
     *  same effect as the GEL file.
     */
    EVMDM6437_startupGEL( );

    /*
     *  Setup I2C/I2C GPIO Expander
     */
    EVMDM6437_I2C_init( );
    EVMDM6437_I2C_GPIO_init( );

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _setupPinMux( )                                                         *
 *                                                                          *
 *      Pin Mux can always be changed w/o any further setup, all that this  *
 *      function does is to set the initial state to a known state.         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 _setupPinMux( )
{
    /* ---------------------------------------------------------------- *
     *                                                                  *
     *  PINMUX settings for normal operation                            *
     *                                                                  *
     *   EVM switch settings:                                           *
     *   AEM0 = Off, AEM1 = On, AEM2 = On (EMIFA enabled)               *
     *   AEAW2 = Off (20-bit address mode)                              *
     *   BOOTM0 = Off, BOOTM1 = Off, BOOTM2 = On, BOOTM3 = Off          *
     *      (direct EMIFA boot at 0x42000000)                           *
     *                                                                  *
     *   8-bit EMIFA (full address mode)                                *
     *   8-bit video input on YI[7:0]                                   *
     *   Digital video outputs disabled                                 *
     *   CS3-CS5 disabled                                               *
     *   McBSP0, McBSP1 enabled                                         *
     *   UART1 enabled                                                  *
     *   Timer 0 enabled (connected to VIC output)                      *
     *   PWM1 enabled (connected to VIC)                                *
     *   VLYNQ enabled                                                  *
     *   MII/MDIO enabled                                               *
     *   PCI disabled                                                   *
     *                                                                  *
     * ---------------------------------------------------------------- */

    CFG_PINMUX0 = 0
        | ( 0 << 30 )   // CI10SEL   - No CI[1:0]
        | ( 0 << 28 )   // CI32SEL   - No CI[3:2]
        | ( 0 << 26 )   // CI54SEL   - No CI[5:4]
        | ( 0 << 25 )   // CI76SEL   - No CI[7:6]
        | ( 0 << 24 )   // CFLDSEL   - No C_FIELD
        | ( 0 << 23 )   // CWENSEL   - No C_WEN
        | ( 0 << 22 )   // HDVSEL    - No CCDC HD and VD
        | ( 1 << 20 )   // CCDCSEL   - CCDC PCLK, YI[7:0] enabled
        | ( 4 << 16 )   // AEAW      - EMIFA full address mode
        | ( 1 << 15 )   // VPBECLK   - VPBECLK enabled
        | ( 0 << 12 )   // RGBSEL    - No digital outputs
        | ( 0 << 10 )   // CS3SEL    - LCD_OE/EM_CS3 disabled
        | ( 0 <<  8 )   // CS4SEL    - CS4/VSYNC disabled
        | ( 0 <<  6 )   // CS5SEL    - CS5/HSYNC disabled
        | ( 0 <<  4 )   // VENCSEL   - Video encoder outputs disabled
        | ( 1 <<  0 );  // AEM       - 8-bit EMIFA, 8-bit CCDC in

    CFG_PINMUX1 = 0
        | ( 1 << 24 )   // SPBK1     - McBSP1 enabled
        | ( 1 << 22 )   // SPBK0     - McBSP0 enabled
        | ( 2 << 20 )   // TIM1BK    - UART1 enabled, Timer1 disabled
        | ( 1 << 16 )   // TIM0BK    - Timer0 enabled
        | ( 2 << 14 )   // CKOBK     - CLKOUT disabled, PWM2 enabled
        | ( 0 << 12 )   // PWM1BK    - PWM1 disabled, GIO84 enabled
        | ( 1 << 10 )   // UR0FCBK   - UART0 HW flow control enabled
        | ( 1 <<  8 )   // UR0DBK    - UART0 data enabled
        | ( 3 <<  4 )   // HOSTBK    - VLYNQ + MII + MDIO Mode
        | ( 0 <<  0 );  // PCIEN     - PCI disabled

    CFG_VDD3P3V_PWDN = 0x0000;      // Everything on
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _setPinMux( pinmux0, pinmux1 )                                          *
 *                                                                          *
 *      Enable pin mux depending on the value of ( pinmux0 ) & ( pinmux1 )  *
 *      The 1's bit indicates which field to enable.                        *
 *                                                                          *
 *      pinmux0 <- Pin Mux Reg 0                                            *
 *      pinmux1 <- Pin Mux Reg 1                                            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 _setPinMux( Uint32 pinmux0, Uint32 pinmux1 )
{
    CFG_PINMUX0 |= pinmux0;
    CFG_PINMUX1 |= pinmux1;
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _clrPinMux( pinmux0, pinmux1 )                                          *
 *                                                                          *
 *      Diasble pin mux depending on the value of ( pinmux0 ) & ( pinmux1 ) *
 *      The 1's bit indicates which field to enable.                        *
 *                                                                          *
 *      pinmux0 <- Pin Mux Reg 0                                            *
 *      pinmux1 <- Pin Mux Reg 1                                            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 _clrPinMux( Uint32 pinmux0, Uint32 pinmux1 )
{
    CFG_PINMUX0 &= ~pinmux0;
    CFG_PINMUX1 &= ~pinmux1;
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _resetEMIF( )                                                           *
 *                                                                          *
 *      Reset EMIF timings to have MAX timeout                              *
 *                                                                          *
 *      @ 594 MHz, AEMIF clock is set to ( 99 MHz or 10.1 ns )              *
 *                                                                          *
 *      chip_select     <- Chip Select [2-5]                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 _resetEMIF( Uint16 chip_select )
{
    Uint32 emif_max_timeout = 0
        | ( 0  << 31 )      // Select Strobe    [ Normal ]
        | ( 0  << 30 )      // Extended Wait    [ Disabled ]
        | ( 15 << 26 )      // Write Setup      [ 151.5 ns ]
        | ( 31 << 20 )      // Write Strobe     [ 313.1 ns ]
        | ( 7  << 17 )      // Write Hold       [  70.7 ns ]
        | ( 15 << 13 )      // Read Setup       [ 151.5 ns ]
        | ( 31 << 7 )       // Read Strobe      [ 313.1 ns ]
        | ( 7  << 4 )       // Read Hold        [  70.7 ns ]
        | ( 3  << 2 )       // Turn Around      [  30.3 ns ]
        | ( 0  << 0 );      // Bus Size         [ 8-bit Bus ]

    switch ( chip_select )
    {
        case 2:
            AEMIF_A1CR = emif_max_timeout;
            AEMIF_NANDFCR &= ~( 1 << 0 );       // NAND Flash [ OFF ]
            break;
        case 3:
            AEMIF_A2CR = emif_max_timeout;
            AEMIF_NANDFCR &= ~( 1 << 1 );       // NAND Flash [ OFF ]
            break;
        case 4:
            AEMIF_A3CR = emif_max_timeout;
            AEMIF_NANDFCR &= ~( 1 << 2 );       // NAND Flash [ OFF ]
            break;
        case 5:
            AEMIF_A4CR = emif_max_timeout;
            AEMIF_NANDFCR &= ~( 1 << 3 );       // NAND Flash [ OFF ]
            break;
        default:
            return -1;
    }
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _setupEMIF( chip_select, emif_timing, emif_mode  )                      *
 *                                                                          *
 *      Set individual EMIF timings                                         *
 *                                                                          *
 *      @ 594 MHz, AEMIF clock is set to ( 99 MHz or 10.1 ns )              *
 *                                                                          *
 *      chip_select     <- Chip Select [2-5]                                *
 *      emif_timing     <- Timing config                                    *
 *      emif_mode       <- Setup for NAND Flash                             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 _setupEMIF( Uint16 chip_select, Uint32 emif_timing, Uint16 emif_mode )
{
    Uint32 nand_enable_bit = 1 << ( chip_select - 2 );

    switch ( chip_select )
    {
        case 2:
            AEMIF_A1CR = emif_timing;
            break;
        case 3:
            AEMIF_A2CR = emif_timing;
            break;
        case 4:
            AEMIF_A3CR = emif_timing;
            break;
        case 5:
            AEMIF_A4CR = emif_timing;
            break;
        default:
            return -1;
    }

    if ( emif_mode == EMIF_NAND_MODE )
        AEMIF_NANDFCR |= nand_enable_bit;   // NAND Flash [ ON ]

    if ( emif_mode == EMIF_NORMAL_MODE );
        AEMIF_NANDFCR &= ~nand_enable_bit;  // NAND Flash [ OFF ]

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _wait( delay )                                                          *
 *                                                                          *
 *      Wait in a software loop for 'x' delay                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void _wait( Uint32 delay )
{
    volatile Uint32 i;
    for ( i = 0 ; i < delay ; i++ ){ };
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _waitusec( usec )                                                       *
 *                                                                          *
 *      Wait in a software loop for 'x' microseconds                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void _waitusec( Uint32 usec )
{
    _wait( usec * 28 );
}
