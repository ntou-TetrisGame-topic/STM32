#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "exti.h"

// 外部中斷0服務程序 (WK_UP) -> 對應 'w' (旋轉)
void EXTI0_IRQHandler(void)
{
	delay_ms(10);	// 消抖 (註：未來實戰中，ISR 內盡量避免使用 delay，但測試用沒關係)
	if(WK_UP==1)
	{
		current_cmd = 'w';
		cmd_ready = true;
	}		 
	EXTI_ClearITPendingBit(EXTI_Line0); // 清除中斷標誌位 
}

// 外部中斷2服務程序 (KEY2) -> 對應 'a' (向左)
void EXTI2_IRQHandler(void)
{
	delay_ms(10);	// 消抖
	if(KEY2==0)	  
	{				 
		current_cmd = 'a';
		cmd_ready = true; 
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}

// 外部中斷3服務程序 (KEY1) -> 對應 's' (向下)
void EXTI3_IRQHandler(void)
{
	delay_ms(10);	// 消抖
	if(KEY1==0)	 
	{	
		current_cmd = 's';
		cmd_ready = true;
	}		 
	EXTI_ClearITPendingBit(EXTI_Line3);
}

// 外部中斷4服務程序 (KEY0) -> 對應 'd' (向右)
void EXTI4_IRQHandler(void)
{
	delay_ms(10);	// 消抖
	if(KEY0==0)	 
	{				 
		current_cmd = 'd';
		cmd_ready = true;
	}		 
	EXTI_ClearITPendingBit(EXTI_Line4);
}

// 外部中斷初始化程序
// 初始化PE2~4,PA0為中斷輸入.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	KEY_Init(); // 按鍵對應的IO口初始化

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // 使能SYSCFG時鐘

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2); // PE2 連接到中斷線2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3); // PE3 連接到中斷線3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4); // PE4 連接到中斷線4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0); // PA0 連接到中斷線0

	/* 配置EXTI_Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;			   // LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	   // 中斷事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿觸發
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			   // 使能LINE0
	EXTI_Init(&EXTI_InitStructure);						   // 配置

	/* 配置EXTI_Line2,3,4 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line3 | EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		// 中斷事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿觸發
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				// 中斷線使能
	EXTI_Init(&EXTI_InitStructure);							// 配置

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			 // 外部中斷0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; // 搶佔優先級0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		 // 子優先級2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 // 使能外部中斷通道
	NVIC_Init(&NVIC_InitStructure);								 // 配置

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			 // 外部中斷2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03; // 搶佔優先級3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		 // 子優先級2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 // 使能外部中斷通道
	NVIC_Init(&NVIC_InitStructure);								 // 配置

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			 // 外部中斷3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; // 搶佔優先級2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		 // 子優先級2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 // 使能外部中斷通道
	NVIC_Init(&NVIC_InitStructure);								 // 配置

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			 // 外部中斷4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // 搶佔優先級1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		 // 子優先級2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 // 使能外部中斷通道
	NVIC_Init(&NVIC_InitStructure);								 // 配置
}
