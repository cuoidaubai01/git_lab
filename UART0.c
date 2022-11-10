#include "UART0.h"

void Init_UART0(uint32_t BAUD_RATE,uint32_t MODULE_CLOCK) 
{
    uint8_t i;
    uint32_t calculated_baud = 0;
    uint32_t baud_diff = 0;
    uint32_t osr_val = 0;
    uint32_t sbr_val, uart0clk;
    uint32_t baud_rate;
    uint32_t reg_temp = 0;
    uint32_t temp = 0;
/***************************************************************/
/* Initializes UART0 for 8 bitdata, 1 bitstop , none parity format*/
/***************************************************************/
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; 
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    PORTA->PCR[1] &= ~(PORT_PCR_MUX_MASK); 
    PORTA->PCR[1] |= PORT_PCR_MUX(2);
    PORTA->PCR[2] &= ~(PORT_PCR_MUX_MASK);
    PORTA->PCR[2] |= PORT_PCR_MUX(2);
    SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
    SIM->SOPT2 |= (SIM_SOPT2_UART0SRC(1)); /* UART0SRC = 01 - Select MCGFLLCLK */
    UART0->C2 &= ~(UART0_C2_RE_MASK | UART0_C2_TE_MASK);// Disable UART0
    MCG->C2 &= ~( 1 << 2); //External reference clock - EREFS0 = 0 // dung
    MCG->C1 = (MCG_C1_CLKS(2) | MCG_C1_FRDIV(3)); //  External reference clock and Divide Factor is 8 
    /* Configure 48MHz */
    MCG->C4 |= MCG_C4_DRST_DRS(1); // DRST_DRS = 01
    MCG->C4 |= ( 1 << 7); // DMX32 = 1;

    /* Configure baud rate */
    baud_rate = BAUD_RATE;
    uart0clk = MODULE_CLOCK;
        i = 4;
    sbr_val = (uint32_t)(uart0clk/(baud_rate * i));
    calculated_baud = (uart0clk / (i * sbr_val));
        
    if (calculated_baud > baud_rate)
    {
        baud_diff = calculated_baud - baud_rate;
    }
    else
    {
        baud_diff = baud_rate - calculated_baud;
    }
    
    osr_val = i;
        
    // Select the best OSR value
    for (i = 5; i <= 32; i++)
    {
        sbr_val = (uint32_t)(uart0clk/(baud_rate * i));
        calculated_baud = (uart0clk / (i * sbr_val));
        
        if (calculated_baud > baud_rate)
            temp = calculated_baud - baud_rate;
        else
            temp = baud_rate - calculated_baud;
        
        if (temp <= baud_diff)
        {
            baud_diff = temp;
            osr_val = i; 
        }
    }
    if ((osr_val >3) && (osr_val < 9))
    {
        UART0->C5|= UART0_C5_BOTHEDGE_MASK;
    }
    // Setup OSR value 
    reg_temp = UART0->C4;
    reg_temp &= ~UART0_C4_OSR_MASK;
    reg_temp |= UART0_C4_OSR(osr_val-1);

    // Write reg_temp to C4 register
    UART0->C4 = reg_temp;
    
    reg_temp = (reg_temp & UART0_C4_OSR_MASK) + 1;
    sbr_val = (uint32_t)((uart0clk)/(baud_rate * (reg_temp)));
    
     /* Save off the current value of the uartx_BDH except for the SBR field */
    reg_temp = UART0->BDH & ~(UART0_BDH_SBR(0x1F));

    UART0->BDH = reg_temp |  UART0_BDH_SBR(((sbr_val & 0x1F00) >> 8));
    UART0->BDL = (uint8_t)(sbr_val & UART0_BDL_SBR_MASK);

//Enable Interrup
    NVIC_EnableIRQ(UART0_IRQn);
    UART0->C2 |= UART0_C2_RIE_MASK;
    UART0->D = 0;
    UART0->C2 |= (UART0_C2_RE_MASK | UART0_C2_TE_MASK);//Enable TE and RE
}

void UART_Send(char character)
{
       while (!(UART0->S1 & UART_S1_TDRE_MASK));
       UART0->D = character;
}

void UART_Send_String(char *ptr)
    {
        while (*ptr != '\0')
        {
            while (!(UART0->S1 & UART_S1_TDRE_MASK));
            UART0->D = *(ptr++);
        }
    }

void UART0_IRQHandler (void) 
{
    char character = 0xFF;
    char * ptr = QUEUE_PushLine();
    static uint8_t i = 0;

    if (UART0->S1 & UART0_S1_RDRF_MASK) 
    {
        character = UART0->D;
        if(character!= '\n')
        {
            *(ptr+i) = character;
            i++;
        }
        else
        {
            QUEUE_push();
            i = 0;
        }
    }
}

void disable_UART_Intrrup()
{
    UART0->C2 &= ~(UART0_C2_RE_MASK | UART0_C2_TE_MASK);
    NVIC_DisableIRQ(UART0_IRQn);
    UART0->C2 &= ~UART0_C2_RIE_MASK;
}


