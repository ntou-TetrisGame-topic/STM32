#include "display.h"

/*
 * RAW 16*16 RGB demo (NO header, NO 8x16)
 * - Each frame = 16*32 pixels
 * - Each pixel = 3 bytes (R,G,B)
 * - Payload length = 16*32*3 = 1536 bytes
 *
 * FPGA side suggestion for this test:
 *   - Treat every 192 bytes as one frame (row-major order).
 *   - Pixel order: row 0..7, col 0..7
 *   - Byte order per pixel: R, G, B
 *
 * UART: USART1 (PA9 TX) -> FPGA RX
 * Baud: 2500000 (enough for 117fps with 1696B/frame)
 */

u8 LED_MAP[PAYLOAD_LEN]; // 用於存放整個 LED 板的 RGB 資料 (16*32*3)

// 發送一整個 Block (16顆 LED) 的封包
// comp_index: 壓縮後的 index (0~63)
// length: 此次傳送的 LED 數量 (通常為 16)
// rgb_data: 指向連續 RGB 資料的指標 (大小應為 length * 3)
void send_led_chunk(u16 comp_index, u8 length, u8 *rgb_data)
{
    u8 packet[60]; // Header(4) + Max Payload(48) + Checksum(1)
    u16 idx = 0;
    u8 chk = comp_index ^ length;
    u8 r, g, b;
    u8 i;

    packet[idx++] = 0xAA;
    packet[idx++] = 0x55;
    packet[idx++] = comp_index;
    packet[idx++] = length;

    // 將 RGB 轉換為 WS2812 需要的 GRB 順序，並計算 Checksum
    for (i = 0; i < length; i++)
    {
        r = rgb_data[comp_index * GRID_W * CELL_BYTES + i * 3 + 0];
        g = rgb_data[comp_index * GRID_W * CELL_BYTES + i * 3 + 1];
        b = rgb_data[comp_index * GRID_W * CELL_BYTES + i * 3 + 2];

        packet[idx++] = g; // WS2812 格式: G
        packet[idx++] = r; // WS2812 格式: R
        packet[idx++] = b; // WS2812 格式: B

        chk ^= g;
        chk ^= r;
        chk ^= b;
    }

    packet[idx++] = chk; // 放入 Checksum

    uart_send_buf(packet, idx);
}

/*
 * 色輪轉換函式 (0~255)
 * 輸入 0~255 的數值，會平滑過渡：紅 -> 黃 -> 綠 -> 青 -> 藍 -> 紫 -> 紅
 */
void get_rainbow_color(u16 wheel_pos, u16 *r, u16 *g, u16 *b)
{
    wheel_pos = 255 - wheel_pos;
    if (wheel_pos < 85)
    {
        *r = 255 - wheel_pos * 3;
        *g = 0;
        *b = wheel_pos * 3;
    }
    else if (wheel_pos < 170)
    {
        wheel_pos -= 85;
        *r = 0;
        *g = wheel_pos * 3;
        *b = 255 - wheel_pos * 3;
    }
    else
    {
        wheel_pos -= 170;
        *r = wheel_pos * 3;
        *g = 255 - wheel_pos * 3;
        *b = 0;
    }
}

void write_to_map(u8 *map, u8 row, u8 col, u8 r, u8 g, u8 b)
{
    u16 idx = (row * GRID_W * CELL_BYTES) + (col * CELL_BYTES);

    map[idx++] = r;
    map[idx++] = g;
    map[idx++] = b;
}

void display_test(void)
{
    u32 tick;
    u8 time_offset = 0; // 用來記錄時間偏移，產生流動感
    u16 i, j, r, g, b;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init(168);
    LED_Init();

    /* UART to FPGA (目前 Baud Rate 2500000) */
    uart_init(115200);

    tick = 0;

    while (1)
    {
        // 目前是反向刷新(由後到前)LED燈板
        // 迴圈發送 32*16 顆 LED 的封包
        for (i = 0; i < 32; i++)
        {
            for (j = 0; j < 16; j++)
            {
                // 計算這顆 LED 應該顯示的顏色位置
                // i * 16 決定了彩虹跨越的長度 (數字越大，漸層越密集)
                // + time_offset 則是讓顏色隨時間推進
                u8 current_hue = (i * 8 + time_offset) & 0xFF;

                // get_rainbow_color(current_hue, &r, &g, &b);

                // 在實驗階段，建議將亮度除以 4 (約 25% 亮度) 或除以 8
                r = r / 16;
                g = g / 16;
                b = b / 16;

                // write_to_map(LED_MAP, i, j, r, g, b);
            }
            send_led_chunk(i, 16, LED_MAP); // 發送這一列的 16 顆 LED 資料
        }

        // 每次迴圈結束，讓時間偏移量增加
        time_offset += 1; // 數字越大，流動速度越快

        /* heartbeat LED */
        if ((tick % 10) == 0)
            LED0 = !LED0;
        tick++;

        // 目前為在512顆LED燈的情況下，117FPS的設定。
        delay_ms(1);
    }
}