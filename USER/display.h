#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "sys.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include <string.h>

// 定義 LED 陣列相關常數
#define GRID_W 16
#define GRID_H 32
#define CELL_BYTES 3
#define PAYLOAD_LEN (GRID_W * GRID_H * CELL_BYTES)
extern u8 LED_MAP[PAYLOAD_LEN];; // 用於存放整個 LED 板的 RGB 資料 (16*32*3)

#ifdef __cplusplus
extern "C" {
#endif

void send_led_chunk(u16 comp_index, u8 length, u8 *rgb_data);
void get_rainbow_color(u16 wheel_pos, u16 *r, u16 *g, u16 *b);
void write_to_map(u8 *map, u8 row, u8 col, u8 r, u8 g, u8 b);
void display_test(void);

#ifdef __cplusplus
}
#endif

#endif // __DISPLAY_H
