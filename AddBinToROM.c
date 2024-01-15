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
/* AddBinToROM - Merge a binary file into a larger binary file @ a location */
/* ------------------------------------------------------------------------ */
/* V1.01 - 2024-01-08 - Jason Birch                                         */
/* ------------------------------------------------------------------------ */
/* Add a binary file to a ROM image file at the specified address. Utility  */
/* used to accumulate binary assets into a ROM file to be programmed in one */
/* go using the EPP-2 Programmer. This utility produces a binary file, use  */
/* the utility BinToMotorola to convert the binary ROM file into a Motorola */
/* S record text file which can be sent to the EPP-2 Programmer using the   */
/* EPP-2_PROG command line application.                                     */
/****************************************************************************/


#include <stdio.h>
#include <string.h>


#define ARG_COUNT             5
#define ARG_EXE               0
#define ARG_ROM_FILE          1
#define ARG_START_ADR         2
#define ARG_END_ADR           3
#define ARG_BIN_FILE          4

#define BUFF_SIZE             255
#define ROM_SIZE              0x10000



int main(int argc, char* argv[])
{
   FILE* File;
   short Result;
   unsigned short Count;
   unsigned short BinCount;
   unsigned short StartAddress;
   unsigned short EndAddress;
   unsigned short BinFileSize;
   unsigned int ScanValue;
   char ROM[ROM_SIZE];
   char BinFile[ROM_SIZE];
   char Buffer[BUFF_SIZE + 1];
   
   if (argc < ARG_COUNT)
   {
      printf("\n%s [ROM_FILE] [START_ADR] [END_ADR] [BIN_FILE]\n", argv[ARG_EXE]);
      printf("WHERE:\n");
      printf("[ROM_FILE]  - ROM File to add binary data to or new ROM file.\n");
      printf("[START_ADR] - Start address to add data.\n");
      printf("[END_ADR]   - End address to add data & pad with 0xFF.\n");
      printf("[BIN_FILE]  - Binary file to add to ROM file.\n");
      printf("\n");
   }
   else
   {
  /*******************************/
 /* Get command line arguments. */
/*******************************/
      sscanf(argv[ARG_START_ADR], "%X", &ScanValue);
      StartAddress = ScanValue;
      sscanf(argv[ARG_END_ADR], "%X", &ScanValue);
      EndAddress = ScanValue;
      printf("ADDING DATA TO RANGE: %4.4X - %4.4X\r\n", StartAddress, EndAddress);

  /************************************************/
 /* Read in binary file to be added to ROM file. */
/************************************************/
      if (!(File = fopen(argv[ARG_BIN_FILE], "rb")))
         printf("Failed to read BIN file: %s\r\n", argv[ARG_BIN_FILE]);
      else
      {
         BinFileSize = fread(BinFile, 1, ROM_SIZE, File);
         fclose(File);
   /************************************************/
  /* Read in current contents of binary ROM file. */
 /* Or make a new ROM file full of 0xFF values.  */
/************************************************/
         if (!(File = fopen(argv[ARG_ROM_FILE], "rb")))
            memset(ROM, 0xFF, ROM_SIZE);
         else
         {
            fread(ROM, ROM_SIZE, 1, File);
            fclose(File);
         }
  /******************************************/
 /*Add new binary data to binary ROM file. */
/******************************************/
         BinCount = 0;
         for (Count = StartAddress; Count <= EndAddress && BinCount < BinFileSize; ++Count)
         {
            ROM[Count] = BinFile[BinCount++];
            if (Count == 0xFFFF)
               break;
         }
  /**********************************************************************/
 /* Pad with 0xFF values if new binary data did not reach end address. */
/**********************************************************************/
         if (Count != 0xFFFF)
         {
            for (Count = Count; Count <= EndAddress; ++Count)
            {
               ROM[Count] = 0xFF;
               if (Count == 0xFFFF)
                  break;
            }
         }
  /***********************************************/
 /* Write updated binary ROM file back to disk. */
/***********************************************/
         if (!(File = fopen(argv[ARG_ROM_FILE], "wb")))
            printf("Failed to write ROM file: %s\r\n", argv[ARG_ROM_FILE]);
         else
         {
            fwrite(ROM, ROM_SIZE, 1, File);
            fclose(File);
         }
      }
   }
}
