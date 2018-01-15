/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Flash interface for Intel/Micron Flash chips & Spansion  Flash chips
 *
 */

#ifndef FLASH_
#define FLASH_

#include "evmdm6437.h"

#define USE_SPANSION
//#define USE_INTEL_MICRON

#define USE_8_BIT
//#define USE_16_BIT

/* ------------------------------------------------------------------------ *
 *  Spansino Flash Devices                                                  *
 * ------------------------------------------------------------------------ */
#define MFG_SPANSION                0x01

#ifdef USE_16_BIT
    #define DEV_AM29LV256M_1        0x227E
    #define DEV_AM29LV256M_2        0x2212
    #define DEV_AM29LV256M_3        0x2201
    #define DEV_AM29LV256M_SIZE     0x02000000

    #define DEV_S29GL512N_1         0x227E
    #define DEV_S29GL512N_2         0x2223
    #define DEV_S29GL512N_3         0x2201
    #define DEV_S29GL512N_SIZE      0x04000000

    #define DEV_S29GL256N_1         0x227E
    #define DEV_S29GL256N_2         0x2222
    #define DEV_S29GL256N_3         0x2201
    #define DEV_S29GL256N_SIZE      0x02000000

    #define DEV_S29GL128N_1         0x227E
    #define DEV_S29GL128N_2         0x2221
    #define DEV_S29GL128N_3         0x2201
    #define DEV_S29GL128N_SIZE      0x01000000
#endif

#ifdef USE_8_BIT
    #define DEV_AM29LV256M_1        0x7E
    #define DEV_AM29LV256M_2        0x12
    #define DEV_AM29LV256M_3        0x01
    #define DEV_AM29LV256M_SIZE     0x02000000

    #define DEV_S29GL512N_1         0x7E
    #define DEV_S29GL512N_2         0x23
    #define DEV_S29GL512N_3         0x01
    #define DEV_S29GL512N_SIZE      0x04000000

    #define DEV_S29GL256N_1         0x7E
    #define DEV_S29GL256N_2         0x22
    #define DEV_S29GL256N_3         0x01
    #define DEV_S29GL256N_SIZE      0x02000000

    #define DEV_S29GL128N_1         0x7E
    #define DEV_S29GL128N_2         0x21
    #define DEV_S29GL128N_3         0x01
    #define DEV_S29GL128N_SIZE      0x01000000
#endif

/* ------------------------------------------------------------------------ *
 *  Intel/Mircon Flash Devices                                              *
 * ------------------------------------------------------------------------ */
#define MFG_INTEL                   0x89

#ifdef USE_16_BIT
    #define DEV_INTEL_64_TOP        0x8817
    #define DEV_INTEL_64_TOP_PAGES  64  // 63@128K + 4@32K
    #define DEV_INTEL_64_TOP_SIZE   0x00800000

    #define DEV_INTEL_64_BOT        0x881A
    #define DEV_INTEL_64_BOT_PAGES  64  // 63@128K + 4@32K
    #define DEV_INTEL_64_BOT_SIZE   0x00800000


    #define DEV_INTEL_128_TOP       0x8818
    #define DEV_INTEL_128_TOP_PAGES 128 // 127@128K + 4@32K
    #define DEV_INTEL_128_TOP_SIZE  0x01000000

    #define DEV_INTEL_128_BOT       0x881B
    #define DEV_INTEL_128_BOT_PAGES 128 // 127@128K + 4@32K
    #define DEV_INTEL_128_BOT_SIZE  0x01000000


    #define DEV_INTEL_256_TOP       0x8919
    #define DEV_INTEL_256_TOP_PAGES 256 // 255@128K + 4@32K
    #define DEV_INTEL_256_TOP_SIZE  0x02000000

    #define DEV_INTEL_256_BOT       0x891C
    #define DEV_INTEL_256_BOT_PAGES 256 // 255@128K + 4@32K
    #define DEV_INTEL_256_BOT_SIZE  0x02000000
#endif

#ifdef USE_8_BIT
    #define DEV_INTEL_64_TOP        0x17
    #define DEV_INTEL_64_TOP_PAGES  64  // 63@128K + 4@32K
    #define DEV_INTEL_64_TOP_SIZE   0x00800000

    #define DEV_INTEL_64_BOT        0x1A
    #define DEV_INTEL_64_BOT_PAGES  64  // 63@128K + 4@32K
    #define DEV_INTEL_64_BOT_SIZE   0x00800000


    #define DEV_INTEL_128_TOP       0x18
    #define DEV_INTEL_128_TOP_PAGES 128 // 127@128K + 4@32K
    #define DEV_INTEL_128_TOP_SIZE  0x01000000

    #define DEV_INTEL_128_BOT       0x1B
    #define DEV_INTEL_128_BOT_PAGES 128 // 127@128K + 4@32K
    #define DEV_INTEL_128_BOT_SIZE  0x01000000


    #define DEV_INTEL_256_TOP       0x19
    #define DEV_INTEL_256_TOP_PAGES 256 // 255@128K + 4@32K
    #define DEV_INTEL_256_TOP_SIZE  0x02000000

    #define DEV_INTEL_256_BOT       0x1C
    #define DEV_INTEL_256_BOT_PAGES 256 // 255@128K + 4@32K
    #define DEV_INTEL_256_BOT_SIZE  0x02000000
#endif

#define MFG_MICRON                  0x2C

/* ------------------------------------------------------------------------ *
 *  Flash Memory Property Definitions                                       *
 * ------------------------------------------------------------------------ */
#define FLASH_BASE                  ( EMIF_CS2_BASE )
#define FLASH_END_OR_RANGE          ( EMIF_CS3_BASE )
#define FLASH_PAGES                 FLASH_getTotalPages( 1 )
#define FLASH_SUPPORT               1

/* Intel only */
#define FLASH_DEVICE_TOP_PAGES      1
#define FLASH_DEVICE_BOTTOM_PAGES   2

/* ------------------------------------------------------------------------ *
 *  Flash Memory Base Pointers                                              *
 * ------------------------------------------------------------------------ */
#ifdef USE_16_BIT
    #define FLASH_BASE_PTR32        *( volatile Uint32* )FLASH_BASE
    #define FLASH_BASE_PTR16        *( volatile Uint16* )FLASH_BASE
    #define FLASH_BASE_PTR8         *( volatile Uint8* ) FLASH_BASE
#endif
#ifdef USE_8_BIT
    #define FLASH_BASE_PTR8         *( volatile Uint8* ) FLASH_BASE
#endif

/* Spansion only */
#ifdef USE_16_BIT
    #define FLASH_CTL555            *( volatile Uint16* )( FLASH_BASE + 0xAAA )
    #define FLASH_CTL2AA            *( volatile Uint16* )( FLASH_BASE + 0x554 )
#endif
#ifdef USE_8_BIT
    #define FLASH_CTL555            *( volatile Uint8* )( FLASH_BASE + 0x555 )
    #define FLASH_CTLAAA            *( volatile Uint8* )( FLASH_BASE + 0xAAA )
#endif

/* ------------------------------------------------------------------------ *
 *  Flash Command Codes ( SPANSION )                                        *
 * ------------------------------------------------------------------------ */
#define FLASH_RESET                         0xF0
#define FLASH_CMD_AA                        0xAA
#define FLASH_CMD_55                        0x55
#define FLASH_ID                            0x90
#define FLASH_PROGRAM                       0xA0
#define FLASH_WRITE_BUFFER                  0x25
#define FLASH_PROGRAM_BUFFER                0x29
#define FLASH_ERASE                         0x80
#define FLASH_ERASE_CHIP                    0x10
#define FLASH_ERASE_SECTOR                  0x30
#define FLASH_ERASE_SUSPEND                 0xB0
#define FLASH_ERASE_RESUME                  0x10
#define FLASH_CFI_QUERY                     0x98

/* ------------------------------------------------------------------------ *
 *  Flash Command Codes ( Intel/Micron )                                    *
 * ------------------------------------------------------------------------ */
#define FLASH_READ_ARRAY                    0xFF
#define FLASH_READ_ID                       0x90
#define FLASH_READ_QUERY                    0x98
#define FLASH_READ_STATUS                   0x70
#define FLASH_CLEAR_STATUS                  0x50
#define FLASH_WORD_PROGRAM                  0x40 // -or- 0x10
#define FLASH_WRITE_TO_BUFFER               0xE8
#define FLASH_BUFFERED_EFP                  0x80
#define FLASH_BLOCK_ERASE                   0x20
#define FLASH_ERASE_PRG_SUSPEND             0xB0
#define FLASH_ERASE_PRG_RESUME              0xD0
#define FLASH_CONFIGURATION                 0xB8
#define FLASH_CONFIG_BLOCK_LOCK_BITS        0x60
#define FLASH_SET_LOCK_BITS                 0x01
#define FLASH_CLEAR_LOCK_BITS               0xD0
#define FLASH_PROTECTION_PROGRAM            0xC0

/* ------------------------------------------------------------------------ *
 *  Flash Memory Read Status Register Fields                                *
 * ------------------------------------------------------------------------ */
#define READ_STATUS_REGISTER_ISMS           0x80 // WRITE STATE MACHINE
#define READ_STATUS_REGISTER_ESS            0x40 // ERASE SUSPEND
#define READ_STATUS_REGISTER_ECLBS          0x20 // ERASE & CLEAR LOCK BITS
#define READ_STATUS_REGISTER_PSLBS          0x10 // PROGRAM & SET LOCK BIT
#define READ_STATUS_REGISTER_VPENS          0x08 // PROGRAMMING VOLTAGE
#define READ_STATUS_REGISTER_PSS            0x04 // PROGRAM SUSPEND
#define READ_STATUS_REGISTER_DPS            0x02 // DEVICE PROTECT

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16  EVMDM6437_FLASH_init         ( );
Int16  EVMDM6437_FLASH_read         ( Uint32 src, Uint32 dst, Uint32 length );
Int16  EVMDM6437_FLASH_write        ( Uint32 src, Uint32 dst, Uint32 length );
Int16  EVMDM6437_FLASH_erase        ( Uint32 start, Uint32 length );
Uint32 EVMDM6437_FLASH_checksum     ( Uint32 start, Uint32 length );
Uint32 EVMDM6437_FLASH_getTotalPages( Uint16 num_flash_device );

/* Internal functions */
Int32  _FLASH_getMfg                ( );
Int32  _FLASH_getPageLength         ( );
Uint32 _FLASH_nextPage              ( Uint32 addr );

/* Spansion */
Int16 _FLASH_waitWhileErasing       ( Uint32 addr, Int32 timeout );
#ifdef USE_8_BIT
    Int16 _FLASH_waitWhileProgramming( Uint32 addr, Uint8 data, Int32 timeout );
#endif
#ifdef  USE_16_BIT
    Int16 _FLASH_waitWhileProgramming( Uint32 addr, Uint16 data, Int32 timeout );
#endif

/* Intel/Micron */
Int16 _FLASH_waitWhileBusy          ( Int32 timeout );
Int16 _FLASH_unlockBlocks           ( Uint32 start, Uint32 length );

#define FLASH_PAGESIZE              ( _FLASH_getPageLength( ) )

#endif
