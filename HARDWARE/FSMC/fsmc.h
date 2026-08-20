#ifndef __FSMC_H
#define __FSMC_H

#include "sys.h" // 你的 SYSTEM/sys.h

// 定義 FSMC Bank1 NE1 的記憶體基底位址
#define FPGA_LED_BASE ((u32)0x60000000)

typedef struct
{
    volatile u8 g;
    volatile u8 r;
    volatile u8 b;
    volatile u8 dummy;
} FPGA_LED_t;

#define LED_RAM ((FPGA_LED_t *)FPGA_LED_BASE)

void FSMC_SRAM_Init(void);

#endif
