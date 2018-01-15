/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Compute number Flash pages
 *
 */

#include "evmdm6437_flash.h"

static Int32 device_mfg = 0;
static Int32 device_devid[3];
static Int32 device_flash_pages = 0;
static Int32 device_flash_page_length = 0;
static Int32 device_flash_page_len_pow2 = 0;

#ifdef USE_INTEL_MICRON
    static Int32 device_page_alignment = 0;
#endif

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_getId( )                                                         *
 *                                                                          *
 *      Who am I?  Intel/Micron based Flash or SPANSION based Flash         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int32 _FLASH_getId( )
{
    Uint32 addr = FLASH_BASE;

    #ifdef USE_16_BIT
        volatile Uint16* addr16  = ( Uint16* )addr;
        volatile Uint16* pmfgid  = ( Uint16* )addr;
        volatile Uint16* pdevid1 = ( Uint16* )( addr + 0x02 );
        volatile Uint16* pdevid2 = ( Uint16* )( addr + 0x1C );
        volatile Uint16* pdevid3 = ( Uint16* )( addr + 0x1E );
        volatile Uint16* flash_ctl555 = ( Uint16* )( addr + 0xAAA );
        volatile Uint16* flash_ctl2AA = ( Uint16* )( addr + 0x554 );
    #endif

    #ifdef USE_8_BIT
        volatile Uint8* addr8   = ( Uint8* )addr;
        volatile Uint8* pmfgid  = ( Uint8* )addr;
        volatile Uint8* pdevid1 = ( Uint8* )( addr + 0x02 );
        volatile Uint8* pdevid2 = ( Uint8* )( addr + 0x1C );
        volatile Uint8* pdevid3 = ( Uint8* )( addr + 0x1E );
        volatile Uint8* flash_ctlAAA = ( Uint8* )( addr + 0xAAA );
        volatile Uint8* flash_ctl555 = ( Uint8* )( addr + 0x555 );
    #endif

    /*------------------------------------------------------------------*
     *                                                                  *
     *  Am I Spansion based Flash?                                      *
     *                                                                  *
     *------------------------------------------------------------------*/
    #ifdef USE_SPANSION
        #ifdef USE_16_BIT
            *addr16 = FLASH_RESET;              // Read Mode
            *flash_ctl555 = FLASH_CMD_AA;
            *flash_ctl2AA = FLASH_CMD_55;
            *flash_ctl555 = FLASH_ID;           // Read ID Mode

            device_mfg      = *pmfgid;          // Read MFG_ID
            device_devid[0] = *pdevid1;         // Read DEV_ID
            device_devid[1] = *pdevid2;
            device_devid[2] = *pdevid3;

            *addr16 = FLASH_RESET;
        #endif

        #ifdef USE_8_BIT
            *addr8 = FLASH_RESET;               // Read Mode
            *flash_ctlAAA = FLASH_CMD_AA;
            *flash_ctl555 = FLASH_CMD_55;
            *flash_ctlAAA = FLASH_ID;           // Read ID Mode

            device_mfg      = *pmfgid;          // Read MFG_ID
            device_devid[0] = *pdevid1;         // Read DEV_ID
            device_devid[1] = *pdevid2;
            device_devid[2] = *pdevid3;

            *addr8 = FLASH_RESET;
        #endif

        if ( device_mfg == MFG_SPANSION )
            return MFG_SPANSION;                // Spansion Flash

    #endif

    /*------------------------------------------------------------------*
     *                                                                  *
     *  Am I Intel/Micron based Flash?                                  *
     *                                                                  *
     *------------------------------------------------------------------*/
    #ifdef USE_INTEL_MICRON
        #ifdef USE_16_BIT
            *addr16 = FLASH_READ_ARRAY;         // Read Mode
            *addr16 = FLASH_READ_ID;            // Read ID Mode

            device_mfg      = *pmfgid;          // Read MFG_ID
            device_devid[0] = *pdevid1;         // Read DEV_ID
            device_devid[1] = 0;
            device_devid[2] = 0;

            *addr16 = FLASH_READ_ARRAY;
        #endif

        #ifdef USE_8_BIT
            *addr8 = FLASH_READ_ARRAY;          // Read Mode
            *addr8 = FLASH_READ_ID;             // Read ID Mode

            device_mfg      = *pmfgid;          // Read MFG_ID
            device_devid[0] = *pdevid1;         // Read DEV_ID
            device_devid[1] = 0;
            device_devid[2] = 0;

            *addr8 = FLASH_READ_ARRAY;
        #endif

        if ( device_mfg == MFG_INTEL )
            return MFG_INTEL;                   // Intel Flash
        if ( device_mfg == MFG_MICRON )
            return MFG_MICRON;                  // Micron Flash
    #endif

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_getMfg( )                                                        *
 *                                                                          *
 *      Who am I?   Intel/Micron based Flash?                               *
 *                  Spansion based Flash?                                   *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int32 _FLASH_getMfg( )
{
    if ( device_mfg != 0 )
        return device_mfg;
    else
        return _FLASH_getId( );
}





/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_getTotalPages( number_devices )                                  *
 *      Compute the total number of Flash pages                             *
 *                                                                          *
 *      number_devices  <- number of Flash device chips ( hardcoded to 1 )  *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Uint32 EVMDM6437_FLASH_getTotalPages( Uint16 number_devices )
{
    number_devices = number_devices;

    device_flash_pages = 0;
    device_flash_page_length = 0;
    device_flash_page_len_pow2 = 0;

    #ifdef USE_SPANSION
        /*------------------------------------------------------------------*
         *                                                                  *
         *  SPANSION Flash                                                  *
         *                                                                  *
         *------------------------------------------------------------------*/
        if ( _FLASH_getMfg( ) == MFG_SPANSION )
        {
            if ( ( device_devid[0] == DEV_AM29LV256M_1 )
              && ( device_devid[1] == DEV_AM29LV256M_2 )
              && ( device_devid[2] == DEV_AM29LV256M_3 ) )
            {
                device_flash_pages += 256;          // 256 sectors
                device_flash_page_length = 0x10000; // 64-KB sector size
                device_flash_page_len_pow2 = 16;    // Pow 2 size
            }
            else if ( ( device_devid[0] == DEV_S29GL256N_1 )
                   && ( device_devid[1] == DEV_S29GL256N_2 )
                   && ( device_devid[2] == DEV_S29GL256N_3 ) )
            {
                device_flash_pages += 256;          // 256 sectors
                device_flash_page_length = 0x10000; // 64-KB sector size
                device_flash_page_len_pow2 = 17;    // Pow 2 size
            }
            else if ( ( device_devid[0] == DEV_S29GL512N_1 )
                   && ( device_devid[1] == DEV_S29GL512N_2 )
                   && ( device_devid[2] == DEV_S29GL512N_3 ) )
            {
                device_flash_pages += 512;          // 512 sectors
                device_flash_page_length = 0x20000; // 128-KB sector size
                device_flash_page_len_pow2 = 17;    // Pow 2 size
            }
        }
    #endif

    #ifdef USE_INTEL_MICRON
        /*------------------------------------------------------------------*
         *                                                                  *
         *  Intel Flash                                                     *
         *                                                                  *
         *------------------------------------------------------------------*/
        if ( _FLASH_getMfg( ) == MFG_INTEL )
        {
            device_page_alignment = 0;
            device_flash_page_length = 0x20000;     // 128 Kbytes
            device_flash_page_len_pow2 = 17;

            switch ( device_devid[0] )
            {
                case 0x8801:
                    device_flash_pages += 64;
                    break;
                case 0x8802:
                    device_flash_pages += 128;
                    break;
                case 0x8803:
                    device_flash_pages += 256;
                    break;

                case DEV_INTEL_64_TOP:
                    device_flash_pages += DEV_INTEL_64_TOP_PAGES;
                    device_page_alignment = FLASH_DEVICE_TOP_PAGES;
                    break;
                case DEV_INTEL_128_TOP:
                    device_flash_pages += DEV_INTEL_128_TOP_PAGES;
                    device_page_alignment = FLASH_DEVICE_TOP_PAGES;
                    break;
                case DEV_INTEL_256_TOP:
                    device_flash_pages += DEV_INTEL_256_TOP_PAGES;
                    device_page_alignment = FLASH_DEVICE_TOP_PAGES;
                    break;

                case DEV_INTEL_64_BOT:
                    device_flash_pages += DEV_INTEL_64_BOT_PAGES;
                    device_page_alignment = FLASH_DEVICE_BOTTOM_PAGES;
                    break;
                case DEV_INTEL_128_BOT:
                    device_flash_pages += DEV_INTEL_128_BOT_PAGES;
                    device_page_alignment = FLASH_DEVICE_BOTTOM_PAGES;
                    break;
                case DEV_INTEL_256_BOT:
                    device_flash_pages += DEV_INTEL_256_BOT_PAGES;
                    device_page_alignment = FLASH_DEVICE_BOTTOM_PAGES;
                    break;
            }
        }

        /*------------------------------------------------------------------*
         *                                                                  *
         *  Micron Flash                                                    *
         *                                                                  *
         *------------------------------------------------------------------*/
        if ( _FLASH_getMfg( ) == MFG_MICRON )
        {
            switch ( device_devid[0] )
            {
                case 0x16:
                    device_flash_pages += 32;
                    break;
                case 0x17:
                    device_flash_pages += 64;
                    break;
                case 0x18:
                    device_flash_pages += 128;
                    break;
            }

            device_flash_page_length = 0x20000;     // 128 Kbytes
            device_flash_page_len_pow2 = 17;
        }
    #endif

    /* Return total pages of Flash */
    return device_flash_pages;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_getPageLength( )                                                 *
 *                                                                          *
 *      What is the page Length?                                            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int32 _FLASH_getPageLength( )
{
    return device_flash_page_length;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_nextPage( )                                                      *
 *                                                                          *
 *      Get Next Page of Flash.  This is only used b/c the Intel Flash being*
 *      contains 4 smaller pages in the TOP or BOTTOM page.  This breaks    *
 *      the assumption that each page is the same size, thus we need to     *
 *      compute what the next page is.                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Uint32 _FLASH_nextPage( Uint32 addr )
{
    Uint32 flash_offset;        // Offset in Flash
    Uint32 flash_major_page;    // Full size page
    #ifdef USE_INTEL_MICRON
        Uint32 flash_minor_page;// Quarter size page
    #endif

    /* Go to first page */
    if ( addr < FLASH_BASE )
        return FLASH_BASE;

    /* Last Page, no where left to go */
    if ( addr >= FLASH_END_OR_RANGE )
        return FLASH_END_OR_RANGE;

    /* ---------------------------------------------------------------- *
     *                                                                  *
     *  Spansion Flash                                                  *
     *                                                                  *
     * ---------------------------------------------------------------- */
    #ifdef USE_SPANSION
        if ( _FLASH_getMfg( ) == MFG_SPANSION )
        {
            /* Compute offset and page */
            flash_offset = addr - FLASH_BASE;
            flash_major_page = flash_offset >> device_flash_page_len_pow2;

            addr = FLASH_BASE;
            addr += ( flash_major_page + 1 ) << device_flash_page_len_pow2;

            return addr;
        }
    #endif

    /* ---------------------------------------------------------------- *
     *                                                                  *
     *  Intel/Micron Flash                                              *
     *                                                                  *
     * ---------------------------------------------------------------- */
    #ifdef USE_INTEL_MICRON
        if ( ( _FLASH_getMfg( ) == MFG_INTEL )
          || ( _FLASH_getMfg( ) == MFG_MICRON ) )
        {
            /* Compute offset and page */
            flash_offset = addr - FLASH_BASE;
            flash_major_page = flash_offset >> device_flash_page_len_pow2;
            flash_minor_page = flash_offset >> ( device_flash_page_len_pow2 - 2 );
            flash_minor_page &= 3;

            /* TOP side page alignment */
            if ( device_page_alignment == FLASH_DEVICE_TOP_PAGES )
            {
                if ( flash_major_page >= ( device_flash_pages - 1 ) )
                {
                    addr = FLASH_BASE;
                    addr += flash_major_page << device_flash_page_len_pow2;
                    addr += ( flash_minor_page + 1 ) << ( device_flash_page_len_pow2 - 2 );
                }
                else
                {
                    addr = FLASH_BASE;
                    addr += ( flash_major_page + 1 ) << device_flash_page_len_pow2;
                }
            }
            /* BOTTOM side page alignment */
            else if ( device_page_alignment == FLASH_DEVICE_BOTTOM_PAGES )
            {
                if ( flash_major_page < 1 )
                {
                    addr = FLASH_BASE;
                    addr += ( flash_minor_page + 1 ) << ( device_flash_page_len_pow2 - 2 );
                }
                else
                {
                    addr = FLASH_BASE;
                    addr += ( flash_major_page + 1 ) << device_flash_page_len_pow2;
                }
            }
            else
            {
                addr = FLASH_BASE;
                addr += ( flash_major_page + 1 ) << device_flash_page_len_pow2;
            }

            return addr;
        }
    #endif

    return 0xFFFFFFFF;
}
