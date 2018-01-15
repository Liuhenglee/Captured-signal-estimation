/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  AIC33 implementation
 *
 */

#include "evmdm6437_aic33.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC33_openCodec( )                                                     *
 *                                                                          *
 *     Open the codec and return a codec handle                             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
AIC33_CodecHandle EVMDM6437_AIC33_openCodec( Uint32 id, AIC33_Config *config )
{
    AIC33_CodecHandle aic33handle = id;

    /* ------------------------------------ *
     *                                      *
     *  AIC33 <-> MCBSP1                    *
     *      .BCLK --> .CLKX1  [input]       *
     *            +-> .CLKR1                *
     *      .WCLK --> .FSX1   [input]       *
     *            +-> .FSR1                 *
     *      .DOUT --> .DR1    [input]       *
     *      .DIN  <-- .DX1    [output]      *
     *                                      *
     * ------------------------------------ */
    if ( aic33handle & MCBSP_INTERFACE )
    {
        EVMDM6437_clrPinMux( 0, ( 3 << 24 ) | ( 3 << 22 ) );
        EVMDM6437_setPinMux( 0, ( 1 << 24 ) | ( 1 << 22 ) );    // MCBSP0/1

        /* Initialize MCBSP1 */
        MCBSP1_PCR  = 0x00000003;
        MCBSP1_RCR  = 0x00010140;
        MCBSP1_XCR  = 0x00010140;
        MCBSP1_SRGR = 0x00000000;
        MCBSP1_SPCR = 0x03010001;

        /* Set Buffer controls */
        EVMDM6437_I2C_GPIO_setOutput( I2C_GPIO_GROUP_3, 0, 1 ); // MCBSP on
        EVMDM6437_I2C_GPIO_setOutput( I2C_GPIO_GROUP_3, 1, 1 ); // MCASP off
        EVMDM6437_I2C_GPIO_setOutput( I2C_GPIO_GROUP_3, 2, 1 ); // SPDIF off

        /* Configure AIC33 */
        EVMDM6437_AIC33_rset( aic33handle, AIC33_PAGESELECT, 0 );// Set to Page0
        EVMDM6437_AIC33_rset( aic33handle, AIC33_RESET, 0x80 ); // Reset AIC33
        EVMDM6437_AIC33_config( aic33handle, config );          // Setup AIC33

        return aic33handle;
    }

    if ( aic33handle & MCASP_INTERFACE )
    {
        /* -------------------------------- *
         *                                  *
         *  AIC33 <-> MCASP0                *
         *      .BCLK --> .ACLKX0 [input]   *
         *            +-> .ACLKR0           *
         *      .WCLK --> .AFSX0  [input]   *
         *            +-> .AFSR0            *
         *      .DOUT --> .AXR0[1] [input]  *
         *      .DIN  <-- .AXR0[0] [output] *
         *                                  *
         * -------------------------------- */
        EVMDM6437_clrPinMux( 0, ( 3 << 24 ) | ( 3 << 22 ) );
        EVMDM6437_setPinMux( 0, ( 2 << 24 ) | ( 2 << 22 ) );    // MCASP0

        /* Set Buffer controls */
        EVMDM6437_I2C_GPIO_setOutput( I2C_GPIO_GROUP_3, 0, 0 ); // MCBSP off
        EVMDM6437_I2C_GPIO_setOutput( I2C_GPIO_GROUP_3, 1, 0 ); // MCASP on
        EVMDM6437_I2C_GPIO_setOutput( I2C_GPIO_GROUP_3, 2, 1 ); // SPDIF off

        /* Configure AIC33 */
        EVMDM6437_AIC33_rset( aic33handle, AIC33_PAGESELECT, 0 );// Select page 0
        EVMDM6437_AIC33_rset( aic33handle, AIC33_RESET, 0x80 ); // Reset AIC33
        EVMDM6437_AIC33_config( aic33handle, config );          // Config AIC33 regs

        /* Initialize MCASP0 */
        EVMDM6437_MCASP_open( MCASP_0 );
        return aic33handle;
    }

    return 0;
}
