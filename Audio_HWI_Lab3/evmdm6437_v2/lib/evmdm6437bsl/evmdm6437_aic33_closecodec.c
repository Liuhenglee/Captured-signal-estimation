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
 *  _AIC33_closeCodec( aic33handle )                                        *
 *                                                                          *
 *     Close the codec                                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_AIC33_closeCodec( AIC33_CodecHandle aic33handle )
{
    if ( aic33handle & MCBSP_INTERFACE )
    {
        MCBSP1_PCR  = 0x00000000;           // Close MCBSP1
        MCBSP1_RCR  = 0x00000000;
        MCBSP1_XCR  = 0x00000000;
        MCBSP1_SRGR = 0x00000000;
        MCBSP1_SPCR = 0x00000000;
    }

    if ( aic33handle & MCASP_INTERFACE )
    {
       EVMDM6437_MCASP_close( MCASP_0 );    // Close McASP0
    }

    EVMDM6437_AIC33_rset( aic33handle, AIC33_PAGESELECT, 0 ); // Select Page 0
    EVMDM6437_AIC33_rset( aic33handle, AIC33_RESET, 0x80 );   // Reset the AIC33

    return 0;
}

