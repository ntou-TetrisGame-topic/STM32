#include "fsmc.h"

void FSMC_SRAM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef Timing;

    // 1. 開啟 GPIO (GPIOD, GPIOE, GPIOF, GPIOG) 與 FSMC 時脈
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE |
                               RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG,
                           ENABLE);
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

    // =========================================================================
    // 2. 配置 GPIO 復用功能 (AF_FSMC)
    // =========================================================================

    // GPIOD
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);  // D2
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);  // D3
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_FSMC);  // CLK
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);  // NOE
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);  // NWE
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);  // NE1
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC); // D0
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC); // D1

    // GPIOE
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);  // D4
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);  // D5
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);  // D6
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC); // D7

    // GPIOF
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_FSMC);  // A0
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_FSMC);  // A1
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF_FSMC);  // A2
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource3, GPIO_AF_FSMC);  // A3
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF_FSMC);  // A4
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF_FSMC);  // A5
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FSMC); // A6
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource13, GPIO_AF_FSMC); // A7
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource14, GPIO_AF_FSMC); // A8
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource15, GPIO_AF_FSMC); // A9

    // GPIOG
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource0, GPIO_AF_FSMC); // A10
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource1, GPIO_AF_FSMC); // A11
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource2, GPIO_AF_FSMC); // A12

    // 通用 GPIO 屬性設定
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    // 套用至 GPIOD
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 |
                                  GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // 套用至 GPIOE
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // 套用至 GPIOF
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 |
                                  GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // 套用至 GPIOG
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    // =========================================================================
    // 3. 配置 FSMC 時序與模式 
    // =========================================================================
    // STM32 HCLK = 168MHz (1 cycle ≈ 5.95ns)
    Timing.FSMC_AddressSetupTime = 0x01;      // 地址建立時間：3 * 5.95ns ≈ 17.85ns
    Timing.FSMC_AddressHoldTime = 0x02;       // Mode A 不使用此參數，保持原樣
    Timing.FSMC_DataSetupTime = 0x0F;         // 數據建立時間(NWE脈寬)：10 * 5.95ns ≈ 59.5ns
    Timing.FSMC_BusTurnAroundDuration = 0x02; // 總線週轉時間：2 * 5.95ns ≈ 11.9ns (關鍵：防止連續寫入防突波)
    Timing.FSMC_CLKDivision = 0x00;           // 異步模式無需分頻
    Timing.FSMC_DataLatency = 0x00;           // 異步模式不使用
    Timing.FSMC_AccessMode = FSMC_AccessMode_A;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &Timing;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &Timing;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}
