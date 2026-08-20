#include "grid.h"
#include "color.h"
#include "display.h"
#include "fsmc.h"

#include <stdlib.h>

Grid *gridConstructor(int rows, int cols)
{
    Grid *g = (Grid *)malloc(sizeof(Grid));
    g->numRows = rows;
    g->numCols = cols;
    g->colors = GetCellColors();
    gridInitialize(g);

    return g;
}

////輔助terminal觀看 (沒有任何用處)////////////
int turnHexToInt(Grid *g, uint32_t sor)
{
    int index;
    for (index = 0; index < 8; index++)
    {
        if (g->colors[index] == sor)
        {
            return index;
        }
    }
    return 1111;
}
///////////////////////////////////////////////

void gridInitialize(Grid *g)
{
    int i, j, row, column;
    // 宣告空間
    g->grid = (uint32_t **)malloc(g->numRows * sizeof(uint32_t *));
    for (i = 0; i < g->numRows; i++)
    {
        g->grid[i] = (uint32_t *)malloc(g->numCols * sizeof(uint32_t));
    }
    g->nextBlockGrid = (uint32_t **)malloc(nextBlockRow * sizeof(uint32_t *));
    for (i = 0; i < nextBlockRow; i++)
    {
        g->nextBlockGrid[i] = (uint32_t *)malloc(nextBlockCol * sizeof(uint32_t));
    }

    // 初始為color[0]，即背景顏色
    for (row = 0; row < g->numRows; row++)
    {
        for (column = 0; column < g->numCols; column++)
        {
            g->grid[row][column] = g->colors[0];
        }
    }
    for (i = 0; i < nextBlockRow; i++)
    {
        for (j = 0; j < nextBlockCol; j++)
        {
            g->nextBlockGrid[i][j] = g->colors[0];
        }
    }
}

void gridDestory(Grid *g)
{
    int i;
    for (i = 0; i < g->numRows; i++)
    {
        free(g->grid[i]);
    }
    free(g->grid);
    for (i = 0; i < nextBlockRow; i++)
    {
        free(g->nextBlockGrid[i]);
    }
    free(g->nextBlockGrid);
    free(g);
}

// grid.c 裡的 gridPrint
void gridPrint(Grid *g)
{
    int i, j;
    uint32_t color;
    uint8_t r, g_color, b;
    uint16_t led_index;

    const int COLS_PER_ROW = 16;

    for (i = 0; i < g->numRows; i++)
    {
        for (j = 0; j < g->numCols; j++)
        {
            // 1. 直接按正常陣列順序 (左到右) 取用顏色，不需處理 logic_j
            if (i < 4 && j >= 12 && j < 16)
            {
                color = g->nextBlockGrid[i][j - 12];
            }
            else
            {
                color = g->grid[i][j];
            }

            // 2. 提取 ARGB -> RGB (調整亮度)
            r = ((color >> 16) & 0xFF) / 4;
            g_color = ((color >> 8) & 0xFF) / 4;
            b = (color & 0xFF) / 4;

            // 3. 計算物理燈珠的蛇形位址 (Serpentine Layout)
            // 偶數列 (0, 2, 4...): 正向 (0 -> 15)
            // 奇數列 (1, 3, 5...): 反向 (15 -> 0)
            if (i % 2 == 0)
            {
                led_index = (i * COLS_PER_ROW) + j;
            }
            else
            {
                led_index = (i * COLS_PER_ROW) + (COLS_PER_ROW - 1 - j);
            }

            // 4. 寫入 FSMC 記憶體
            LED_RAM[led_index].g = g_color;
            LED_RAM[led_index].r = r;
            LED_RAM[led_index].b = b;
        }
    }
}

bool gridIsCellOutside(Grid *g, int row, int column)
{
    if ((row >= 0) && (row < (g->numRows)) && (column >= 0) && (column < (g->numCols)))
    {
        return false;
    }
    return true;
}

bool gridIsCellEmpty(Grid *g, int row, int column)
{
    if (g->grid[row][column] == g->colors[0])
    {
        return true;
    }
    return false;
}

int gridClearFullRows(Grid *g)
{
    int row;
    int completed = 0;
    for (row = g->numRows - 1; row >= 0; row--)
    {
        if (gridIsRowFull(g, row))
        {
            gridClearRow(g, row);
            completed++;
        }
        else if (completed > 0)
        {
            gridMoveRowDown(g, row, completed);
        }
    }
    return completed;
}

bool gridIsRowFull(Grid *g, int row)
{
    int column;
    for (column = 0; column < g->numCols; column++)
    {
        if (g->grid[row][column] == g->colors[0])
        {
            return false;
        }
    }
    return true;
};

void gridClearRow(Grid *g, int row)
{
    int column;
    for (column = 0; column < g->numCols; column++)
    {
        g->grid[row][column] = g->colors[0];
    }
}

void gridMoveRowDown(Grid *g, int row, int numRows)
{
    int column;
    for (column = 0; column < g->numCols; column++)
    {
        g->grid[row + numRows][column] = g->grid[row][column];
        g->grid[row][column] = g->colors[0];
    }
}
