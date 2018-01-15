/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 *
 *  Not for distribution.
 */

/*
 *  AIC33 implementation
 *
 */

#include "evmdm6437_aic33.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC33_mute( )                                                          *
 *                                                                          *
 *      Enable/disable codec mute mode                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void EVMDM6437_AIC33_mute( AIC33_CodecHandle aic33handle, Uint16 mode )
{
    if ( mode )
    {
        EVMDM6437_AIC33_rset( aic33handle, 43, 0x80 );    //  Left DAC Mute = ON
        EVMDM6437_AIC33_rset( aic33handle, 44, 0x80 );    // Right DAC Mute = ON
    }
    else
    {
        EVMDM6437_AIC33_rset( aic33handle, 43, 0x00 );    //  Left DAC Mute = OFF
        EVMDM6437_AIC33_rset( aic33handle, 44, 0x00 );    // Right DAC Mute = OFF
    }
}
