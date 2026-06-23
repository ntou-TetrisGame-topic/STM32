#include "block.h"
#include "color.h"
#include "grid.h"
#include "tetris.h"

#include <stdlib.h>

block* blockConstructor (blockType bt) {
    int r;
    block* b = (block*)malloc(sizeof(block));
    b->colors = GetCellColors();
    b->rotationState = 0;
    b->rowOffset = 0;
    b->columnOffset = 0;
    b->lastRowOffset = 0;
    b->lastColumnOffset = 0;
    if (bt == BLOCK_L) {
        b->type = BLOCK_L;
        b->id = 1;
        // 1
        // 1 1 1
        b->cells[0][0].row = 0;
        b->cells[0][0].column = 2;

        b->cells[0][1].row = 1;
        b->cells[0][1].column = 0;
        
        b->cells[0][2].row = 1;
        b->cells[0][2].column = 1;
        
        b->cells[0][3].row = 1;
        b->cells[0][3].column = 2;

        //  1
        //  1
        //  1
        //  1 1
        b->cells[1][0].row = 0;
        b->cells[1][0].column = 1;

        b->cells[1][1].row = 1;
        b->cells[1][1].column = 1;

        b->cells[1][2].row = 2;
        b->cells[1][2].column = 1;

        b->cells[1][3].row = 2;
        b->cells[1][3].column = 2;
        
        // 1 1 1
        //     1
        b->cells[2][0].row = 1;
        b->cells[2][0].column = 0;

        b->cells[2][1].row = 1;
        b->cells[2][1].column = 1;

        b->cells[2][2].row = 1;
        b->cells[2][2].column = 2;

        b->cells[2][3].row = 2;
        b->cells[2][3].column = 0;
        
        // 1 1
        //   1
        //   1
        b->cells[3][0].row = 0;
        b->cells[3][0].column = 0;

        b->cells[3][1].row = 0;
        b->cells[3][1].column = 1;

        b->cells[3][2].row = 1;
        b->cells[3][2].column = 1;

        b->cells[3][3].row = 2;
        b->cells[3][3].column = 1;

        blockMove(b, 0, 2);
    }
    else if (bt == BLOCK_J) {
        b->type = BLOCK_J;
        b->id = 2;

        // mirror of L
        // 1
        // 1 1 1
        b->cells[0][0].row = 0;
        b->cells[0][0].column = 0;

        b->cells[0][1].row = 1;
        b->cells[0][1].column = 0;

        b->cells[0][2].row = 1;
        b->cells[0][2].column = 1;

        b->cells[0][3].row = 1;
        b->cells[0][3].column = 2;

        //  1 1
        //  1
        //  1
        b->cells[1][0].row = 0;
        b->cells[1][0].column = 1;

        b->cells[1][1].row = 0;
        b->cells[1][1].column = 2;

        b->cells[1][2].row = 1;
        b->cells[1][2].column = 1;

        b->cells[1][3].row = 2;
        b->cells[1][3].column = 1;

        // 1 1 1
        //     1
        b->cells[2][0].row = 1;
        b->cells[2][0].column = 0;

        b->cells[2][1].row = 1;
        b->cells[2][1].column = 1;

        b->cells[2][2].row = 1;
        b->cells[2][2].column = 2;

        b->cells[2][3].row = 2;
        b->cells[2][3].column = 2;

        //   1
        //   1
        // 1 1
        b->cells[3][0].row = 0;
        b->cells[3][0].column = 1;

        b->cells[3][1].row = 1;
        b->cells[3][1].column = 1;

        b->cells[3][2].row = 2;
        b->cells[3][2].column = 0;
        
        b->cells[3][3].row = 2;
        b->cells[3][3].column = 1;
   
        blockMove(b, 0, 2);
    }
    else if (bt == BLOCK_I) {
        b->type = BLOCK_I;
        b->id = 3;

        // I tetromino (4 in a line)
        // 1 1 1 1
        b->cells[0][0].row = 1;
        b->cells[0][0].column = 0;

        b->cells[0][1].row = 1;
        b->cells[0][1].column = 1;

        b->cells[0][2].row = 1;
        b->cells[0][2].column = 2;

        b->cells[0][3].row = 1;
        b->cells[0][3].column = 3;

        //    1
        //    1
        //    1
        //    1
        b->cells[1][0].row = 0;
        b->cells[1][0].column = 2;

        b->cells[1][1].row = 1;
        b->cells[1][1].column = 2;

        b->cells[1][2].row = 2;
        b->cells[1][2].column = 2;

        b->cells[1][3].row = 3;
        b->cells[1][3].column = 2;

        // repeat rotations 2 and 3 as mirrors
        // first part
        b->cells[2][0].row = 2;
        b->cells[2][0].column = 0;

        b->cells[2][1].row = 2;
        b->cells[2][1].column = 1;

        b->cells[2][2].row = 2;
        b->cells[2][2].column = 2;

        b->cells[2][3].row = 2;
        b->cells[2][3].column = 3;

        // another part
        b->cells[3][0].row = 0;
        b->cells[3][0].column = 1;

        b->cells[3][1].row = 1;
        b->cells[3][1].column = 1;

        b->cells[3][2].row = 2;
        b->cells[3][2].column = 1;

        b->cells[3][3].row = 3;
        b->cells[3][3].column = 1;
        blockMove(b, -1, 2);
    }
    else if (bt == BLOCK_O) {
        b->type = BLOCK_O;
        b->id = 4;

        // O tetromino (2x2), same in all rotations
        // 1 1
        // 1 1
        b->cells[0][0].row = 0;
        b->cells[0][0].column = 0;

        b->cells[0][1].row = 0;
        b->cells[0][1].column = 1;

        b->cells[0][2].row = 1;
        b->cells[0][2].column = 0;

        b->cells[0][3].row = 1;
        b->cells[0][3].column = 1;

        for (r = 1; r < MAX_ROTATIONS; r++) {
            b->cells[r][0] = b->cells[0][0];
            b->cells[r][1] = b->cells[0][1];
            b->cells[r][2] = b->cells[0][2];
            b->cells[r][3] = b->cells[0][3];
        }
        blockMove(b, 0, 3);
    }
    else if (bt == BLOCK_S) {
        b->type = BLOCK_S;
        b->id = 5;

        // S tetromino
        //   1 1
        // 1 1
        b->cells[0][0].row = 0;
        b->cells[0][0].column = 1;

        b->cells[0][1].row = 0;
        b->cells[0][1].column = 2;

        b->cells[0][2].row = 1;
        b->cells[0][2].column = 0;

        b->cells[0][3].row = 1;
        b->cells[0][3].column = 1;

        //   1
        //   1 1
        //     1
        b->cells[1][0].row = 0;
        b->cells[1][0].column = 1;

        b->cells[1][1].row = 1;
        b->cells[1][1].column = 1;

        b->cells[1][2].row = 1;
        b->cells[1][2].column = 2;

        b->cells[1][3].row = 2;
        b->cells[1][3].column = 2;

        //   1 1
        // 1 1
        b->cells[2][0].row = 1;
        b->cells[2][0].column = 1;

        b->cells[2][1].row = 1;
        b->cells[2][1].column = 2;

        b->cells[2][2].row = 2;
        b->cells[2][2].column = 0;
        
        b->cells[2][3].row = 2;
        b->cells[2][3].column = 1;

        // 1
        // 1 1
        //   1
        b->cells[3][0].row = 0;
        b->cells[3][0].column = 0;

        b->cells[3][1].row = 1;
        b->cells[3][1].column = 0;

        b->cells[3][2].row = 1;
        b->cells[3][2].column = 1;

        b->cells[3][3].row = 2;
        b->cells[3][3].column = 1;

        blockMove(b, 0, 2);
    }
    else if (bt == BLOCK_T) {
        b->type = BLOCK_T;
        b->id = 6;

        // T tetromino
        //   1
        // 1 1 1
        b->cells[0][0].row = 0;
        b->cells[0][0].column = 1;

        b->cells[0][1].row = 1;
        b->cells[0][1].column = 0;

        b->cells[0][2].row = 1;
        b->cells[0][2].column = 1;

        b->cells[0][3].row = 1;
        b->cells[0][3].column = 2;

        //   1
        //   1 1
        //   1
        b->cells[1][0].row = 0;
        b->cells[1][0].column = 1;

        b->cells[1][1].row = 1;
        b->cells[1][1].column = 1;

        b->cells[1][2].row = 1;
        b->cells[1][2].column = 2;
        
        b->cells[1][3].row = 2;
        b->cells[1][3].column = 1;

        // 1 1 1
        //   1
        b->cells[2][0].row = 1;
        b->cells[2][0].column = 0;

        b->cells[2][1].row = 1;
        b->cells[2][1].column = 1;

        b->cells[2][2].row = 1;
        b->cells[2][2].column = 2;

        b->cells[2][3].row = 2;
        b->cells[2][3].column = 1;

        //   1
        // 1 1
        //   1
        b->cells[3][0].row = 0;
        b->cells[3][0].column = 1;

        b->cells[3][1].row = 1;
        b->cells[3][1].column = 0;

        b->cells[3][2].row = 1;
        b->cells[3][2].column = 1;
        
        b->cells[3][3].row = 2;
        b->cells[3][3].column = 1;
        blockMove(b, 0, 2);
    }
    else if (bt == BLOCK_Z) {
        b->type = BLOCK_Z;
        b->id = 7;

        // Z tetromino
        // 1 1
        //   1 1
        b->cells[0][0].row = 0;
        b->cells[0][0].column = 0;

        b->cells[0][1].row = 0;
        b->cells[0][1].column = 1;

        b->cells[0][2].row = 1;
        b->cells[0][2].column = 1;

        b->cells[0][3].row = 1;
        b->cells[0][3].column = 2;

        //     1
        //   1 1
        //   1
        b->cells[1][0].row = 0;
        b->cells[1][0].column = 2;

        b->cells[1][1].row = 1;
        b->cells[1][1].column = 1;

        b->cells[1][2].row = 1;
        b->cells[1][2].column = 2;

        b->cells[1][3].row = 2;
        b->cells[1][3].column = 1;

        // 1 1
        //   1 1 
        b->cells[2][0].row = 1;
        b->cells[2][0].column = 0;

        b->cells[2][1].row = 1;
        b->cells[2][1].column = 1;

        b->cells[2][2].row = 2;
        b->cells[2][2].column = 1;

        b->cells[2][3].row = 2;
        b->cells[2][3].column = 2;

        //   1
        // 1 1
        // 1
        b->cells[3][0].row = 0;
        b->cells[3][0].column = 1;

        b->cells[3][1].row = 1;
        b->cells[3][1].column = 0;

        b->cells[3][2].row = 1;
        b->cells[3][2].column = 1;

        b->cells[3][3].row = 2;
        b->cells[3][3].column = 0;

        blockMove(b, 0, 2);
    }
    return b;
}

void blockDraw(block* b, block* n, Grid *g, Grid *ans) {
    int i;
    position *nextTiles;
    position *tiles = GetCellPositions(b);

    // 下落過程標記
    for (i = 0; i<MAX_CELLS_PER_BLOCK; i++) {
        g->grid[tiles[i].row][tiles[i].column] = g->colors[b->id];
    }

    n->lastRowOffset = n->rowOffset; n->lastColumnOffset = n->columnOffset;
    n->rowOffset = 0; n->columnOffset = 0;
    nextTiles = GetCellPositions(n);
    for (i = 0; i<MAX_CELLS_PER_BLOCK; i++) {
        g->nextBlockGrid[nextTiles[i].row][nextTiles[i].column] = g->colors[n->id];
    }
    n->rowOffset = n->lastRowOffset; n->columnOffset = n->lastColumnOffset;

    // (遊戲內容) 輸出
    gridPrint(g);

    // 保存輸出
    tertisOutputCopyAns(g, ans);

    // 取消下落過程標記
    for (i = 0; i<MAX_CELLS_PER_BLOCK; i++) {
        g->grid[tiles[i].row][tiles[i].column] = g->colors[0];
    }
    for (i = 0; i<MAX_CELLS_PER_BLOCK; i++) {
        g->nextBlockGrid[nextTiles[i].row][nextTiles[i].column] = g->colors[0];
    }
    free (tiles);
    free (nextTiles);
}

void blockMove(block* b, int rows, int columns) {
    b->rowOffset += rows;
    b->columnOffset += columns;
}

position* GetCellPositions(block* b) {
    int i;
    position *tiles = b->cells[b->rotationState];
    position *movedTiles = (position*)malloc(sizeof(position)*MAX_CELLS_PER_BLOCK);
    for (i = 0; i < MAX_CELLS_PER_BLOCK; i++) {
        movedTiles[i].row = tiles[i].row + b->rowOffset;
        movedTiles[i].column = tiles[i].column + b->columnOffset;
    }
    return movedTiles;
}

void blockRotate(block* b) {
    b->rotationState++;
    if (b->rotationState == MAX_ROTATIONS) {
        b->rotationState = 0;
    }
}

void blockUndoRotate(block* b) {
    b->rotationState--;
    if (b->rotationState == -1) {
        b->rotationState = MAX_ROTATIONS - 1;
    }
}
