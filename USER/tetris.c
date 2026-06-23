#include "tetris.h"
#include "sys.h"
#include "detectKey.h"
#include "game.h"
#include "grid.h"
#include "block.h"
#include "color.h"
#include "usart.h"
#include "time.h"
#include "beep.h"
#include "key.h"
#include "stdbool.h"
#include "exti.h"
#include <stdlib.h>

bool timer3IRQ1secfg = false;

/* main.c 裡面的函式：負責組成 19 碼 ASCII 字串並用 USART2 傳給 ESP32 */
extern void ESP32_SendGameData(u8 next_block[16], u8 add_score, u8 reset);

/* 清空 4x4 next block 資料 */
void ESP32_ClearNextBlock(u8 next_block[16])
{
    int i;
    for (i = 0; i < 16; i++)
    {
        next_block[i] = 0;
    }
}

/* 比較兩個 4x4 next block 是否相同 */
bool ESP32_IsSameNextBlock(u8 a[16], u8 b[16])
{
    int i;
    for (i = 0; i < 16; i++)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

/* 複製 4x4 next block */
void ESP32_CopyNextBlock(u8 dst[16], u8 src[16])
{
    int i;
    for (i = 0; i < 16; i++)
    {
        dst[i] = src[i];
    }
}

/*
 * 從 game->nextBlock 轉成 ESP32 網頁需要的 16 格資料。
 * 格式：row-major，index = row * 4 + col
 * 0 代表空格，1~7 代表方塊顏色 id。
 */
void ESP32_BuildNextBlock4x4(Game *game, u8 next_block[16])
{
    int i;
    int old_row_offset;
    int old_col_offset;
    position *tiles;

    ESP32_ClearNextBlock(next_block);

    if (game == 0 || game->nextBlock == 0)
    {
        return;
    }

    /*
     * blockConstructor() 會讓不同方塊帶有 rowOffset/columnOffset。
     * 這裡要顯示 NEXT 4x4，所以先暫時歸零，做法和 blockDraw() 類似。
     */
    old_row_offset = game->nextBlock->rowOffset;
    old_col_offset = game->nextBlock->columnOffset;

    game->nextBlock->rowOffset = 0;
    game->nextBlock->columnOffset = 0;

    tiles = GetCellPositions(game->nextBlock);

    for (i = 0; i < MAX_CELLS_PER_BLOCK; i++)
    {
        int row = tiles[i].row;
        int col = tiles[i].column;

        if ((row >= 0) && (row < 4) && (col >= 0) && (col < 4))
        {
            next_block[row * 4 + col] = game->nextBlock->id;
        }
    }

    free(tiles);

    game->nextBlock->rowOffset = old_row_offset;
    game->nextBlock->columnOffset = old_col_offset;
}

/*
 * 將目前遊戲狀態送到 ESP32。
 * 因為 ESP32 目前的格式只有兩位數分數，所以這裡送的是「分數增量」。
 * 若一次加超過 99 分，會拆成多筆，例如 100 分 -> 99 + 1。
 */
void ESP32_SendStateIfChanged(Game *game, u32 *last_score, u8 last_next_block[16], bool force_send)
{
    u8 now_next_block[16];
    u32 score_delta;
    bool next_changed;

    if (game == 0)
    {
        return;
    }

    ESP32_BuildNextBlock4x4(game, now_next_block);
    next_changed = !ESP32_IsSameNextBlock(now_next_block, last_next_block);

    if (game->score >= *last_score)
    {
        score_delta = game->score - *last_score;
    }
    else
    {
        score_delta = 0;
    }

    if (force_send || next_changed || score_delta > 0)
    {
        if (score_delta == 0)
        {
            ESP32_SendGameData(now_next_block, 0, 0);
        }
        else
        {
            while (score_delta > 99)
            {
                ESP32_SendGameData(now_next_block, 99, 0);
                score_delta -= 99;
            }
            ESP32_SendGameData(now_next_block, (u8)score_delta, 0);
        }

        *last_score = game->score;
        ESP32_CopyNextBlock(last_next_block, now_next_block);
    }
}

/* 傳 reset 給 ESP32：ESP32 收到最後一碼 1 後會清空分數與 NEXT */
void ESP32_SendResetFrame(void)
{
    u8 empty_next[16];
    ESP32_ClearNextBlock(empty_next);
    ESP32_SendGameData(empty_next, 0, 1);
}

int tetris() {
    // 是否已經輸出過遊戲結束訊息
    bool output_result = false;
    u32 esp32_last_score = 0;
    u8 esp32_last_next_block[16];
    bool esp32_force_send = true;
    int final_score;
    
    // 初始化保存結果
    tetrisOutput *tetrisAns = tertisOutputConstructor(gridRow, gridColumn, nextBlockRow, nextBlockCol);
    // 遊戲初始化
    Game *game = gameConstructor(gridRow, gridColumn);
    // 輸入長度
    int key;

    ESP32_ClearNextBlock(esp32_last_next_block);

    if (debug_mode) {
        printf("control (Case insensitive):\n");
        printf("a: left\n");    // botton 0
        printf("s: down\n");    // botton 1
        printf("d: right\n");   // botton 2
        printf("w, space: spin\n");     // botton wake up
        printf("t: pause/continue\n");  // 't'
        printf("Q: quit\n");            // 'q'
    }

    // reset timer 3, for block go down auto
    timer3IRQ1secfg = false;
    while (true) {
        gameDraw(game, tetrisAns->record);

        // 每次畫面更新後，把 NEXT 與 SCORE 傳給 ESP32
        ESP32_SendStateIfChanged(game, &esp32_last_score, esp32_last_next_block, esp32_force_send);
        esp32_force_send = false;

        // go down auto after each second
        if (timer3IRQ1secfg == true) {
            if ((game->gameOver == false) && (game->gameStop == false)) {
                gameMoveBlockDown(game);
                timer3IRQ1secfg = false;
            }
        }
        // 指令觸發中斷
        if(cmd_ready == true) { 
            
            key = current_cmd;   // 把中斷收到的指令拿出來
            cmd_ready = false;   // 立刻清除 Flag，準備迎接下一次按鍵中斷

            if (game->gameOver == false) {
                if ((key == 'Q') || (key == 'q')) {
                    if (debug_mode) printf("exit\r\n");
                    break;
                }
            }
            
            if ((key == 'T') || (key == 't')) {
                game->gameStop = !game->gameStop;
                if (game->gameStop == false) {
                    if (debug_mode) printf("continue\r\n");
                } else if (game->gameStop == true) {
                    if (debug_mode) printf("pause\r\n");
                }
            } else if ((game->gameOver == false) && (key != KEY_NULL)) {
                gameHandleInput(game, key);
                
                if (key == 'S' || key == 's') {
                    timer3IRQ1secfg = false;
                }
            }
            
            if (game->gameOver == true) {
                if ((key == 'Q') || (key == 'q')) {
                    if (debug_mode) printf("exit (loose out)\r\n");
                    break; 
                } else if (key != KEY_NULL) {
                    game->gameOver = false;
                    output_result = false;
                    gameReset(game);

                    // 通知 ESP32 reset，並讓下一圈重新送新的 NEXT
                    ESP32_SendResetFrame();
                    esp32_last_score = 0;
                    ESP32_ClearNextBlock(esp32_last_next_block);
                    esp32_force_send = true;
                }
            }
        }

        if (game->gameOver == true) {
            if (!output_result) {
                if (debug_mode) printf("You are lose!\r\n");
                output_result = true;
            }
        }
        // 保存最高分數
        // tertisOutputPrint(tetrisAns);
    }

    final_score = game->score;
    gameDestructor(game);
    tertisOutputDestroy(tetrisAns);
    return final_score;
}

tetrisOutput* tertisOutputConstructor(int graphRow, int graphCol, int ntBlockRow, int ntBlockCol) {
    tetrisOutput *t = (tetrisOutput*)malloc(sizeof(tetrisOutput));
    t->colors = GetCellColors();

    t->record = gridConstructor(gridRow, gridColumn);

    t->score = 0;
    return t;
}

void tertisOutputDestroy(tetrisOutput *t) {
    gridDestory(t->record);
}

void tertisOutputCopyAns(Grid *g, Grid *t) {
    int i, j;
    for(i = 0; i<gridRow; i++) {
        for(j = 0; j<gridColumn; j++) {
            t->grid[i][j] = g->grid[i][j];
        }
    }

    for(i = 0; i<nextBlockRow; i++) {
        for(j = 0; j<nextBlockCol; j++) {
            t->nextBlockGrid[i][j] = g->nextBlockGrid[i][j];
        }
    }
}

// 接出Tetris輸出
void tertisOutputPrint(tetrisOutput* t) {
    gridPrint(t->record);
    if (debug_mode) printf("Score: %d\n", t->score);
}
