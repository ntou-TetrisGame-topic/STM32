#ifndef GAME_H
#define GAME_H

#include "grid.h"
#include "block.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // public
    bool gameOver;
    bool gameStop;
    int score;
    
    // private
    Grid *grid;
    block** blocks;
    int blockIndex;
    block *currentBlock;
    block *nextBlock;
} Game;

// public
Game* gameConstructor(int, int);
void gameDestructor(Game*);
void gameDraw(Game*, Grid*);
void gameHandleInput(Game*, int);
void gameMoveBlockDown(Game*);

// private
void gameMoveBlockLeft(Game*);
void gameMoveBlockRight(Game*);
block* gameGetRandomBlock(Game*);
void gameGetAllBlocks(Game*);
bool IsBlockOutside(Game*);
void gameRotateBlock(Game*);
void gameLockBlock(Game*);
bool gameBlockFits(Game*);
void gameReset(Game*);
void gameUpdateScore(Game*, int, int);

// helper-shuffle(blockes)
void shuffleBlocks(block**, int);
uint32_t myRand(void);
extern uint32_t seed;

#ifdef __cplusplus
}
#endif

#endif
