#include "MKL46Z4.h"
#include "UART0.h"
#include "srec.h"
#include "Flash.h"
#include "bootloader.h"
/***************************************************************
define for UART0
***************************************************************/
#define MODULE_CLOCK 48000000U
#define BAUD_RATE    115200U

/***************************************************************
define for GPIO 
***************************************************************/
#define SW1                 (FGPIOC->PDIR & 0x08)
#define SW_PRESSED          0
#define SW_UNPRESSED        1
#define RED_LED_PIN         (1 << 29)
#define GREEN_LED_PIN       (1 << 5)
#define RED_LED_OFF         (FPTE->PSOR |= RED_LED_PIN)
#define RED_LED_ON          (FPTE->PCOR |= RED_LED_PIN)
#define GREEN_LED_OFF       (FPTD->PSOR |= GREEN_LED_PIN)
#define GREEN_LED_ON        (FPTD->PCOR |= GREEN_LED_PIN)

void initLed(void);
void Init_SW1(void);
void Enter_Bootloader(void);

void main()
{
    Init_SW1();
    initLed();
    Init_UART0(BAUD_RATE,MODULE_CLOCK);
    UART_Send_String("Start.\r");
    if(SW1 == SW_PRESSED) //run mode BOOT
    {
        while(SW1 == SW_PRESSED);
        UART_Send_String("Entering Bootloader!\r");
        UART_Send_String("Please send file SREC!\r");
        Enter_Bootloader();
    }
    else // run mode APP
    {
        Bootloader_JumpToApplication();
    }
}

void Enter_Bootloader(void)
{
    uint8_t status_lineSREC = 0,j=0;
    SREC_parseData_t parseData;
    char *srecLine;

//Erase flash
    Bootloader_EraseFlash();
    while(status_lineSREC != parseStatus_Done)
    {
        while(QUEUE_empty());
        srecLine = QUEUE_PopLine();
        status_lineSREC = SREC_check(srecLine,&parseData);

//write data in flash
        if( (status_lineSREC != parseStatus_Error) && (status_lineSREC != parseStatus_Start))
        {
            for( j = 0; j < parseData.length; j+= 4)
            {
                Program_LongWord_8B((parseData.address + j), &(parseData.data[j]));
            }
            for( j = 0; j < parseData.length; j++)
            {
                parseData.data[j] = 0xFF;
            }
        }
        
        if(status_lineSREC == parseStatus_Done)
        {
            UART_Send_String("Loaded successfully. Plase press reset on board....\r");
        }
        QUEUE_pop();
    }
}

void Init_SW1(void)
    {
        SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
        PORTC->PCR[3] = (PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK);
        FGPIOC->PDDR |= 0 << 3;
    }

void initLed(void)
{
    SIM->SCGC5 |= (1 << 12);
    SIM->SCGC5 |= (1 << 13);
    PORTD->PCR[5] |= PORT_PCR_MUX(1);
    PORTE->PCR[29] |= PORT_PCR_MUX(1);
    GPIOD->PDDR |= GREEN_LED_PIN;
    GPIOE->PDDR |= RED_LED_PIN;
    RED_LED_ON;
    GREEN_LED_ON;
}

