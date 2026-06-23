#ifndef __EXTI_H
#define __EXIT_H
#include "sys.h"
#include <stdbool.h> 

void EXTIX_Init(void); // 外部中斷初始化
// extern的意思是引用來自其他檔案的全域變數
extern bool cmd_ready;
extern char current_cmd;
#endif
