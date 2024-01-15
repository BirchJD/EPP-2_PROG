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
/* BinToMotorola - Convert a binary file into Motorola S record text file.  */
/* ------------------------------------------------------------------------ */
/* V1.01 - 2024-01-08 - Jason Birch                                         */
/* ------------------------------------------------------------------------ */
/* Convert a binary file into a Motorola S record text file. Which can then */
/* be sent to an EPP-2 EPROM Programmer, to program a ROM device. Specify   */
/* start address for the binary data. Specify the start address the data in */
/* the file is to start at. Also specify the max address that can be used,  */
/* in order to keep within the target EPROM device memory range.            */
/* The utility AddBinToROM can be used to create a ROM image with binary    */
/* resources at their required address location. This can also be used to   */
/* locate data to be added to a pre-programmed EPROM at an unprogrammed     */
/* location.                                                                */
/****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ARG_COUNT          4
#define ARG_EXE            0
#define ARG_START_ADR      1
#define ARG_MAX_ADR        2
#define ARG_HEX_FILE       3

#define BUFF_SIZE          255


int main(int argc, char* argv[])
{
   FILE* File;
   FILE* OutFile;
   unsigned char Data;
   unsigned char CheckSum;
   unsigned int Count;
   unsigned long Address = 0x0000;
   unsigned long MaxAddress = 0xFFFF;
   char Hex[BUFF_SIZE + 1];
   char Buffer[BUFF_SIZE + 1];

   if (argc != ARG_COUNT)
      printf("\n%s [START_ADR] [MAX_ADR] [BIN_FILE]\n\n", argv[ARG_EXE]);
   else
   {
  /*******************************/
 /* Get command line arguments. */
/*******************************/
      sscanf(argv[ARG_START_ADR], "%X", &Address);
      sscanf(argv[ARG_MAX_ADR], "%X", &MaxAddress);
  /**********************************************************************/
 /* Open binary file to be converted to a Motorola S record text file. */
/**********************************************************************/
      if (!(File = fopen(argv[ARG_HEX_FILE], "rb")))
         printf("Failed to open file for reading: %s\n", argv[ARG_HEX_FILE]);
      else
      {
   /***********************************************************************/
  /* Open a file of the same name as the source file with .HEX appended, */
 /* for the Motorola S record target file.                              */
/***********************************************************************/
         strcpy(Buffer, argv[ARG_HEX_FILE]);
         strcat(Buffer, ".HEX");
         if (!(OutFile = fopen(Buffer, "wb")))
            printf("Failed to open file for writing: %s\n", Buffer);
         else
         {
   /**********************************************************/
  /* Convert the data from the source binary file until the */
 /* end of file, or the target end address is reached.     */
/**********************************************************/
            while (!feof(File) && Address < MaxAddress && Address <= 0xFFFFFFFF)
            {
   /***************************************************************/
  /* Each line of a Motorola S recored file is 32 bytes of data, */
 /* and accumulate a checksum for the line of data.             */
/***************************************************************/
               CheckSum = 0;
               Buffer[0] = '\0';
               for (Count = 0; Count < 32 && !feof(File); ++Count)
               {
                  Data = fgetc(File);
                  CheckSum += Data;
                  sprintf(Hex, "%2.2X", (Data & 0xFF));
                  strcat(Buffer, Hex);
               }
  /****************************************************************************/
 /* Add the address of the line of data to the checksum of the line of data. */
/****************************************************************************/
               CheckSum += 5 + Count;
               CheckSum += (Address & 0x00FF);
               CheckSum += ((Address & 0xFF00) >> 8);
               CheckSum += ((Address & 0xFF00) >> 16);
               CheckSum += ((Address & 0xFF00) >> 24);
  /************************************************************************/
 /* Write the S record for the line of data to the Motorola target file. */
/************************************************************************/
               fprintf(OutFile, "S3%2.2X%8.8X%s%2.2X\r\n", ((5 + Count) & 0xFF), (Address & 0xFFFFFFFF), Buffer, (~CheckSum & 0xFF));
  /****************************************************************/
 /* Calculate the address of the start of the next line of data. */
/****************************************************************/
               Address += Count;
            };
   /************************************************/
  /* Calculate the final line of the Montorola S  */
 /*  record file, to terminate the data records. */
/************************************************/
            --Address;
            CheckSum = 5;
            CheckSum += (Address & 0x00FF);
            CheckSum += ((Address & 0xFF00) >> 8);
            CheckSum += ((Address & 0xFF00) >> 16);
            CheckSum += ((Address & 0xFF00) >> 24);
            fprintf(OutFile, "S705%8.8X%2.2X\r\n", (Address & 0xFFFFFFFF), (~CheckSum & 0xFF));
            fclose(OutFile);
         }
         fclose(File);
      }
   }
}
