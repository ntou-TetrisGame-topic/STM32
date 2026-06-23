#ifndef TETRIS_H
#define TETRIS_H

#include "game.h"

#include <stdint.h>

// 地圖長寬
#define gridRow 32
#define gridColumn 16

// 下墜時間
#define dropIntervalMs 500

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	uint32_t *colors;
	Grid *record;
	int score;
} tetrisOutput;

tetrisOutput *tertisOutputConstructor(int, int, int, int);
void tertisOutputDestroy(tetrisOutput *);
void tertisOutputCopyAns(Grid *, Grid *);
void tertisOutputPrint(tetrisOutput *);
int tetris(void);
extern bool debug_mode;

#ifdef __cplusplus
}
#endif
	
#endif
