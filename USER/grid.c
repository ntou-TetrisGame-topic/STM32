#include "grid.h"
#include "color.h"
#include "display.h"

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

// grid.c 裡面的 gridPrint
void gridPrint(Grid *g)
{
    int i, j, next_block_offset_col, idx, logic_j;
    uint32_t color;
    uint8_t r, g_color, b; /// 使用 g_color 避免和 Grid *g 變數名搞混

    for (i = 0; i < g->numRows; i++)
    {
        for (j = 0; j < g->numCols; j++)
        {
            // 優化：決定我們要從 grid 的哪一列拿資料
            logic_j = (i % 2 == 0) ? j : (g->numCols - 1 - j);

            if (i < 4 && logic_j < 16 && logic_j > 11) {
                color = g->nextBlockGrid[i][logic_j - 12];
            }else {
                color = g->grid[i][logic_j];
            }

             // 提取顏色 (ARGB -> RGB)
            r = ((color >> 16) & 0xFF)/4;
            g_color = ((color >> 8) & 0xFF)/4;
            b = (color & 0xFF)/4;

            // 這裡的 j 永遠是 0 -> numCols-1 (物理掃描順序)
            write_to_map(LED_MAP, i, j, r, g_color, b);
        }
    }

    // 繪製 Next Block (放在主遊戲區右側，這裡假設主區寬度 < 12，往右偏移)
    // next_block_offset_col = g->numCols + 2; // 中間空兩格
    // for (i = 0; i < 4; i++)
    // {
    //     for (j = 0; j < 4; j++)
    //     {
    //         color = g->nextBlockGrid[i][j];

    //         r =       (color >> 16) & 0xFF;
    //         g_color = (color >> 8)  & 0xFF;
    //         b =        color        & 0xFF;

    //         // r /= 4; g_color /= 4; b /= 4;

    //         // 寫入 LED_MAP，行數(i)可加上偏移量讓它往下放，列數(j)加上 offset 往右放
    //         write_to_map(LED_MAP, i + 2, j + next_block_offset_col, r, g_color, b);
    //     }
    // }

    // 將整張更新後的 LED_MAP 送出到 FPGA  
	for (i = 0; i < 32; i++)
    {
        send_led_chunk(i, 16, LED_MAP);
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
