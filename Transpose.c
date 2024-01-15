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
/* Transpose - Convert a vertical list into horizontal data.                */
/* ------------------------------------------------------------------------ */
/* V1.01 - 2024-01-08 - Jason Birch                                         */
/* ------------------------------------------------------------------------ */
/* Utility program used to transpose data from vertical to horizontal when  */
/* converting device informtaion from the PDF file into a header file.      */
/* This is an incidental program not intended for use, but left with the    */
/* source code, in case it is required in future.                           */
/****************************************************************************/


#include <stdio.h>
#include <string.h>


#define BUFF_SIZE             255


void main()
{
   FILE* File = NULL;
   unsigned short Count;
   unsigned short DataCount;
   unsigned short Index;
   unsigned char Buffer[BUFF_SIZE+1];
   unsigned char Data[512][3][BUFF_SIZE+1];
   
   if ((File = fopen("DEVICE.DAT", "rt")))
   {
      Index = 0;
      DataCount = 0;
      while(fgets(Buffer, BUFF_SIZE, File))
      {
         while (Buffer[0] != '\0' && (Buffer[strlen(Buffer)-1] == '\r' || Buffer[strlen(Buffer)-1] == '\n'))
         {
            Buffer[strlen(Buffer)-1] = '\0';
         };

         if (Buffer[0] == '\0')
         {
            DataCount = 0;
            ++Index;
         }
         else
         {
            for (Count = 0; Count < strlen(Buffer); ++Count)
               if (Buffer[Count] == '\\')
                  Buffer[Count] = '/';
//            printf("%u %s\r\n", Index, Buffer);
            strcpy(Data[DataCount][Index], Buffer);
            ++DataCount;
         }
      };
      fclose(File);
      
      printf("#ifndef __EPP_2_PROG_H\r\n");
      printf("#define __EPP_2_PROG_H\r\n\r\n");
      printf("const unsigned char* Devices[][3] =\r\n");
      printf("{\r\n");
      for (Count = 0; Count < DataCount; ++Count)
      {
         printf("  { ");
         for (Index = 0; Index < 3; ++Index)
            printf(" \"%s\",", Data[Count][Index]);
         printf("},\r\n");
      }
      printf("   \"\\0\", \"\\0\", \"\\0\",\r\n");
      printf("};\r\n\r\n");
      printf("#endif\r\n");
   }
}
