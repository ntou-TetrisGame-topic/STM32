#ifndef GRID_H
#define GRID_H

#include <stdint.h>
#include <stdbool.h>

#define nextBlockRow 8
#define nextBlockCol 4

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Grid {
    // public
    int **grid;
    int **nextBlockGrid;

    // private
    int numRows;
    int numCols;
    uint32_t *colors;
} Grid;

// public
Grid* gridConstructor(int row, int col);
void gridInitialize(Grid*);
void gridDestory(Grid*);
void gridPrint(Grid*);
bool gridIsCellOutside (Grid*, int, int);
bool gridIsCellEmpty (Grid*, int, int);
int gridClearFullRows(Grid*);
extern bool debug_mode;

// private
bool gridIsRowFull (Grid*, int);
void gridClearRow (Grid*, int);
void gridMoveRowDown (Grid*, int, int);

#ifdef __cplusplus
}
#endif

#endif // GRID_H
