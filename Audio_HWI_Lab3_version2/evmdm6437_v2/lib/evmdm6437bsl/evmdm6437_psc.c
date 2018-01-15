/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Power Sleep Controller
 *
 */

#include "evmdm6437_psc.h"

static Int32 psc_timeout = 0x00010000;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _PSC_enableAll( )                                                       *
 *                                                                          *
 *      This turns on all clocks in ALWAYSON and DSP power domains.         *
 *      Note this function assumes that the Power Domains are already on.   *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_PSC_enableAll( )
{
    Int16 i;

    /*
     *  Enable all non reserved power domains
     */
    for ( i = 0 ; i <= 9 ; i++ )
        EVMDM6437_PSC_changeState( i, PSC_ENABLE );
    for ( i = 11 ; i <= 28 ; i++ )
        EVMDM6437_PSC_changeState( i, PSC_ENABLE );
    i = 39;
        EVMDM6437_PSC_changeState( i, PSC_ENABLE );

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _PSC_changeState( id, state )                                           *
 *                                                                          *
 *      Change power module state to ( ENABLE, DISABLE, SYNCRESET, RESET )  *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_PSC_changeState( Uint32 id, Uint16 state )
{
    Int32 timeout;
    volatile Uint32* mdstat = ( Uint32* )( PSC_MDSTAT_BASE + ( 4 * id ) );
    volatile Uint32* mdctl  = ( Uint32* )( PSC_MDCTL_BASE  + ( 4 * id ) );

    /*
     *  Step 0 - Ignore request if the state is already set as is
     */
    if ( ( *mdstat & 0x001F ) == state )
        return 0;

    /*
     *  Step 1 - Wait for PTSTAT.GOSTAT to clear
     */
    timeout = psc_timeout;
    while( PSC_PTSTAT & 1 )
        if ( timeout-- < 0 )
            return -1;

    /*
     *  Step 2 - Set MDCTLx.NEXT to new state
     */
    *mdctl &= ~0x001F;
    *mdctl |= state;

    /*
     *  Step 3 - Start power transition ( set PTCMD.GO to 1 )
     */
    PSC_PTCMD = 0x0001;

    /*
     *  Step 4 - Wait for PTSTAT.GOSTAT to clear
     */
    timeout = psc_timeout;
    while( PSC_PTSTAT & 1 )
        if ( timeout-- < 0 )
            return -1;

    /*
     *  Step 5 - Verify state changed
     */
    timeout = psc_timeout;
    while( ( *mdstat & 0x001F ) != state )
        if ( timeout-- < 0 )
            return -1;

    return 0;
}
