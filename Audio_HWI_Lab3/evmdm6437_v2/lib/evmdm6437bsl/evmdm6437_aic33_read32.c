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
 *  _AIC33_read32( aic33handle, data32 )                                    *
 *                                                                          *
 *      Read a 32-bit value from the codec                                  *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_AIC33_read32( AIC33_CodecHandle aic33handle, Int32 *data32 )
{
    /* Process McASP requests first */
    if ( aic33handle & MCASP_INTERFACE )
    {
        if ( ( MCASP0_SRCTL1 & 0x20 ) == 0 )            // Check for Rx ready
            return 0;

        *data32 = MCASP0_RBUF1_32BIT;                   // Read 32-bits of data
        return 1;
    }

    /* Process McBSP requests */
    if ( aic33handle & MCBSP_INTERFACE )
    {
        if ( ( MCBSP1_SPCR & MCBSP_SPCR_RRDY ) == 0 )   // Check for Rx ready
            return 0;

        *data32 = MCBSP1_DRR_32BIT;                     // Read 32-bits of data
        return 1;
    }

    return -1;
}
