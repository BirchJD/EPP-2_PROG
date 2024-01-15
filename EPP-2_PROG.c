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
/* Program EPROMs or EEPROMs via serial communication on the EPP-2 EPROM    */
/* programmer. The application is used in a command line window on Linux    */
/* distributions. It can be used to search for device codes to be used for  */
/* specific EPROM devices. Custom device codes can be used too, also        */
/* supporting EEPROM devices. Devices can be checked as being empty, read,  */
/* written or verified in a specified address range.                        */
/****************************************************************************/


#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include "EPP-2_PROG.h"



int main(int argc, char* argv[])
{
   FILE* File;
   short Count;
   short TryCount;
   short Result;
   int DeviceCode;
   int SerialPort;
   char Buffer[BUFF_SIZE + 1];
   struct termios tty;
   ConfigType Config;

  /*******************************************/
 /* Check for valid command line arguments. */
/*******************************************/
   if (argc < 3 || argc > ARG_COUNT
      || !strchr("DSERWV", argv[ARG_OPERATION][0])
      || (argv[ARG_OPERATION][0] == 'D' && argc < 2)
      || (argv[ARG_OPERATION][0] == 'S' && argc != 3)
      || ((argv[ARG_OPERATION][0] == 'W' || argv[ARG_OPERATION][0] == 'V') && argc != 5))
   {
      fprintf(stderr, "\r\n");
      fprintf(stderr, "EPP-2 EPROM Programmer Linux Application V1.01 (C)2024-01-08 Jason Birch\r\n\r\n");
      fprintf(stderr, "%s [D|S|E|R|W|V] [DEVICE] <START_ADR> <END_ADR>\r\n", argv[ARG_EXE]);
      fprintf(stderr, "%s [D|S|E|R|W|V] [DEVICE] [START_ADR] [MOTOROLA_FILE]\r\n", argv[ARG_EXE]);
      fprintf(stderr, "\r\n");
      fprintf(stderr, "WHERE:\r\n");
      fprintf(stderr, "[D] <NAME>                          - EPROM Device/Manufacturer name search.\r\n");
      fprintf(stderr, "[S] [DEVICE]                        - EPROM Device programming specification.\r\n");      
      fprintf(stderr, "[E] [DEVICE] <START_ADR> <END_ADR>  - Empty check address range.\r\n");
      fprintf(stderr, "[R] [DEVICE] <START_ADR> <END_ADR>  - Read data in address range.\r\n");
      fprintf(stderr, "[W] [DEVICE] [START_ADR] [MOTOROLA] - Write data in address range.\r\n");
      fprintf(stderr, "[V] [DEVICE] [START_ADR] [MOTOROLA] - Verify data in address range.\r\n");
      fprintf(stderr, "\r\n");
   }
   else
   {
  /**********************************/
 /* Read configuration paramaters. */
/**********************************/
      strcpy(Config.SerialPort, "/dev/ttyUSB0");
      strcpy(Config.BaudRate, "19200");
      if (!(File = fopen("EPP-2_PROG.CFG", "rt")))
         fprintf(stderr, "USING DEFAULT CONFIG VALUES, FAILED TO OPEN CONFIG FILE FOR READING: EPP-2_PROG.CFG\r\n");
      else
      {
         while (fgets(Buffer, BUFF_SIZE, File))
         {
            while (Buffer[0] != '\0' && (Buffer[strlen(Buffer)-1] == '\r' || Buffer[strlen(Buffer)-1] == '\n'))
            {
               Buffer[strlen(Buffer)-1] = '\0';
            };
            if (!strncmp(Buffer, "SERIAL_PORT=", 12))
               strcpy(Config.SerialPort, &(Buffer[12]));
            else if (!strncmp(Buffer, "BAUD_RATE=", 10))
               strcpy(Config.BaudRate, &(Buffer[10]));
         };
         fclose(File);
      }
      fprintf(stderr, "\r\nCONFIGURATION\r\n");
      fprintf(stderr, "=============\r\n");
      fprintf(stderr, "SERIAL PORT: %s\r\n", Config.SerialPort);
      fprintf(stderr, "BAUD RATE: %s\r\n", Config.BaudRate);

  /******************************************/
 /* EPROM Device/Manufacturer name search. */
/******************************************/
      if (argv[ARG_OPERATION][0] == 'D' && argc < 4)
      {
         fprintf(stderr, "\r\n%-6s : %-20s %-20s\r\n", "CODE", "MANUFACTURER", "DEVICE");
         fprintf(stderr, "====== : ==================== ====================\r\n");
         // Search performed in all upper case characters.
         for (Count = 0; Count < strlen(argv[ARG_DEVICE]); ++Count)
            Buffer[Count] = toupper(argv[ARG_DEVICE][Count]);
         Buffer[Count] = '\0';
         Count = 0;
         while (Devices[Count][0][0] != '\0')
         {
            if (argv[ARG_DEVICE] == NULL || strstr(Devices[Count][0], Buffer) || strstr(Devices[Count][1], Buffer) || strstr(Devices[Count][2], Buffer))
               fprintf(stderr, "%-6s : %-20s %-20s\r\n", Devices[Count][2], Devices[Count][0], Devices[Count][1]);
            ++Count;
         };
         fprintf(stderr, "\r\n");
      }
  /*******************************************/
 /* EPROM Device Programming Specification. */
/*******************************************/
      else if (argv[ARG_OPERATION][0] == 'S' && argc == 3)
      {
         sscanf(argv[ARG_DEVICE], "%X", &DeviceCode);
         fprintf(stderr, "\r\nDEVICE CODE   : %6.6X\r\n", DeviceCode);
         fprintf(stderr, "EPROM Size    : %s\r\n", EPROM_Size[(DeviceCode >> 0) & 0x0F]);
         fprintf(stderr, "Pin Config    : %s\r\n", PinConfig[(DeviceCode >> 4) & 0x07]);
         fprintf(stderr, "FF Skip       : %s\r\n", FF_Skip[(DeviceCode >> 7) & 0x01]);
         fprintf(stderr, "Vpp           : %s\r\n", Vpp[(DeviceCode >> 8) & 0x0F]);
         fprintf(stderr, "Vcc           : %s\r\n", Vpp[(DeviceCode >> 12) & 0x03]);
         fprintf(stderr, "Margin Factor : %s\r\n", MarginFactor[(DeviceCode >> 14) & 0x03]);
         fprintf(stderr, "Pulse Time    : %s\r\n", PulseTime[(DeviceCode >> 16) & 0x0F]);
         fprintf(stderr, "Algorithm     : %s\r\n", Algorithms[(DeviceCode >> 20) & 0x03]);
         fprintf(stderr, "\r\n");
      }
  /**********************************/
 /* Open Linux serial port device. */
/**********************************/
      else if (!(SerialPort = open(Config.SerialPort, O_RDWR)))
         fprintf(stderr, "Failed to open serial port: %s\n", Config.SerialPort);
      else
      {
  /*****************************************/
 /* Read Linux serial port configuration. */
/*****************************************/
         if (tcgetattr(SerialPort, &tty))
            fprintf(stderr, "Failed to get communication paramaters: %s\n", Config.SerialPort);
         else
         {
  /********************************/
 /* Configure Linux serial port. */
/********************************/
            // Disable parity bit.
            tty.c_cflag &= ~PARENB;
            // One stop bit.
            tty.c_cflag &= ~CSTOPB;
            // Eight data bits.
            tty.c_cflag |= CS8;
            // Enable hardware handshaking.
            tty.c_cflag |= CRTSCTS;
            // Disable modem controls.
            tty.c_cflag |= CREAD | CLOCAL;

            // Enable sending each character, not just at a carrage return.
            tty.c_lflag &= ~ICANON;
            // Disable echo.
            tty.c_lflag &= ~(ECHO | ECHOE | ECHONL);
            // Disable signal characters.
            tty.c_lflag &= ~ISIG;

            // Disable software flow control.
            tty.c_iflag &= ~(IXON | IXOFF | IXANY);
            // Disable special bytes.
            tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
            tty.c_oflag &= ~(OPOST | ONLCR);
            
            // Set timeout.
            tty.c_cc[VTIME] = 0;
            tty.c_cc[VMIN] = 0;

            // Set local baud rate.
            SelectBaudRate(&tty, Config.BaudRate);

  /******************************************/
 /* Write Linux serial port configuration. */
/******************************************/
            if (tcsetattr(SerialPort, TCSANOW, &tty))
               fprintf(stderr, "Failed to set communication paramaters: %s\n", Config.SerialPort);

   /**********************************************/
  /* Check communication, configure remote baud */
 /* rate if remote command prompt not present. */
/**********************************************/
            do
            {
               fprintf(stderr, "\r\nCHECK FOR COMMAND PROMT\r\n");
               fprintf(stderr, "=======================\r\n");
               // Check for remote command prompt.
               sprintf(Buffer, "%c\r", 0x1B);
               SendData(FALSE, SerialPort, Buffer);
               sleep(1);
               Result = ReceiveData(FALSE, SerialPort, Buffer, 1024, stderr);
               if (Result != PROMPT)
               {
  /************************************************************/
 /* Set local baud rate to EPP-2 default power on baud rate. */
/************************************************************/
                  fprintf(stderr, "\r\nSET EPP-2 BAUD: %s\r\n", Config.BaudRate);
                  fprintf(stderr, "=====================\r\n");
  /***************************/
 /* Find current baud rate. */
/***************************/
                  Count = 0;
                  do
                  {
                     // Set local baud rate to default for EPP-2, 9600.
                     SelectBaudRate(&tty, BaudRates[Count]);
                     if (tcsetattr(SerialPort, TCSANOW, &tty))
                        fprintf(stderr, "Failed to set communication paramaters: %s\n", Config.SerialPort);
  /******************************************************/
 /* Send cancel current command, ESC [0x1B], to EPP-2. */
/******************************************************/
                     TryCount = 0;
                     do
                     {
                        // Clear send buffer.
                        sprintf(Buffer, "%c\r", 0x1B);
                        SendData(TRUE, SerialPort, Buffer);
                        // Clear receive buffer.
                        Result = ReceiveData(TRUE, SerialPort, Buffer, 128, stderr);
                     } while (Result != PROMPT && ++TryCount < 4);
                     if (Result == PROMPT)
                     {
                        fprintf(stderr, "CURRENT BAUD RATE: %s\r\n", BaudRates[Count]);
                        break;
                     }
                  } while (BaudRates[++Count]);
                  // Set remote baud rate to configuration baud rate.
                  if (!strcmp(Config.BaudRate, "300"))
                     SendData(FALSE, SerialPort, "6X\r");
                  else if (!strcmp(Config.BaudRate, "600"))
                     SendData(FALSE, SerialPort, "5X\r");
                  else if (!strcmp(Config.BaudRate, "1200"))
                     SendData(FALSE, SerialPort, "4X\r");
                  else if (!strcmp(Config.BaudRate, "2400"))
                     SendData(FALSE, SerialPort, "3X\r");
                  else if (!strcmp(Config.BaudRate, "4800"))
                     SendData(FALSE, SerialPort, "2X\r");
                  else if (!strcmp(Config.BaudRate, "9600"))
                     SendData(FALSE, SerialPort, "1X\r");
                  else if (!strcmp(Config.BaudRate, "19200"))
                     SendData(FALSE, SerialPort, "0X\r");
                  else
                     fprintf(stderr, "INVALID BAUD RATE FOR EPP-2 PROGRAMMER: %s\r\n", Config.BaudRate);
                  sleep(1);
  /***************************************************/
 /* Set local baud rate to configuration baud rate. */
/***************************************************/
                  // Set local baud rate.
                  SelectBaudRate(&tty, Config.BaudRate);
                  if (tcsetattr(SerialPort, TCSANOW, &tty))
                     fprintf(stderr, "Failed to set communication paramaters: %s\n", Config.SerialPort);
  /********************************************************/
 /* Send return to EPP-2 and check for a command prompt. */
/********************************************************/
                  WaitForPrompt(SerialPort);
               }
            } while (Result != PROMPT);

  /**************************************************/
 /* Configure the EPP-2 for the device to be used. */
/**************************************************/
            fprintf(stderr, "\r\nSET DEVICE CODE: %s\r\n", argv[ARG_DEVICE]);
            fprintf(stderr, "================\r\n");
            sprintf(Buffer, "%sS\r", argv[ARG_DEVICE]);
            SendData(FALSE, SerialPort, Buffer);
            if (ReceiveData(FALSE, SerialPort, Buffer, 1024, stderr) == TRUE)
               goto EPP_2_ERROR;

            if (argc > ARG_START_ADR)
            {
               fprintf(stderr, "\r\nSET START ADDRESS\r\n");
               fprintf(stderr, "=================\r\n");
               sprintf(Buffer, "%sP\r", argv[ARG_START_ADR]);
               SendData(FALSE, SerialPort, Buffer);
               if (ReceiveData(FALSE, SerialPort, Buffer, 1024, stderr) == TRUE)
                  goto EPP_2_ERROR;

               fprintf(stderr, "\r\nSET OFFSET ADDRESS\r\n");
               fprintf(stderr, "==================\r\n");
               sprintf(Buffer, "%sO\r", argv[ARG_START_ADR]);
               SendData(FALSE, SerialPort, Buffer);
               if (ReceiveData(FALSE, SerialPort, Buffer, 1024, stderr) == TRUE)
                  goto EPP_2_ERROR;
            }
            else
            {
               SendData(FALSE, SerialPort, "0000O\r");
               if (ReceiveData(FALSE, SerialPort, Buffer, 1024, stderr) == TRUE)
                  goto EPP_2_ERROR;
            }

            if (!strchr("WV", argv[ARG_OPERATION][0]))
            {
               if (argc > ARG_END_ADR)
               {
                  fprintf(stderr, "\r\nSET END ADDRESS\r\n");
                  fprintf(stderr, "===============\r\n");
                  sprintf(Buffer, "%sL\r", argv[ARG_END_ADR]);
                  SendData(FALSE, SerialPort, Buffer);
                  if (ReceiveData(FALSE, SerialPort, Buffer, 1024, stderr) == TRUE)
                     goto EPP_2_ERROR;
               }
            }

            fprintf(stderr, "\r\nGET ADDRESS RANGE\r\n");
            fprintf(stderr, "=================\r\n");
            SendData(FALSE, SerialPort, "SPLO\r");
            if (ReceiveData(FALSE, SerialPort, Buffer, 1024, stderr) == TRUE)
               goto EPP_2_ERROR;

  /****************************************************************/
 /* Perform an empty check of the device over the address range. */
/****************************************************************/
            if (argv[ARG_OPERATION][0] == 'E')
            {
               fprintf(stderr, "\r\nEMPTY CHECK\n");
               fprintf(stderr, "===========\n");
               SendData(FALSE, SerialPort, "T\r");
               WaitForPrompt(SerialPort);
            }
  /*****************************************************/
 /* Read data from the device over the address range. */
/*****************************************************/
            else if (argv[ARG_OPERATION][0] == 'R')
            {
               fprintf(stderr, "\r\nREAD DATA\n");
               fprintf(stderr, "=========\n");
               SendData(FALSE, SerialPort, "R\r");
               ReceiveData(FALSE, SerialPort, Buffer, 1024, stdout);
            }
  /***************************************************/
 /* Write the Motorola S-Record file to the device. */
/***************************************************/
            else if (argv[ARG_OPERATION][0] == 'W')
            {
               fprintf(stderr, "\r\nWRITE DATA\r\n");
               fprintf(stderr, "==========\r\n");
               SendData(FALSE, SerialPort, "W\r");
               Result = ReceiveData(FALSE, SerialPort, Buffer, 1024, stderr);
               if (!(File = fopen(argv[ARG_DATA_FILE], "rt")))
                  fprintf(stderr, "Failed to open Motorola S-Record file: %s\r\n", argv[ARG_DATA_FILE]);
               else
               {
                  Result = FALSE;
                  while (!feof(File) && !Result)
                  {
                     Buffer[0] = '\0';
                     fgets(Buffer, BUFF_SIZE, File);
                     if (Buffer[0] == 'S')
                     {
                        SendData(FALSE, SerialPort, Buffer);
                        Result = ReceiveData(FALSE, SerialPort, Buffer, 8, stderr);
                        if (Buffer[0] != '\0')
                           break;
                     }
                  };
                  fclose(File);
                  WaitForPrompt(SerialPort);
               }
            }
  /*********************************************************/
 /* Verify the Motorola S-Record file to the device data. */
/*********************************************************/
            else if (argv[ARG_OPERATION][0] == 'V')
            {
               fprintf(stderr, "\r\nVERIFY DATA\r\n");
               fprintf(stderr, "===========\r\n");
               SendData(FALSE, SerialPort, "V\r");
               Result = ReceiveData(FALSE, SerialPort, Buffer, 1024, stderr);
               if (!(File = fopen(argv[ARG_DATA_FILE], "rt")))
                  fprintf(stderr, "Failed to open Motorola S-Record file: %s\r\n", argv[ARG_DATA_FILE]);
               else
               {
                  Result = FALSE;
                  Buffer[0] = '\0';
                  while (!feof(File) && !Result)
                  {
                     Buffer[0] = '\0';
                     fgets(Buffer, BUFF_SIZE, File);
                     if (Buffer[0] == 'S')
                     {
                        SendData(FALSE, SerialPort, Buffer);
                        Result = ReceiveData(FALSE, SerialPort, Buffer, 8, stderr);
                        if (Buffer[0] != '\0')
                           break;
                     }
                  };
                  fclose(File);
                  WaitForPrompt(SerialPort);
               }
            }
            else
               fprintf(stderr, "UNKNOWN OPERATION: %s\r\n", argv[ARG_OPERATION][0]);

  /*********************************************************/
 /* Display the EPP-2 status at the end of the operation. */
/*********************************************************/
            fprintf(stderr, "\r\nEEP-2 STATUS\n");
            fprintf(stderr, "============\n");
            SendData(FALSE, SerialPort, "G\r");
            ReceiveData(FALSE, SerialPort, Buffer, 1024, stderr);
         }

EPP_2_ERROR:
         close(SerialPort);
      }
   }
}



/**********************************************************************/
/* Wait for a command prompt to be available on the EPP-2 Programmer. */
/**********************************************************************/
void WaitForPrompt(int SerialPort)
{
   short Result = FALSE;
   short TryCount = 200;
   char Buffer[BUFF_SIZE + 1];

   SendData(TRUE, SerialPort, "\r");
   sleep(1);
   do
   {
      Result = ReceiveData(TRUE, SerialPort, Buffer, 1024, stderr);
   } while (Result != PROMPT && --TryCount);
   sleep(1);
   Result = ReceiveData(TRUE, SerialPort, Buffer, 1024, stderr);
   if (!TryCount)
      fprintf(stderr, "WARNING: DIDN'T FIND COMMAND PROMPT\r\n");
}



/**************************************************************/
/* Configure the local serial port on Linux for the specified */
/* baud rate, provided as a string value.                     */
/**************************************************************/
void SelectBaudRate(struct termios* tty, unsigned char* BaudRate)
{
  /******************************************************/
 /* Select a vaid baud rate for the Linux serial port. */
/******************************************************/
   if (!strcmp(BaudRate, "300"))
   {
      cfsetispeed(tty, B300);
      cfsetospeed(tty, B300);
   }
   else if (!strcmp(BaudRate, "600"))
   {
      cfsetispeed(tty, B600);
      cfsetospeed(tty, B600);
   }
   else if (!strcmp(BaudRate, "1200"))
   {
      cfsetispeed(tty, B1200);
      cfsetospeed(tty, B1200);
   }
   else if (!strcmp(BaudRate, "2400"))
   {
      cfsetispeed(tty, B2400);
      cfsetospeed(tty, B2400);
   }
   else if (!strcmp(BaudRate, "4800"))
   {
      cfsetispeed(tty, B4800);
      cfsetospeed(tty, B4800);
   }
   else if (!strcmp(BaudRate, "9600"))
   {
      cfsetispeed(tty, B9600);
      cfsetospeed(tty, B9600);
   }
   else if (!strcmp(BaudRate, "19200"))
   {
      cfsetispeed(tty, B19200);
      cfsetospeed(tty, B19200);
   }
   else
   {
      cfsetispeed(tty, B9600);
      cfsetospeed(tty, B9600);
      fprintf(stderr, "INVALID BAUD RATE FOR EPP-2 PROGRAMMER: %s\r\n", BaudRate);
   }
}



unsigned char* ChrReplace(unsigned char* Data, unsigned char Find, unsigned char Replace)
{
   unsigned short Count;
   
   for (Count = 0; Count < strlen(Data); ++Count)
      if (Data[Count] == Find)
         Data[Count] = Replace;

   return Data;
}



/***********************************************************/
/* Send data to the EPP-2 Programmer, via the serial port. */
/***********************************************************/
void SendData(unsigned char Silent, int SerialPort, char* Data)
{
   write(SerialPort, Data, strlen(Data));
   if (!Silent)
      fprintf(stderr, ">%s\n", ChrReplace(Data, 0x1B, '~'));
}



/****************************************************************/
/* Receive data from the EPP-2 Programmer, via the serial port. */
/****************************************************************/
short ReceiveData(unsigned char Silent, int SerialPort, char* Data, int TimeOut, FILE* OutStream)
{
   short Result = FALSE;
   unsigned char FirstLine = TRUE;
   unsigned int ByteCount = 0;
   int TimeOutCount;
   int Bytes;
   char Buffer[BUFF_SIZE + 1];
   struct timespec Sleep = { 0, 1000000};
   
   Data[0] = '\0';
   TimeOutCount = 0;
   do
   {
      if ((Bytes = read(SerialPort, Buffer, BUFF_SIZE)))
      {
         TimeOutCount = 0;
         ByteCount += Bytes;
         Buffer[Bytes] = '\0';
         if (Buffer[Bytes - 1] == '*')
         {
            Result = PROMPT;
            break;
         }
         if (strcmp(Buffer, "Error\n"))
            Result = TRUE;
         if (FirstLine && strchr(Buffer, '\r'))
         {
            FirstLine = FALSE;
            strcpy(Data, &(strchr(Buffer, '\r')[1]));
         }
         else
            strcpy(Data, Buffer);
         if (OutStream == stdout)
         {
            if (!Silent)
               fprintf(stderr, "%u Bytes Received\r", ByteCount);
            fprintf(OutStream, Data);
         }
         else if (!Silent)
            fprintf(OutStream, ChrReplace(Data, 0x1B, '~'));
      }
      if (TimeOut)
         nanosleep(&Sleep, NULL);
   } while (++TimeOutCount < TimeOut);
   if (!Silent && ByteCount)
      fprintf(OutStream, "\n");

   return Result;
}
