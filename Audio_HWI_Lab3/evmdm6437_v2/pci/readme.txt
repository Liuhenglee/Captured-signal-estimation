PCI Host Driver (Version 0.90 Beta)

This directory contains a Windows PCI host driver that can be used
with your EVM.  You can find the most recent version of the drivers
and documentation at:

   http://support.spectrumdigital.com

The directory structure is as follows:

pci
   drivers         <-- Windows PCI Host Driver Files
   examples
      include      <-- Include files for writing your own PCI host app
      lib          <-- Library files for writing your own PCI host app
      pci_host     <-- A sample PCI host app (MS Visual C++ 6.0)
      pci_slave    <-- A sample PCI slave app (CCStudio V3.2)
      test         <-- Binary executables and a PCI test

--------------------
    PCI Usage
--------------------

1)  Configure your board for PCI boot mode.

    SW2-1:  Off
    SW2-2:  Off
    SW2-3:  Off
    SW2-4:  On

    SW1-1:  On (Boot Mode = 0x1)
    SW1-2:  Off
    SW1-3:  Off
    SW1-4:  Off
    SW1-5:  On (FastBoot = 1)

2)  Turn your PC off.

3)  Insert the EVM into an available PCI slot.

4)  Boot your PC.

5)  After Windows loads, the Add New Hardware Wizard will open automatically
    and prompt for the location of the drivers.  Instruct the wizard to load
    drivers from pci\drivers in the directory structure above.


--------------------
    PCI Test
--------------------

To check if the PCI driver has been loaded correctly, please run:

    pci\examples\test\pci_host.exe

It is a command line application that will do the following:

    1) Test the first 4Kbytes of the DSP's internal RAM

    2) Load pci_slave.out from the same directory into the DSP's RAM

    3) Set the DSPBOOTADDR register and BOOTCMPLT.BC bit to tell the ROM
       bootloader code.  The pci_slave code will then execute and 
       write the magic number 0xABCD1234 at the beginning of the DSP's RAM.

    4) pci_host will wait a few seconds and then check to make sure the
       magic number written by pci_slave is present.  This consitutes a 
       passed test.

All of the source code for pci_host and pci_slave exists in their respective
directories.  These projects also serve as a starting point for your own code.

**** NOTE - The DSP's bootload process is only designed once.  After the
            BOOTCOMPLT.BC bit has been set and the code starts running,
            there is no mechanism for the host to assume control of the
            DSP's code execution again.  The pci_host app checks if the
            BOOTCOMPLT.BC bit and will alert you if it has already been
            set.  Please reboot your PC to reset the DSP and start the
            process over again.


--------------------
    PCI Loader
--------------------

The pci_host application also serves as a simple DSP program loader.  If you
run the pci_host application with the name of a .out file as an argument
like this:

    pci_host pci_slave.out

The .out file will be loaded and executed using the same procedure followed
by the PCI test above.


--------------------
    Notes
--------------------

This driver can handle interrupts as well, but this functionality is not
demonstrated in the current version of the code.


--------------------
    Driver API
--------------------

The PCI Driver API is described below.  The driver uses PCI region 2 for the
register access functions and assumes that the default mapping of region 2 to
0x01C00000 always exists.  The driver uses PCI region 4 for memory accesses.
It maps region 4 to the target address and assumes that the driver can remap
PCI region 4 at will.  If you require other mappings, please use regions other
than 2 and 4.

For core usage examples, please see:

    examples\pci_host

An API description follows:


/*F***************************************************************************
* NAME:  PCI64_Open()
*
* DESCRIPTION:  Open access to DM642 EVM at BoardNum
*
* PARMETERS:
*   BoardNum       - Board number 0-3
*   pHndl          - Pointer to return handle
*     
* NOTES: The return handle can be valid even if PCI64_Open fails.  In this 
*        case you should immediately call PCI64_Close.
*
* RETURN VALUE:
*    0 Success, non-0 for Fail
*   
*F***************************************************************************/
PCI64_API INT32 PCI64_Open( int BoardNum,               
                                                    PCI64_HANDLE * pHndl );     

/*F***************************************************************************
* NAME:  PCI64_Close()
*
* DESCRIPTION:  Close access to DM642 EVM.  Should always close
*   
* PARMETERS:
*   Hndl           - Handle returned from PCI64_Open
*
* NOTES: Always call PCI64_Close() even if PCI64_Open fails.
*
* RETURN VALUE:
*    0 Success, non-0 for Fail
*
*F***************************************************************************/
PCI64_API INT32 PCI64_Close( PCI64_HANDLE Hndl );
 

/*F***************************************************************************
* NAME:  PCI64_MemWrite32()
*
* DESCRIPTION:  Write Count 32 bit values to Taddr.
*         
* PARMETERS:
*   Hndl           - Handle returned from PCI64_Open
*   Taddr          - C64x target address  
*   Count          - Number of 32 bit data values
*   pData          - Pointer to array of 32 bit data types  
*
* NOTES: Taddr must be aligned to 32 bit address boundary.  
*        Count must be less then 8M bytes or the basic 64xx PCI memory 
*          window.
*        Taddr+Count must not cross a 8M byte PCI page boundary.
*
*        Function maps PCI region 4 to the base of the address space
*        specified by Taddr and then performs the write operation.
*
* RETURN VALUE:
*    0 Success, non-0 for Fail
*
*F***************************************************************************/

PCI64_API INT32 PCI64_MemWrite32( PCI64_HANDLE Hndl,  
                                  UINT32   Taddr,     
                                  UINT32   Count,      
                                  UINT32 * pData );


/*F***************************************************************************
* NAME:  PCI64_MemFill32()
*
* DESCRIPTION:  Fill Count 32 bit values to Taddr.
*
* PARMETERS:
*   Hndl           - Handle returned from PCI64_Open
*   Taddr          - C64x target address  
*   Count          - Number of 32 bit data values
*   pData          - Pointer to array of 32 bit data types  
*         
* NOTES: Taddr must be aligned to 32 bit address boundary.  
*        Count must be less then 8M bytes or the basic 64xx PCI memory 
*          window.
*        Taddr+Count must not cross a 8M byte PCI page boundary.
*
*        Function maps PCI region 4 to the base of the address space
*        specified by Taddr and then performs the memfill operation.
*
* RETURN VALUE:
*    0 Success, non-0 for Fail
*
*F***************************************************************************/

PCI64_API INT32 PCI64_MemFill32( PCI64_HANDLE Hndl,  UINT32   Taddr,
                                                     UINT32   Count,
                                                     UINT32 * pData );
 

/*F***************************************************************************
* NAME:  PCI64_MemRead32()
*
* DESCRIPTION:  Read Count 32 bit values from Taddr.
*   
* PARMETERS:
*   Hndl           - Handle returned from PCI64_Open
*   Taddr          - C64x target address  
*   Count          - Number of 32 bit data values
*   pData          - Pointer to array of 32 bit data types  
*      
* NOTES: Taddr must be aligned to 32 bit address boundary.  
*        Count must be less then 8M bytes or the basic 64xx PCI memory 
*          window.
*        Taddr+Count must not cross a 8M byte PCI page boundary.
*
*        Function maps PCI region 4 to the base of the address space
*        specified by Taddr and then performs the read operation.
*
* RETURN VALUE:
*    0 Success, non-0 for Fail
*
*F***************************************************************************/

PCI64_API INT32 PCI64_MemRead32( PCI64_HANDLE Hndl,  UINT32   Taddr,
                                                     UINT32   Count,
                                                     UINT32 * pData );


/*F***************************************************************************
* NAME:  PCI64_RegWrite32()
*
* DESCRIPTION:  Write Count 32 bit values to Taddr.
*
* PARMETERS:
*   Hndl           - Handle returned from PCI64_Open
*   Taddr          - C64x target address  
*   Count          - Number of 32 bit data values
*   pData          - Pointer to array of 32 bit data types  
*         
* NOTES: Taddr must be aligned to 32 bit address boundary.  
*        Count must be less then 1024.
*        
*        Function uses the PCI region 2 default address mapping at DSP
*        address 0x01C00000 with a range of 4MBytes.  It can only address
*        registers in that range.
*
* RETURN VALUE:
*    0 Success, non-0 for Fail
*
*F***************************************************************************/
PCI64_API INT32 PCI64_RegWrite32( PCI64_HANDLE Hndl, UINT32   Taddr,
                                                     UINT32   Count,
                                                     UINT32 * pData );


/*F***************************************************************************
* NAME:  PCI64_RegRead32()
*
* DESCRIPTION:  Read Count 32 bit values from Taddr.
* 
* PARMETERS:
*   Hndl           - Handle returned from PCI64_Open
*   Taddr          - C64x target address  
*   Count          - Number of 32 bit data values
*   pData          - Pointer to array of 32 bit data types  
*        
* NOTES: Taddr must be aligned to 32 bit address boundary.  
*        Count must be less then 1024.
*        
*        Function uses the PCI region 2 default address mapping at DSP
*        address 0x01C00000 with a range of 4MBytes.  It can only address
*        registers in that range.
*
* RETURN VALUE:
*    0 Success, non-0 for Fail
*
*F***************************************************************************/
PCI64_API INT32 PCI64_RegRead32( PCI64_HANDLE Hndl,  UINT32   Taddr,
                                                     UINT32   Count,
                                                     UINT32 * pData );


/*F***************************************************************************
* NAME:  PCI64_LoadCoff()
*
* DESCRIPTION:  Load Coff image to PCI target board
*    
* PARMETERS:
*   Hndl           - Handle returned from PCI64_Open
*   pCoffName      - Pointer to coff name string 
*     
* NOTES: The following restrictions apply:
*        -C64xx is held in reset.
*        -C64xx EMIF has been setup if loading off chip
*        -Non-relocatable coff file.
*        -All COFF sections must begin on a 32 bit boundary.  Be aware this
*          may require link command file changes to ensure alignment. 
*          If a COFF ends on a non-32 bit boundary then the remaining
*          1-3 bytes will be filled with zero(s). 
*        -DSPBOOTADDR will be set according to the COFF entry point
*
* RETURN VALUE:
*    0 Success, non-0 for Fail
*
*F***************************************************************************/
PCI64_API INT32 PCI64_LoadCoff( PCI64_HANDLE hPci, char * pCoffName);

 
/*F***************************************************************************
* NAME:  PCI64_GetCallbackEvent()
*
* DESCRIPTION:  Get the handle used for callback events
*    
* PARMETERS:
*   Hndl  - Handle returned from PCI64_Open
*
* NOTES: This call is only valid after a successfull PCI64_Open call.
*
* RETURN VALUE:
*     HANDLE for Success; NULL for Fail
*
*F***************************************************************************/
PCI64_API HANDLE PCI64_GetCallbackEvent( PCI64_HANDLE hPci );


/*F***************************************************************************
* NAME:  PCI64_SetupCallbackEvent()
*
* DESCRIPTION:  Setup a callback function to respond to PCI interrupts
*    
* PARMETERS:
*   Hndl                  - Handle returned from PCI64_Open
*   callbackFunction      - Pointer to callback function of type
*                             DWORD WINAPI Callback(LPVOID dummy)
*
* NOTES:  This function creates a thread to run the Callback function.
*
*
* RETURN VALUE:
*    0 Success, non-0 for Fail
*
*F***************************************************************************/
PCI64_API INT32 
PCI64_SetupCallbackEvent(PCI64_HANDLE Hndl, void *callbackFunction);


