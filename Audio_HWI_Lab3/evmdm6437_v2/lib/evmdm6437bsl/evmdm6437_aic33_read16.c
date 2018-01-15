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
 *  _AIC33_read16( aic33handle, data16 )                                    *
 *                                                                          *
 *      Read a 16-bit value from the codec                                  *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_AIC33_read16( AIC33_CodecHandle aic33handle, Int16 *data16 )
{
    /* Process McASP requests first */
    if ( aic33handle & MCASP_INTERFACE )
    {
        if ( ( MCASP0_SRCTL1 & 0x20 ) == 0 )            // Check for Rx ready
            return 0;

        *data16 = MCASP0_RBUF1_16BIT;                   // Read 16-bits of data
        return 1;
    }

    /* Process McBSP requests */
    if ( aic33handle & MCBSP_INTERFACE )
    {
        if ( ( MCBSP1_SPCR & MCBSP_SPCR_RRDY ) == 0 )   // Check for Rx ready
            return 0;

        *data16 = MCBSP1_DRR_16BIT;                     // Read 16-bits of data
        return 1;
    }

    return -1;
}
