call ..\..\..\..\DosRun.bat

echo off
rem This file is called from top-level build.bat to build bsl examples
rem This script can be run stand-alone if you initialize the 'TIROOT', 'TCONF'
rem and TIMAKE variables.
echo on

set TIROOT=%TI_DIR%
set TCONF=%TIROOT%\bin\utilities\tconf\tconf
set TIMAKE=%TIROOT%\cc\bin\timake

%TIMAKE% evmdm6437bsl.pjt Debug

call cleanup.bat
