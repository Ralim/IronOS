using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;

namespace IntelHex
{
    public class IntelHex
    {
        public static string encode(byte[] data, UInt32 startingAddress, int bytesPerLine, bool header = true,bool Addbloat=false)
        {
            //We are taking the byte array and encoding it into the wanted hex file contents (aka string of data).
            //First create the starting base address record
            //:020000040800F2
            string Output = "";
            if (header)
            {
                //Set the upper 16 bits of the address space
                Output += getHighAddressSetLine((UInt16)(startingAddress >> 16));
            }

            //Now loop through all the data that we were given
            for (UInt32 index = 0; index < data.Length; index = (UInt32)(index + bytesPerLine))
            {
                //We want to read from data[index] to data[index+bytesPerLine-1]
                UInt32 currentAddress = (UInt32)(startingAddress + index);
                if ((currentAddress >> 16) != (startingAddress >> 16))
                {
                    Output += getHighAddressSetLine((UInt16)(currentAddress >> 16));
                    //The address has rolled over the 64K boundry, so write a new high address change line
                }
                //We should now be good for the higher part of the address
                Output += encodeDataLine((UInt16)(currentAddress & 0xFFFF), data, index, bytesPerLine);
            }
            if(Addbloat)
            {
                //Repeat the instructs a stack of times to get around filesize minimums
                for(int x=0;x<3;x++)
                {
                    for (UInt32 index = 0; index < data.Length; index = (UInt32)(index + bytesPerLine))
                    {
                        //We want to read from data[index] to data[index+bytesPerLine-1]
                        UInt32 currentAddress = (UInt32)(startingAddress + index);
                        if ((currentAddress >> 16) != (startingAddress >> 16))
                        {
                            Output += getHighAddressSetLine((UInt16)(currentAddress >> 16));
                            //The address has rolled over the 64K boundry, so write a new high address change line
                        }
                        //We should now be good for the higher part of the address
                        Output += encodeDataLine((UInt16)(currentAddress & 0xFFFF), data, index, bytesPerLine);
                    }
                }
            }
            //We have now written out all the data lines
            Output += ":00000001FF\r\n";//End of file marker

            return Output;
        }
        private static string encodeDataLine(UInt16 address, byte[] data, UInt32 startindex, int bytes)
        {
            string line = ":";
            line += bytes.ToString("X2");//add the marker of line length
            line += address.ToString("X4");//write the address
            line += "00";//Data line
            //Next copy bytes bytes 
            for (int i = 0; i < bytes; i++)
            {
                if ((startindex + i) < data.Length)
                    line += data[startindex + i].ToString("X2");
                else
                    line += "FF";//pad images out with FF
            }
            line += checksumLine(line);//Adds checksum and EOL
            return line;
        }
        private static string getHighAddressSetLine(UInt16 HighAddress)
        {
            string Output = "";
            Output += ":02000004" + (HighAddress).ToString("X4");
            Output += checksumLine(Output);
            return Output;
        }
        private static string checksumLine(string line)
        {
            //We want to convert the string line to each byte and sum. 
            byte sum = 0;
            for (int i = 1; i < line.Length; i += 2)
            {
                byte b = byte.Parse((line.Substring(i, 2)), System.Globalization.NumberStyles.HexNumber);
                sum += b;
            }
            byte checksum = (byte)(((byte)0) - sum);//invert
            return checksum.ToString("X2") + "\r\n";
        }
    }
}
