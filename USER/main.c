#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "display.h"
#include "beep.h"
#include "key.h"
#include "exti.h"
#include "grid.h"
#include "fsmc.h"
#include <stdbool.h>

/*
 * 傳給 ESP32 的固定 19 碼格式：
 * [0..15]  NEXT 4x4，row-major，0=空格，1~7=方塊顏色
 * [16..17] 分數增量，00~99
 * [18]     狀態，0=continue，1=reset
 * 最後加 \n，方便 ESP32 readline 或 uart.read 緩衝解析。
 */
void ESP32_SendGameData(u8 next_block[16], u8 add_score, u8 reset)
{
    char buf[21];
    int i;

    for (i = 0; i < 16; i++)
    {
        if (next_block[i] <= 9)
            buf[i] = next_block[i] + '0';
        else
            buf[i] = '0';
    }

    if (add_score > 99)
        add_score = 99;

    buf[16] = add_score / 10 + '0';
    buf[17] = add_score % 10 + '0';

    buf[18] = reset ? '1' : '0';

    buf[19] = '\n';
    buf[20] = '\0';

    USART_SendString(USART2, buf);
}


bool cmd_ready = false;
char current_cmd = ' ';
// 切換PC/FPGA輸出
bool debug_mode = false;

int main(void)
{
	int tetrisScore;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart_init(2500000); // FPGA mode
	
	uart2_init(115200);
	//uart_init(115200); // debug mode
	EXTIX_Init();       //初始化外部中斷輸入 

	LED_Init();
	// timer 3 interrupt
	TIM3_Int_Init(3000 - 1, 8400 - 1);
	FSMC_SRAM_Init();
	
	*(__IO uint8_t *)(0x60000000) = 0xFF; // G
	*(__IO uint8_t *)(0x60000001) = 0xFF; // R
	*(__IO uint8_t *)(0x60000002) = 0xFF; // B
	
	while (true)
	{
		tetrisScore = tetris();
		// printf("Max Score: %d\n", tetrisScore);
		return 0;
	};
}
