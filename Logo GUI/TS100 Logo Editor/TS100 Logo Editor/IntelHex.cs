using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;

namespace IntelHex
{
    public class IntelHex
    {      
      public static string encode(byte[] data,uint32 startingAddress,int bytesPerLine)
      {
            //We are taking the byte array and encoding it into the wanted hex file contents (aka string of data).
            //First create the starting base address record
            //:020000040800F2
            string Output;
            //Set the upper 16 bits of the address space
          Output+=getHighAddressSetLine(startingAddress>>16);
            //Now loop through all the data that we were given
            for(int index=0;index<data.Length;index+=bytesPerLine)
            {
                //We want to read from data[index] to data[index+bytesPerLine-1]
                uint32 currentAddress = startingAddress+index;
                if((currentAddress>>16)!=(startingAddress>>16))
                {
                    Output+= getHighAddressSetLine(currentAddress>>16);
                    //The address has rolled over the 64K boundry, so write a new high address change line
                }
                //We should now be good for the higher part of the address
                Output+=encodeDataLine(currentAddress&0xFFFF,data,index,bytesPerLine);
            }
            //We have now written out all the data lines
            Output+= ":00000001FF\r\n";//End of file marker

            return Output;
      }
      private static string encodeDataLine(uint16 address,byte[] data,uint32 startindex,int bytes)
      {
            string line = ":";
            line += bytes.ToString("X");//add the marker of line length
            line += address.ToString("X");//write the address
            line += "00";//Data line
            //Next copy bytes bytes 
            for(int i=0;i<bytes;i++)
                line+= data[startindex+i].ToString("X");
            line +=checksumLine(line);//Adds checksum and EOL
            return line;
      }
      private static string getHighAddressSetLine(uint16 HighAddress)
      {
        string Output;
        Output += ":02000004"+(HighAddress).ToString("X");
        Output += checksumLine(Output);
        return Output;
      }
      private static string checksumLine(string line)
      {
          //We want to convert the string line to each byte and sum. 
          byte sum =0;
          for(int index =1;i<line.Length;i+=2)
          {
            byte b = byte.fromString(line.subString(i,2));
            sum+=b;
          }
          byte checksum = 0-sum;//invert
          return checksum.toString("X")+"\r\n";
      }
    }
}
