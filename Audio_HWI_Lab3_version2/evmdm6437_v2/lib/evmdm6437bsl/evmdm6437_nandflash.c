/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  NAND Flash interface
 *
 */

#include "evmdm6437_nandflash.h"

/*
 *  Invalid Block & Page list
 */
static Uint16 invalid_blk_count = 0;
static Uint16 invalid_blks[80];

static Uint16 invalid_pg_count = 0;
static Uint16 invalid_pgs[80];

/*
 *  MFG & Device IDs
 */
static Uint8 mfg_id;
static Uint8 dev_id;

/*
 *  NAND Flash timeout
 */
static Uint32 nand_timeout = ( 0x00100000 );

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _NAND_readECC( )                                                        *
 *                                                                          *
 *      Read ECC calcualtions                                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#ifdef USE_ECC
    inline Uint32 _NAND_readECC( void )
    {
        return AEMIF_NANDECC2;
    }
#endif

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _NAND_startECC( )                                                       *
 *                                                                          *
 *      Start ECC calcualtions                                              *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#ifdef USE_ECC
    inline void _NAND_startECC( void )
    {
        _NAND_readECC( );
        AEMIF_NANDFCR |= 0x0100;
    }
#endif

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _NAND_getBadPages( pages )                                              *
 *                                                                          *
 *      Get the last invaild pages                                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void _NAND_getBadPages( Uint32* pages )
{
    Int16 i;
    for ( i = 0 ; invalid_pg_count ; i++ )
        pages[i] = invalid_pgs[i];

    if ( invalid_pg_count == 0 )
        pages[0] = 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _NAND_getBadBlocks( blocks )                                            *
 *                                                                          *
 *      Get the last invaild blocks                                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void _NAND_getBadBlocks( Uint32* blocks )
{
    Int16 i;
    for ( i = 0 ; invalid_blk_count ; i++ )
        blocks[i] = invalid_blks[i];

    if ( invalid_blk_count == 0 )
        blocks[0] = 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _NAND_busywait( timeout )                                               *
 *                                                                          *
 *      Poll the external RDY/BSY pin                                       *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 _NAND_busywait( Int32 timeout )
{
    /*
     *  Short delay to let the Ready/Busy signal go LOW
     */
    _wait( 200 );

    /*
     *  Wait while the device is busy
     */
    while( ( ! NAND_READ_RB ) && ( timeout-- > 0 ) );

    if ( timeout == 0 )
        return NAND_ERR_TIMEOUT;
    else
        return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _NANDFLASH_init( )                                                      *
 *      Initialize the NAND Flash                                           *
 *                                                                          *
 *  Note:                                                                   *
 *      The Write Protect on the NAND Flash is disabled.  This allows the   *
 *      erase & write NAND flash to work.                                   *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_NANDFLASH_init( )
{
    /*------------------------------------------------------------------*
     *                                                                  *
     *  NAND Flash timing parameters                                    *
     *                                                                  *
     *      EMIF.CLK freq   = PLL1/6 = 594/6 = 99 MHz                   *
     *      EMIF.CLK period = 1/99 MHz = 10.1 ns                        *
     *                                                                  *
     *------------------------------------------------------------------*/
    Uint32 acfg2 = 0
            | ( 0 << 31 )           // selectStrobe
            | ( 0 << 30 )           // extWait
            | ( 0 << 26 )           // writeSetup      //   0 ns
            | ( 8 << 20 )           // writeStrobe     //  40 ns
            | ( 2 << 17 )           // writeHold       //  10 ns
            | ( 0 << 13 )           // readSetup       //   0 ns
            | ( 6 << 7 )            // readStrobe      //  25 ns
            | ( 2 << 4 )            // readHold        //  10 ns
            | ( 2 << 2 )            // turnAround      //  10 ns
            | ( 0 << 0 );           // asyncSize       //  8-bit bus

    _resetEMIF( EMIF_CS2 );
    _setupEMIF( EMIF_CS2, acfg2, EMIF_NAND_MODE );

    /* ---------------------------------------------------------------- *
     *                                                                  *
     *  Note: If NANDFLASH_CE_DO_CARE is defined, then CE will be low   *
     *        during read/write/erase operations.                       *
     *                                                                  *
     *        Else if NANDFLASH_CE_DO_NOT_CARE is defined, then CE will *
     *        only be low during accesses to the NAND Flash device.     *
     *                                                                  *
     * ---------------------------------------------------------------- */

  //NAND_ASSERT_CE( );                  // Assert CE ( for CE-care devices )

    NAND_CMD( CMD_RESET );              // Reset NAND Flash

    if ( _NAND_busywait( nand_timeout ) )
        return NAND_ERR_TIMEOUT;

  //NAND_DEASSERT_CE( );                // Deassert CE ( for CE-care devices )

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _NANDFLASH_getTotalPages( )                                             *
 *                                                                          *
 *      Determine the total number of pages of NAND Flash                   *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Uint32 EVMDM6437_NANDFLASH_getTotalPages( )
{
    Uint32 pages = 0;

  //NAND_ASSERT_CE( );                  // Assert CE ( for CE-care devices )

    NAND_CMD( CMD_READID );             // Issue Read ID command
    NAND_ADDR_1( 0 );

    mfg_id = NAND_DATA;                 // Read MFG Id
    dev_id = NAND_DATA;                 // Read Device Id

    /*
     *  Compared MFG ID & Device ID to the supported devices
     */
    if ( mfg_id == MFG_SAMSUNG )
    {
        switch ( dev_id )
        {
            case DEV_K9F5608U0B:
                pages = DEV_K9F5608U0B_PAGE_COUNT;
                break;

            case DEV_K9F5608Q0B:
                pages = DEV_K9F5608Q0B_PAGE_COUNT;
                break;

            case DEV_K9F2808U0C:
                pages = DEV_K9F2808U0C_PAGE_COUNT;
                break;

            case DEV_K9K1208Q0C:
                pages = DEV_K9K1208Q0C_PAGE_COUNT;
                break;
        }
    }
    else if ( mfg_id == MFG_STI )
    {
        switch( dev_id )
        {
            case DEV_NAND512W3A:
                pages = DEV_NAND512W3A_PAGE_COUNT;
                break;
        }
    }

  //NAND_DEASSERT_CE( );                // Deassert CE ( for CE-care devices )

    return pages;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _NANDFLASH_erase( start, block_count )                                  *
 *                                                                          *
 *      Erase the blocks of Flash memory                                    *
 *          start = destination address [ block address ]                   *
 *          block_count = # of blocks   [ 1 block = 16384 bytes ]           *
 *                                                                          *
 *      Returns:    0   pass                                                *
 *                  1+  failed - bad blocks                                 *
 *                 -1   timeout                                             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_NANDFLASH_erase( Uint32 start, Uint32 block_count )
{
    Uint32 i;

    invalid_blk_count = 0;

    for ( i = 0 ; i < block_count ; i++ )
    {
      //NAND_ASSERT_CE( );              // Assert CE ( for CE-care devices )

        NAND_CMD( CMD_ERASE );          // Erase block
        NAND_ADDR_3( start );
        NAND_CMD( CMD_ERASE_CONFIRM );  // Confirm Erase

        if ( _NAND_busywait( nand_timeout ) )
        {
          //NAND_DEASSERT_CE( );        // Deassert CE ( for CE-care devices )
            return NAND_ERR_TIMEOUT;    // Timeout Error
        }

        NAND_CMD( CMD_STATUS );         // Check Status

        if ( NAND_DATA & CMD_STATUS_ERROR )
            invalid_blks[invalid_blk_count++] = start;

      //NAND_DEASSERT_CE( );            // Deassert CE ( for CE-care devices )

        start += NAND_BLOCKSIZE;        // Increment address
    }
    return invalid_blk_count;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _NANDFLASH_readPage( src, dst, page_count )                             *
 *                                                                          *
 *      Read data from NAND Flash by pages ( 512 bytes )                    *
 *                                                                          *
 *          src = source address      [ must be aligned to start of page ]  *
 *          dst = destination address [ any address ]                       *
 *          page_count = # of pages   [ a page is 512 bytes ]               *
 *                                                                          *
 *      Note does not return spare array data                               *
 *                                                                          *
 *      Returns:    0   pass                                                *
 *                  1+  failed - bad pages                                  *
 *                 -1   timeout                                             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_NANDFLASH_readPage( Uint32 src, Uint32 dst, Uint32 page_count )
{
    Uint32 i, j;
    volatile Uint8* dst8 = ( volatile Uint8* )dst;
    Uint8 spare[NAND_SPARESIZE];

    #ifdef USE_ECC
        Uint32 computed_ecc;
        Uint8  spare_ecc[4];
        Uint32 spare_ecc32;
    #endif

    invalid_pg_count = 0;

    for ( i = 0 ; i < page_count ; i++ )
    {
      //NAND_ASSERT_CE( );              // Assert CE ( for CE-care devices )

        NAND_CMD( CMD_READ );           // Read page
        NAND_ADDR_4( src );

        if ( _NAND_busywait( nand_timeout ) )
        {
          //NAND_DEASSERT_CE( );        // Deassert CE ( for CE-care devices )
            return NAND_ERR_TIMEOUT;    // Timeout Error
        }

        #ifdef USE_ECC
            _NAND_startECC( );          // Start ECC
        #endif

        j = NAND_PAGESIZE;
        while ( j-- )
            *dst8++ = NAND_DATA;        // Read MAIN array

        #ifdef USE_ECC
            computed_ecc = _NAND_readECC( ); // Read computed ECC
            computed_ecc &= 0x0FFF0FFF;
        #endif

        for ( j = 0 ; j < NAND_SPARESIZE ; j++ )
            spare[j] = NAND_DATA;       // Read SPARE leftovers

        #ifdef USE_ECC
            for ( j = 0 ; j < 4 ; j++ )
                spare_ecc[j] = spare[j];// Read SPARE ECC

            spare_ecc32 = *( Uint32* )( &spare_ecc[0] );
            spare_ecc32 &= 0x0FFF0FFF;

            if ( spare_ecc32 != computed_ecc )// Compare ECCs
                invalid_pgs[invalid_pg_count++] = src;
        #endif

      //NAND_DEASSERT_CE( );            // Deassert CE ( for CE-care devices )

        src += NAND_PAGESIZE;           // Increment address
    }

    return invalid_pg_count;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _NANDFLASH_writePage( src, dst, page_count )                            *
 *                                                                          *
 *      Program NAND Flash.                                                 *
 *          src = source address      [ any address ]                       *
 *          dst = destination address [ must be aligned to start of page ]  *
 *          page_count = # of pages   [ a page is 512 bytes ]               *
 *                                                                          *
 *      Note does not program SPARE arrays                                  *
 *                                                                          *
 *      Returns:    0   pass                                                *
 *                  1+  failed - bad pages                                  *
 *                 -1   timeout                                             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_NANDFLASH_writePage( Uint32 src, Uint32 dst, Uint32 page_count )
{
    Uint32 i, j = 0;
    volatile Uint8* src8 = ( volatile Uint8* )src;

    #ifdef USE_ECC
        Uint32 computed_ecc;
    #endif

    invalid_pg_count = 0;

    for ( i = 0 ; i < page_count ; i++ )
    {
      //NAND_ASSERT_CE( );              // Assert CE ( for CE-care devices )

        NAND_CMD( CMD_PROGRAM );        // Program page
        NAND_ADDR_4( dst );

        #ifdef USE_ECC
            _NAND_startECC( );          // Start ECC calculation
        #endif

        j = NAND_PAGESIZE;
        while ( j-- )                   // Write MAIN array
            NAND_DATA = *src8++;

        #ifdef USE_ECC
            computed_ecc = _NAND_readECC( ); // Read computed ECC
            computed_ecc &= 0x0FFF0FFF;

            NAND_DATA = ( computed_ecc >> 24 );
            NAND_DATA = ( computed_ecc >> 16 );
            NAND_DATA = ( computed_ecc >> 8 );
            NAND_DATA = ( computed_ecc );

            j = NAND_SPARESIZE - 4;
        #else
            j = NAND_SPARESIZE;
        #endif

        while ( j-- );                  // Write Spare array
            NAND_DATA = 0xFF;        

        NAND_CMD( CMD_PROGRAM_CONFIRM );// Confirm Program

        if ( _NAND_busywait( nand_timeout ) )
        {
          //NAND_DEASSERT_CE( );        // Deassert CE ( for CE-care devices )
            return NAND_ERR_TIMEOUT;    // Timeout Error
        }

        NAND_CMD( CMD_STATUS );         // Check Status

        if ( NAND_DATA & CMD_STATUS_ERROR )
            invalid_pgs[invalid_pg_count++] = dst;

      //NAND_DEASSERT_CE( );            // Deassert CE ( for CE-care devices )

        dst += NAND_PAGESIZE;           // Increment address
    }

    return invalid_pg_count;
}
