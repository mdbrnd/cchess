#include "include/raylib.h"
#include <stdio.h>

typedef enum
{
    Empty = 0,
    BlackPawn = 1,
    BlackRook = 2,
    BlackKnight = 3,
    BlackBishop = 4,
    BlackQueen = 5,
    BlackKing = 6,
    WhitePawn = 7,
    WhiteRook = 8,
    WhiteKnight = 9,
    WhiteBishop = 10,
    WhiteQueen = 11,
    WhiteKing = 12,
} piece_type;

typedef struct
{
    piece_type board[8][8];
    int current_turn;
} game;

game init_game()
{
    game game;

    game.current_turn = 0; // White starts

    // Init board with empty spaces
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            game.board[i][j] = Empty;
        }

    }

    // Set up Black pieces
    game.board[0][0] = BlackRook;
    game.board[0][1] = BlackKnight;
    game.board[0][2] = BlackBishop;
    game.board[0][3] = BlackQueen;
    game.board[0][4] = BlackKing;
    game.board[0][5] = BlackBishop;
    game.board[0][6] = BlackKnight;
    game.board[0][7] = BlackRook;

    for (int j = 0; j < 8; j++) // Black pawns
    {
        game.board[1][j] = BlackPawn;
    }

    // Set up White pieces
    game.board[7][0] = WhiteRook;
    game.board[7][1] = WhiteKnight;
    game.board[7][2] = WhiteBishop;
    game.board[7][3] = WhiteQueen;
    game.board[7][4] = WhiteKing;
    game.board[7][5] = WhiteBishop;
    game.board[7][6] = WhiteKnight;
    game.board[7][7] = WhiteRook;

    for (int j = 0; j < 8; j++) // White pawns
    {
        game.board[6][j] = WhitePawn;
    }

    return game;
}

int main()
{
    const int CELL_SIZE = 160;
    const int BOARD_LABEL_ROW_WIDTH = 50;
    const int PADDING = BOARD_LABEL_ROW_WIDTH / 3;

    const int SCREEN_WIDTH = CELL_SIZE * 8 + BOARD_LABEL_ROW_WIDTH;
    const int SCREEN_HEIGHT = CELL_SIZE * 8 + BOARD_LABEL_ROW_WIDTH;
    const Color BG_COLOR_1 = {150, 77, 34, 255};
    const Color BG_COLOR_2 = {238, 220, 151, 255};
    const Color SIDEBAR_COLOR = {21, 10, 4, 255};

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess");
    InitAudioDevice();
    SetTargetFPS(60);

    game game = init_game();

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(SIDEBAR_COLOR);

        // Draw board
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                if ((y + x) % 2 == 0)
                {
                    DrawRectangle(BOARD_LABEL_ROW_WIDTH + x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BG_COLOR_1);
                }
                else
                {
                    DrawRectangle(BOARD_LABEL_ROW_WIDTH + x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BG_COLOR_2);
                }
            }
        }

        // Draw numbers
        for (int i = 0; i < 8; i++)
        {
            char num[2]; // 1 byte for char itself, 1 for string termination char
            snprintf(num, sizeof(num), "%d", 8 - i);
            DrawText(num, PADDING, CELL_SIZE / 2 + i * CELL_SIZE, 30, WHITE);
        }

        // Draw letters
        for (int i = 0; i < 8; i++)
        {
            char letter[2];
            snprintf(letter, sizeof(letter), "%c", 'A' + i);
            DrawText(letter, BOARD_LABEL_ROW_WIDTH + CELL_SIZE / 2 + i * CELL_SIZE, SCREEN_HEIGHT - 2.3 * PADDING, 30, WHITE);
        }

        // Draw pieces
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                switch (game.board[i][j])
                {
                case Empty:
                    break;
                case BlackPawn:
                    break;

                case BlackRook:
                    break;

                case BlackKnight:
                    break;

                case BlackBishop:
                    break;

                case BlackQueen:
                    break;

                case BlackKing:
                    break;

                case WhitePawn:
                    break;

                case WhiteRook:
                    break;

                case WhiteKnight:
                    break;

                case WhiteBishop:
                    break;

                case WhiteQueen:
                    break;

                case WhiteKing:
                    break;
                default:
                    break;
                }
            }

        }


        EndDrawing();
    }

    CloseWindow();
    return 0;
}