#include "sys.h"
#include "usart.h"
#include "exti.h"
//////////////////////////////////////////////////////////////////////////////////
// If you use uCOS, keep original include logic.
#if SYSTEM_SUPPORT_OS
#include "includes.h"
#endif
//////////////////////////////////////////////////////////////////////////////////

// printf redirect (MicroLIB / no semihosting)
#if 1
#pragma import(__use_no_semihosting)
struct __FILE { int handle; };
FILE __stdout;
_sys_exit(int x) { x = x; }

// NOTE:
// This fputc still writes to USART1.
// If you use USART1 to stream frames to FPGA, DO NOT call printf() in your main loop,
// or you'll inject ASCII into your binary stream.
int fputc(int ch, FILE *f)
{
    while((USART1->SR & 0X40) == 0); // wait TC
    USART1->DR = (u8)ch;
    return ch;
}
#endif

//========================================================
//stm32 to esp32 send function( send next_block and score)
//========================================================

void USART_SendChar(USART_TypeDef* USARTx, char ch)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, ch);
}

void USART_SendString(USART_TypeDef* USARTx, char *str)
{
    while (*str)
    {
        USART_SendChar(USARTx, *str);
        str++;
    }
}

// =================================================================================
// UART1 Init (PA9 TX, PA10 RX)  +  Simple TX API for FPGA streaming
// =================================================================================

void uart_init(u32 bound)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // AF mapping
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    // GPIO config
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART config
    USART_InitStructure.USART_BaudRate            = bound;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx; // keep RX for future ACK
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);

#if EN_USART1_RX
    // Enable RX interrupt (optional)
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // NVIC config for USART1 IRQ
    {
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }
#endif
}

// Blocking send buffer (safe & simple).
// For 30fps with 389 bytes/frame @ 230400 baud, this is fine.
// If later you want higher fps or larger frames, switch to DMA.
void uart_send_buf(const u8 *buf, u16 len)
{
	u16 i;
    for (i = 0; i < len; i++)
    {
        // Wait DR empty
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) { }
        USART_SendData(USART1, buf[i]);
    }
    // Wait entire frame shifted out
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) { }
}

// =================================================================================
// (Optional) If you want FPGA -> STM32 ACK/NAK, you can enable RX interrupt here.
// But for "only STM32 -> FPGA streaming" you can ignore the rest.
// =================================================================================

// If your original project expects these symbols, keep them guarded by EN_USART1_RX.
#if EN_USART1_RX

u8  USART_RX_BUF[USART_REC_LEN];
u16 USART_RX_STA = 0;

void USART1_IRQHandler(void)
{
    u8 Res;

#if SYSTEM_SUPPORT_OS
    OSIntEnter();
#endif

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        Res = (u8)USART_ReceiveData(USART1);

        if ((USART_RX_STA & 0x8000) == 0)
        {
            if (USART_RX_STA & 0x4000)
            {
                if (Res != 0x0a) USART_RX_STA = 0;
                else             USART_RX_STA |= 0x8000;
            }
            else
            {
                if (Res == 0x0d) USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1)) USART_RX_STA = 0;
                }
            }
        }

        if (Res == 'a' || Res == 's' || Res == 'd' || Res == 'w' || Res == 't' || Res == 'q')
        {
            current_cmd = Res;
            cmd_ready = 1; 
        }
    }

#if SYSTEM_SUPPORT_OS
    OSIntExit();
#endif
}
#endif


 void uart2_init(u32 bound) {
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);
}
