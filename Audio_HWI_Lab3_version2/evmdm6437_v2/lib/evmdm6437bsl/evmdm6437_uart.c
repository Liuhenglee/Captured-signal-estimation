/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  UART implementation
 *
 */

#include "evmdm6437_uart.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _UART_open( id, baudrate )                                              *
 *                                                                          *
 *      Open UART handle                                                    *
 *                                                                          *
 * ------------------------------------------------------------------------ */
UART_Handle EVMDM6437_UART_open( Uint16 id, Uint32 baudrate )
{
    UART_Handle uart_handle;
    Uint32 divisor;

    /*
     *  UART clk / ( baudrate * 16 )
     *  = 27,000,000 / ( 115200 * 16 )
     *  = 14
     */
    divisor = 27000000 / ( baudrate * 16 );

    switch ( id )
    {
        case 0:
            uart_handle = ( UART_Handle )&UART_MODULE_0;
            break;
        case 1:
            uart_handle = ( UART_Handle )&UART_MODULE_1;
            break;
        default:
            return ( UART_Handle )-1;
    }

    uart_handle->regs->PWREMU_MGMT = 0;     // Reset UART TX & RX components

    _wait( 100 );

    uart_handle->regs->DLL = ( divisor & 0xff );    // Set baud rate
    uart_handle->regs->DLH = ( divisor >> 8 ) & 0xff;

    uart_handle->regs->FCR = 0x0007;        // Clear UART TX & RX FIFOs
    uart_handle->regs->FCR = 0x0001;        // Enable TX & RX FIFOs 1-byte
    uart_handle->regs->IER = 0x0007;        // Enable interrupts
    uart_handle->regs->LCR = 0x0003;        // 8-bit words,
                                            // 1 STOP bit generated,
                                            // No Parity, No Stick paritiy,
                                            // No Break control
    uart_handle->regs->MCR = 0x0000;        // RTS & CTS disabled,
                                            // Loopback mode disabled,
                                            // Autoflow disabled

    uart_handle->regs->PWREMU_MGMT = 0x6001; // Emulation Free,
                                             // Enable TX & RX componenets

    if ( ( uart_handle->regs->IIR & 0xC0 ) == 0xC0 )// Check FIFOs are Enabled
        return uart_handle;
    else
        return ( UART_Handle )-1;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _UART_close( uart_handle )                                              *
 *                                                                          *
 *      Close UART handle                                                   *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_UART_close( UART_Handle uart_handle  )
{
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _UART_xmtReady( uart_handle )                                           *
 *                                                                          *
 *      Check if Transmitter is ready to send                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_UART_xmtReady( UART_Handle uart_handle )
{
    Uint8 iir_status = ( uart_handle->regs->IIR >> 1 ) & 0x0E;

    if ( iir_status == 0 )                  // Check for No Interrupts
        return 0;
    else if ( iir_status == 1 )             // Check for Current TX emptying
        return 1;
    else                                    // Everything else
        return 2;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _UART_putChar( uart_handle, data )                                      *
 *                                                                          *
 *      Send 1 byte of serial data                                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_UART_putChar( UART_Handle uart_handle, Uint8 data )
{
    uart_handle->regs->THR = data;
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _UART_rcvReady( uart_handle )                                           *
 *                                                                          *
 *      Check if Receiver is ready to read                                  *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_UART_rcvReady( UART_Handle uart_handle )
{
    Uint8 iir_status = ( uart_handle->regs->IIR >> 1 ) & 0x0E;

    if ( iir_status == 2 )                  // Check for Data Ready
        return 0;
    else if ( iir_status == 0 )             // Check for No Interrupts
        return 1;
    else                                    // Everything else
        return 2;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _UART_getChar( uart_handle, data )                                      *
 *                                                                          *
 *      Recv 1 byte of serial data                                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVMDM6437_UART_getChar( UART_Handle uart_handle, Uint8* data )
{
    *data = uart_handle->regs->THR;
    return 0;
}
