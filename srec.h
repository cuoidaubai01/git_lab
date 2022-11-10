#ifndef SREC_H
#define SREC_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
DEFINE STATUS OF CHECK SREC AND DEFINE STRUCT TO SAVE THE DATA RECEIVED FROM SREC
*******************************************************************************/
enum SREC_status_t
{
    parseStatus_Start = 0,
    parseStatus_Data = 1,
    parseStatus_Done = 2,
    parsesStatus_Unsupported = 3,
    parseStatus_Error = 4,
};

typedef struct 
{
    uint32_t address;
    uint8_t data[1024];
    uint8_t length;
}SREC_parseData_t;

/******************************************************************************
                                    API                                         
*******************************************************************************/

/******************************************************************************
Object: Convert Ascii to Hex
Input : character : type char need convert
Output: value type Hex, if value unsupport type Hex will return value 0xFF
*******************************************************************************/
uint8_t SREC_AsciiToHex(char character);

/******************************************************************************
Object: Convert char to decimal
Input : srecLine : pointer(char*) to point the frist element of a line Srec
        parseData: pointer to point in type SREC_parseData_t with defined in srec.h
Output: Return fag status check srec with code error difine 
        in enum SREC_status_t in srec.h
*******************************************************************************/
uint8_t SREC_check(char *srecLine,SREC_parseData_t *parseData);

#endif