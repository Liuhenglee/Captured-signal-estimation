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
 *  _AIC33_rget( aic33handle, regnum, regval )                              *
 *                                                                          *
 *      Return value of codec register regnum                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_AIC33_rget( AIC33_CodecHandle aic33handle, Uint16 regnum, Uint16* regval )
{
    Int16 i2c_addr = aic33handle & 0x007F;
    Int16 retcode = 0;
    Uint8 cmd[2];

    cmd[0] = regnum & 0x007F;       // 7-bit Device Address
    cmd[1] = 0;

    retcode |= EVMDM6437_I2C_write( i2c_addr, cmd, 1 );
    retcode |= EVMDM6437_I2C_read( i2c_addr, cmd, 1 );

    *regval = cmd[0];

    return retcode;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC33_rset( aic33handle, regnum, regval )                              *
 *                                                                          *
 *      Set codec register regnum to value regval                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_AIC33_rset( AIC33_CodecHandle aic33handle, Uint16 regnum, Uint16 regval )
{
    Int16 i2c_addr = aic33handle & 0x007F;
    Uint8 cmd[2];
    cmd[0] = regnum & 0x007F;       // 7-bit Device Address
    cmd[1] = regval;                // 8-bit Register Data

    return EVMDM6437_I2C_write( i2c_addr, cmd, 2 );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC33_rset_mask( aic33handle, regnum, regval, mask )                   *
 *                                                                          *
 *      Set codec register regnum to value regval                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_AIC33_rset_mask( AIC33_CodecHandle aic33handle, Uint16 regnum,
                                 Uint16 regval, Uint8 mask )
{
    Int16 retcode = 0;
    Uint16 regval_save;

    /*
     *  Read, mask, then write register
     */
    retcode |= EVMDM6437_AIC33_rget( aic33handle, regnum, &regval_save );

    regval_save &= ~mask;
    regval_save |= regval & mask;

    retcode |= EVMDM6437_AIC33_rset( aic33handle, regnum, regval_save );

    return retcode;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC33_config( aic33handle, config )                                    *
 *                                                                          *
 *     Set the default codec register config values                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_AIC33_config( AIC33_CodecHandle aic33handle, AIC33_Config *config )
{
    Int16 retcode = 0;
    Int16 i;

    if ( config == 0 )
        return -1;

    /*
     *  Configure every non-reserved, non-status register
     */
    for ( i = 0 ; i < AIC33_NUMREGS ; i++ )
        if ( ( config->regs[i] & 0xFF00 ) == 0 )
            retcode |= EVMDM6437_AIC33_rset( aic33handle, i, config->regs[i] );

    return retcode;
}
