#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

#include "position.h"
#include "grid.h"

#define MAX_ROTATIONS 4
#define MAX_CELLS_PER_BLOCK 4

#ifdef __cplusplus
extern "C"
{
#endif

#define BLOCK_COUNT 7

    typedef enum
    {
        BLOCK_L,
        BLOCK_J,
        BLOCK_I,
        BLOCK_O,
        BLOCK_S,
        BLOCK_T,
        BLOCK_Z
    } blockType;

    typedef struct
    {
        // public
        blockType type;
        int id;
        position cells[MAX_ROTATIONS][MAX_CELLS_PER_BLOCK];

        // private
        int rotationState;
        uint32_t *colors;
        int rowOffset;
        int columnOffset;

        // remember for last offset (show on nextBlockTiles will let rowOffset=0, columnOffset=0)
        int lastRowOffset;
        int lastColumnOffset;
    } block;

    // public
    block *blockConstructor(blockType);
    void blockDraw(block *, block *, Grid *, Grid *);
    void blockMove(block *, int, int);
    position *GetCellPositions(block *);
    void blockRotate(block *);
    void blockUndoRotate(block *);

#ifdef __cplusplus
}
#endif

#endif
