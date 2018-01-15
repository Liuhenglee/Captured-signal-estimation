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
 *  _AIC33_write16( aic33handle, data16 )                                   *
 *                                                                          *
 *      Write a 16-bit value to the codec                                   *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_AIC33_write16( AIC33_CodecHandle aic33handle, Int16 data16 )
{
    /* Process McASP requests first */
    if ( aic33handle & MCASP_INTERFACE )
    {
        if ( ( MCASP0_SRCTL0 & 0x10 ) == 0 )            // Check for Tx ready
            return 0;

        MCASP0_XBUF0_32BIT = data16 << 15;              // Write 16-bits of data
        return 1;
    }

    /* Process McBSP requests */
    if ( aic33handle & MCBSP_INTERFACE )
    {
        if ( ( MCBSP1_SPCR & MCBSP_SPCR_XRDY ) == 0 )   // Check for Tx ready
            return 0;

        MCBSP1_DXR_16BIT = data16;                      // Write 16-bits of data
        return 1;
    }

    return -1;
}
