#include "srec.h"
#include "UART0.h"
#include <stdio.h>

#define ELEMENT_NUM     4
#define ELEMENT_SIZE    1024

uint8_t SREC_AsciiToHex(char character)
{
    uint8_t reVal = 0xFF;
    if(character >= '0' && character <='9')
    {
        reVal = character - 0x30;
    }
    else if(character >= 'A' && character <= 'F')
    {
        reVal = character - 'A' + 10;
    }
    return reVal;
}

uint8_t SREC_check(char *srecLine,SREC_parseData_t *parseData)
{
    uint8_t index = 0,srecType = 0,dataTpm = 0,offset = 4,count = 0;
    uint8_t numberOfAddressByte = 0,checksum = 0, bytecount = 0;
    uint8_t  status;

    srecType = SREC_AsciiToHex(srecLine[1]);
    switch(srecType)
    {
    case 0:
            status = parseStatus_Start;
    case 1:
    case 2:
    case 3:
        numberOfAddressByte = srecType + 1;
        status = parseStatus_Data;
        break;
    case 5:
    case 6:
        status = parsesStatus_Unsupported;
        break;
    case 7:
    case 8:
    case 9:
        numberOfAddressByte = 11 - srecType;
        status = parseStatus_Done;
        break;
    default:
        status = parseStatus_Error;
        break;
    }

//get byte count 
    bytecount = (SREC_AsciiToHex(srecLine[2]) << 4) | SREC_AsciiToHex(srecLine[3]);
    checksum = bytecount;

    count = 0;
    while(srecLine[count] != '\r')
    {
        count++;
    }
    count = count-4;
    if((bytecount << 1) != count )
    {
        status = parseStatus_Error;
    }
    parseData->length = bytecount - numberOfAddressByte - 1;

    if(status != parseStatus_Error)
    {
//get address
        parseData->address = 0;
        for (index = 0; index < numberOfAddressByte; index++)
        {
            dataTpm = (SREC_AsciiToHex(srecLine[offset]) << 4) | SREC_AsciiToHex(srecLine[offset + 1]);
            parseData->address = (parseData->address << 8) | dataTpm;
            checksum += dataTpm;
            offset += 2;
        }

//get Data
        for (index = 0; index < (bytecount - numberOfAddressByte - 1); index++)
        {
            dataTpm = (SREC_AsciiToHex(srecLine[offset]) << 4) | SREC_AsciiToHex(srecLine[offset + 1]);
            parseData->data[index] = dataTpm;
            checksum += dataTpm;
            offset += 2;
        }

//get checkSum
        checksum += (SREC_AsciiToHex(srecLine[offset]) << 4) | SREC_AsciiToHex(srecLine[offset + 1]);
        if (0xFF != (checksum & 0xFF))
        {
            status = parseStatus_Error;
        }
    }
    return status;
}