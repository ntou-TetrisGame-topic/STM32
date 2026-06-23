#include "detectKey.h"
#include "game.h"

#include <stdlib.h>
#include <stdbool.h>

uint32_t seed = 1;

Game *gameConstructor(int rows, int cols)
{
    Game *g = (Game *)malloc(sizeof(Game));
    g->grid = gridConstructor(rows, cols);
    gameGetAllBlocks(g);
    g->blockIndex = BLOCK_COUNT;
    g->currentBlock = gameGetRandomBlock(g);
    g->nextBlock = gameGetRandomBlock(g);
    g->gameOver = false;
    g->gameStop = false;
    g->score = 0;
    return g;
}

void gameDestructor(Game *g)
{
    int i;
    for (i = 0; i < BLOCK_COUNT; i++)
    {
        free(g->blocks[i]);
    }
    free(g->blocks);
    free(g);
}

block *gameGetRandomBlock(Game *g)
{
    if (g->blockIndex >= BLOCK_COUNT)
    {
        shuffleBlocks(g->blocks, BLOCK_COUNT);
        g->blockIndex = 0;
    }
    return g->blocks[(g->blockIndex)++];
}

uint32_t myRand(void)
{
    seed = seed * 1664525 + 1013904222;
    return seed;
}

void shuffleBlocks(block **b, int n)
{
    int i, j;
    block *temp;
    for (i = n - 1; i > 0; i--)
    {
        j = myRand() % (BLOCK_COUNT);
        temp = b[i];
        b[i] = b[j];
        b[j] = temp;
    }
}

void gameGetAllBlocks(Game *g)
{
    g->blocks = (block **)malloc(sizeof(block *) * BLOCK_COUNT);
    g->blocks[0] = blockConstructor(BLOCK_I);
    g->blocks[1] = blockConstructor(BLOCK_J);
    g->blocks[2] = blockConstructor(BLOCK_L);
    g->blocks[3] = blockConstructor(BLOCK_O);
    g->blocks[4] = blockConstructor(BLOCK_S);
    g->blocks[5] = blockConstructor(BLOCK_T);
    g->blocks[6] = blockConstructor(BLOCK_Z);
}

void gameDraw(Game *g, Grid *t)
{
    blockDraw(g->currentBlock, g->nextBlock, g->grid, t);
}

void gameHandleInput(Game *g, int key)
{
    //
    switch (key)
    {
    // 旋轉
    case 'W':
    case 'w':
        gameRotateBlock(g);
        break;

    // 向下（加速下落）
    case 'S':
    case 's':
        gameMoveBlockDown(g);
        gameUpdateScore(g, 0, 1);
        break;

    // 向左移動
    case 'A':
    case 'a':
        gameMoveBlockLeft(g);
        break;

    // 向右移動
    case 'D':
    case 'd':
        gameMoveBlockRight(g);
        break;

    default:
        break;
    }
}

void gameMoveBlockLeft(Game *g)
{
    if (g->gameOver == false)
    {
        blockMove(g->currentBlock, 0, -1);
        if (IsBlockOutside(g) || gameBlockFits(g) == false)
        {
            blockMove(g->currentBlock, 0, 1);
        }
    }
}

void gameMoveBlockRight(Game *g)
{
    if (g->gameOver == false)
    {
        blockMove(g->currentBlock, 0, 1);
        if (IsBlockOutside(g) || gameBlockFits(g) == false)
        {
            blockMove(g->currentBlock, 0, -1);
        }
    }
}

void gameMoveBlockDown(Game *g)
{
    if (g->gameOver == false)
    {
        blockMove(g->currentBlock, 1, 0);
        if (IsBlockOutside(g) || gameBlockFits(g) == false)
        {
            blockMove(g->currentBlock, -1, 0);
            gameLockBlock(g);
        }
    }
}

bool IsBlockOutside(Game *g)
{
    int i;
    position *tiles = GetCellPositions(g->currentBlock);
    for (i = 0; i < MAX_CELLS_PER_BLOCK; i++)
    {
        if (gridIsCellOutside(g->grid, tiles[i].row, tiles[i].column))
        {
            free(tiles);
            return true;
        }
    }
    free(tiles);
    return false;
}

void gameRotateBlock(Game *g)
{
    if (g->gameOver == false)
    {
        blockRotate(g->currentBlock);
        if (IsBlockOutside(g) || gameBlockFits(g) == false)
        {
            blockUndoRotate(g->currentBlock);
        }
    }
}

void gameLockBlock(Game *g)
{
    int i, rowsCleared;
    position *tiles = GetCellPositions(g->currentBlock);
    for (i = 0; i < MAX_CELLS_PER_BLOCK; i++)
    {
        g->grid->grid[tiles[i].row][tiles[i].column] = g->grid->colors[g->currentBlock->id];
    }
    free(tiles);
    // change block
    g->currentBlock = blockConstructor(g->nextBlock->type);
    if (gameBlockFits(g) == false)
    {

        g->gameOver = true;
    }
    g->nextBlock = gameGetRandomBlock(g);
    rowsCleared = gridClearFullRows(g->grid);
    if (rowsCleared > 0)
    {
        gameUpdateScore(g, rowsCleared, 0);
    }
}

bool gameBlockFits(Game *g)
{
    int i;
    position *tiles = GetCellPositions(g->currentBlock);
    for (i = 0; i < MAX_CELLS_PER_BLOCK; i++)
    {
        if (gridIsCellEmpty(g->grid, tiles[i].row, tiles[i].column) == false)
        {
            free(tiles);
            return false;
        }
    }
    free(tiles);
    return true;
}

void gameReset(Game *g)
{
    gridInitialize(g->grid);
    gameGetAllBlocks(g);
    g->currentBlock = gameGetRandomBlock(g);
    g->nextBlock = gameGetRandomBlock(g);
    g->score = 0;
}

void gameUpdateScore(Game *g, int linesCleared, int moveDownPoints)
{
    switch (linesCleared)
    {
    case 0:
        g->score += 0;
        break;
    case 1:
        g->score += 100;
        break;
    case 2:
        g->score += 200;
        break;
    case 3:
        g->score += 300;
        break;
    default:
        g->score += 400;
        break;
    }
    g->score += moveDownPoints;
}
