#include "color.h"
#include <stdint.h>

// 使用 HEX 格式定義 10 種顏色 (0xRRGGBB)
// const uint32_t darkGrey = 0x1A1F28;    // #1A1F28
const uint32_t darkGrey = 0x000000;  // #000000
const uint32_t green = 0x2FE617;     // #2FE617
const uint32_t red = 0xE81212;       // #E81212
const uint32_t orange = 0xE27411;    // #E27411
const uint32_t yellow = 0xEDEA04;    // #EDEA04
const uint32_t purple = 0xA600F7;    // #A600F7
const uint32_t cyan = 0x15CCD1;      // #15CCD1
const uint32_t blue = 0x0D40D8;      // #0D40D8
const uint32_t lightBlue = 0x3B55A2; // #3B55A2
const uint32_t darkBlue = 0x2C2C7F;  // #2C2C7F

// 8 種顏色
uint32_t *GetCellColors()
{
    static uint32_t colors[] = {
        darkGrey, green, red, orange, yellow,
        purple, cyan, blue};
    return colors;
}
