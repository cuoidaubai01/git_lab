#ifndef UART0_H
#define UART0_H

#include <MKL46Z4.h>
#include "queue.h"

/*****************************************************************
Object: Setup for UART0 with clock 48Mhz, Baud rate 115200
Input:  BAUD_RATE       : (Ex : 115200U)
        MODULE_CLOCK    : (Ex :48000000U)
Output: No
******************************************************************/
void Init_UART0();



/*****************************************************************
Object: Send a character
Input:  character want to send
Output: No
******************************************************************/
void UART_Send(char character);


/*****************************************************************
Object: Send a string
Input:  ptr : pointer to point a string with end of string is '\0'
Output: No
******************************************************************/
void UART_Send_String(char *ptr);


/*****************************************************************
Object: Interrup UART0, a character received put in Queue
Input:  No
Output: No
******************************************************************/
void UART0_IRQHandler (void);


/*****************************************************************
Object: disable interrup UART0
Input:  No
Output: No
******************************************************************/
void disable_UART_Intrrup();

#endif