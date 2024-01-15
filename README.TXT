// EPP-2_PROG - Linux EPP-2 EPROM Programmer Application
// Copyright (C) 2024 Jason Birch
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/****************************************************************************/
/* EPP-2_PROG - Linux EPP-2 EPROM Programmer Application.                   */
/* ------------------------------------------------------------------------ */
/* V1.01 - 2024-01-08 - Jason Birch                                         */
/* ------------------------------------------------------------------------ */
/* EPP-2_PROG EPP-2 Programmer command line application manual.             */
/****************************************************************************/



      INDEX

      1. FILES
         A description of the files included with this Open Source project.

      2. BINARY DATA MANAGEMENT
         Managing binary files to be programmed on EPROM devices.

      3. BINARY DATA TO MOTOROLA S RECORDS
         Converting binary files to Motorola S Record text files.

      4. EPP-2 PROGRAMMER STATUS
         Checking the status of the result of an EPP-2 Programmer operation.

      5. PROGRAMMING an EPROM OR EEPROM
         Programming Motorola S Record text files on an EPROM device.
         i)    Searching for device codes.
         ii)   Displaying a device code's information.
         iii)  Creating a custom device code.
         iv)   Checking a device is empty.
         v)    Writing a Motorola S Record file to an EPROM device.
         vi)   Verifying a device has been programmed correctly.
         vii)  Reading a Motorola S Record file from an EPROM device.
         viii) Alternate method for verifying the data written to a device.



1. FILES
========

README.TXT
This manual on how to use the programs included to program EPROM devices on
an EPP-2 EPROM programmer, from Linux.

GNU-GeneralPublicLicense.txt
This source code is released for use as laid out in the GNU license file
indicated here. And must be included with all distributions of the source
code and applications.

EPP-1F-2 E(E)PROM PROGRAMMER COMMAND MANUAL.txt
From the web site http://www.artbv.nl thank you for providing the
information that makes this Open Source software possible.

EPP-2_Device List.pdf
From the web site http://www.artbv.nl thank you for providing the
information that makes this Open Source software possible.

EPP-2_PROG.c
EPP-2_PROG.h
The source code for the EPP-2 EPROM Programmer to perform operations 
with EPROM & EEPROM devices.

EPP-2_PROG
The compiled EPP-2 Programmer application, used to perform operation on
& program EPROM & EEPROM devices. Execute ./Build.sh if not present.

EPP-2_PROG.CFG
Configuration parameters for the EPP-2_PROG application.

AddBinToROM.c
The source code for a utility to merge binary assets into a binary ROM
file to be programmed onto an EPROM or EEPROM device.

AddBinToROM
Compiled utility to merge binary assets into a binary ROM file to be
programmed onto an EPROM or EEPROM device. Execute ./Build.sh if not present.

BinToMotorola.c
The source code for a utility to convert a binary file into a text file
of a Motorola S Record format.

BinToMotorola
Compiled utility to convert a binary file into a text file of a
Motorola S Record format. Execute ./Build.sh if not present.

Build.sh
Shell script to compile the source code of this project.

Transpose
Transpose.c
Incidental utility used when organising data for this project. Included
for future reference. These files should be ignored.



2. BINARY DATA MANAGEMENT
=========================
ROM image files are typically found in binary form. A utility AddBinToROM is
included in this software to allow several binary files to be combined in a
single binary ROM image, to make it easier to program an EPROM.

e.g.
Start by removing any old version of the single ROM image file to be built:

rm ROM.BIN

Then add each binary asset to a new binary ROM image file, specifying the
start and end address range to place the binary asset at:

./AddBinToROM ROM.BIN 0000 3FFF ../../Emulator/RUNTIME/TAPEFILE/CollecoVision/GORF.ROM
./AddBinToROM ROM.BIN 4000 7FFF ../../Emulator/RUNTIME/TAPEFILE/CollecoVision/GYRUSS.ROM
./AddBinToROM ROM.BIN 8000 BFFF ../../Emulator/RUNTIME/TAPEFILE/CollecoVision/PACMAN.ROM
./AddBinToROM ROM.BIN C000 FFFF ../../Emulator/RUNTIME/TAPEFILE/CollecoVision/FROGGER.ROM

Use the Linux hexdump command to review the single binary ROM image file created:

hexdump -C ROM.BIN | more



3. BINARY DATA TO MOTOROLA S RECORDS
====================================
The EPP-2 Programmer writes Motorola S Record files to EPROM & EEPROM devices.
In order to write a binary ROM image file to an EPROM/EEPROM, it must be
converted to an S Record text file first. The utility BinToMotorola has been
included with this software to convert binary files to Motorola S Record files.

e.g.
./BinToMotorola [START_ADR] [MAX_ADR] [BIN_FILE]

Convert binary data file to a Motorola S-Record file by specifying the
[START_ADR], the address in the EPROM device to start writing to and
[MAX_ADR], the maximum address not to write past. The file created will have
the same name as the input binary file with .HEX appended to the file name:

./BinToMotorola 0000 FFFF ROM.BIN



4. EPP-2 PROGRAMMER STATUS
==========================
At the end of each operation, the application will display the EPP-2 Programmer
status, which is three hexadecimal values. The first value displays the error
code, if it is zero the operation was successful.

e.g.
>G

0000
0069EC0F
0000FFFF

The document "EPP-1F-2 E(E)PROM PROGRAMMER COMMAND MANUAL.txt" explains the
status codes in section "6.0 RESULT CODES".



5. PROGRAMMING an EPROM OR EEPROM
=================================

i) Searching for device codes
-----------------------------
As each EPROM/EEPROM device has it's own parameters required for programming.
Such as programming voltage, pulse time, ... When programming a device, a
device code needs to be provided, which defines the circumstances required to
program the specific device.

WARNING: As a device code defines the programming voltage, using an incorrect
device code to program a device, can damage an EPROM or EEPROM. Or can shorten
the life of an EPROM/EEPROM, for example if the incorrect pulse length is used.

There are a set of EPROM devices which have device codes already defined.
These can be found in the PDF file "EPP-2_Device List.pdf", but is also
included in the application and the list can be searched from the command line.

e.g.
./EPP-2_PROG D [Search Text]

The [Search Text] can be all or part of an EPROM device Name/Manufacturer/Code.

The command:
./EPP-2_PROG D 27C512

Will respond with the following devices:
CODE   : MANUFACTURER         DEVICE              
====== : ==================== ====================
222996 : AMD                  Am27C512            
222996 : AMD                  Am27C512L           
269896 : ATMEL                AT27C512            
269896 : FUJITSU              MBM27C512           
269896 : FUJITSU              MBM27C512P          
222996 : INTEL                27C512              
269896 : MICROCHIP            27C512              
222996 : NAT. SEMICONDUC.     NM27C512            
269896 : NEC                  27C512              
222996 : SGS-THOMSON          M27C512             
223A96 : TEXAS INSTRUMENTS    TMS27C512           



ii) Displaying a device code's information
------------------------------------------
The programming specification for a specific device code can be displayed.
This can be used to verify against a data sheet that a device code is
suitable for a specific device and will not cause damage.

e.g.
./EPP-2_PROG [S] [DEVICE CODE]

The [DEVICE CODE] must be the complete device code for a device as each
digit of the code provides specific information about the device.

The command:
./EPP-2_PROG S 222996

Will respond with the following programming specification:
DEVICE CODE   : 222996
EPROM Size    : 64 x 8 Kbit
Pin Config    : STD1
FF Skip       : FF SKIP
Vpp           : 12.75 VDC
Vcc           : 6.25 VDC
Margin Factor : 0x
Pulse Time    : 0.10 ms
Algorithm     : ALG2



iii) Creating a custom device code
----------------------------------
The document "EPP-1F-2 E(E)PROM PROGRAMMER COMMAND MANUAL.txt" describes how
to create a device code for a device if it is not included in the document
"EPP-2_Device List.pdf". See section "5.0 SELECTION CODE".

// A device code for a device included in the standard device list is built up
// as follows:
//
// DEVICE CODE FOR TMM27512AD-20
// 0           Always 0
// 0 1 0       ALG2 Program algorytum
// 0 0 1 0     0.1 ms pulse period
// 0 0         0x Margin factor
// 1 0         6.25 VDC Vcc
// 1 0 0 1     12.75 VDC Vpp
// 1           FF SKIP
// 0 0 1       STD1 Pin Config
// 0 1 1 0     64   X 8 KBIT
//
// DEVICE CODE = 222996
//

// A custom device code for a common EEPROM device is built up as follows:
//
// DEVICE CODE FOR W27C512
// 0           Always 0
// 0 1 0       ALG2 Program algorytum
// 0 0 0 1     0.05 ms pulse period
// 0 0         0x Margin factor
// 0 0         5.00 VDC Vcc
// 0 1 1 0     12.00 VDC Vpp
// 1           FF SKIP
// 0 0 1       STD1 Pin Config
// 0 1 1 0     64   X 8 KBIT
//
// DEVICE CODE = 210696
//



iv) Checking a device is empty
------------------------------
A device can be checked if it is empty with the command:
e.g.
./EPP-2_PROG [E] [DEVICE] <START_ADR> <END_ADR>

To check if a device is completely empty:
./EPP-2_PROG E 210696

To check if a device is empty in the address range 0x1000 to 0x2FFF:
./EPP-2_PROG E 210696 1000 2FFF



v) Writing a Motorola S Record file to an EPROM device
------------------------------------------------------
Write a Motorola S Record file to an EPROM or EEPROM. Specify the address
the data is to start writing to:
e.g.
./EPP-2_PROG [W] [DEVICE] [START_ADR] [MOTOROLA]

./EPP-2_PROG W 210696 0000 ROM.BIN.HEX



vi) Verifying a device has been programmed correctly
----------------------------------------------------
After writing to a device, the written data can be verified as written
correctly by using the same command line but replacing the W with a V for
verify:
e.g.
./EPP-2_PROG [V] [DEVICE] [START_ADR] [MOTOROLA]

./EPP-2_PROG V 210696 0000 ROM.BIN.HEX



vii Reading a Motorola S Record file from an EPROM device
---------------------------------------------------------
The content of an EPROM or EEPROM can be read back from the EPP-2 Programmer
directly into a Motorola S Record file, or to the computer display.
e.g.
./EPP-2_PROG [R] [DEVICE] <START_ADR> <END_ADR>

Read the complete content of an EPROM back into a file:
./EPP-2_PROG R 210696 > ROM.BIN.HEX.VFY

Read the data from the specified address range of an EPROM back to the display:
./EPP-2_PROG R 210696 0100 01FF

Read the data from the specified address range of an EPROM back into a file:
./EPP-2_PROG R 210696 4000 7FFF > ROM.BIN.HEX.VFY



viii Alternate method for verifying the data written to a device
----------------------------------------------------------------
An alternate method to verify a device is programmed correctly is to read the
content of the device back to a file and use the Linux diff command to display
any differences:

./EPP-2_PROG R 210696 > ROM.BIN.HEX.VFY
diff ROM.BIN.HEX ROM.BIN.HEX.VFY

The terminating line of the S Record file will show as different, but no other
lines should appear as different:

> R
2049c2050,2051
< S7050000FFFFFC
---
> S70500010000F9
> 

