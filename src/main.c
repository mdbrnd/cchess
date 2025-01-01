#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "chess.h"
#include "renderer.h"

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

    // Load textures
    Texture2D textures[12];
    for (int i = 0; i < 12; i++)
    {
        textures[i] = get_piece_texture(i + 1);
    }

    // Load sounds
    Sound moveSound = LoadSound("assets/move.wav");
    Sound captureSound = LoadSound("assets/capture.wav");
    Sound castleSound = LoadSound("assets/castle.wav");

    int selectedSquare = -1;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(SIDEBAR_COLOR);

        // Draw board
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                // Draw selected square
                if (selectedSquare != -1 &&
                    y == selectedSquare / 8 &&
                    x == selectedSquare % 8)
                {
                    Color highlightColor = (Color){255, 255, 0, 200}; // Semi-transparent yellow
                    DrawRectangle(
                        BOARD_LABEL_ROW_WIDTH + x * CELL_SIZE,
                        y * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE,
                        highlightColor);
                    continue;
                }

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
        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                if (game.board[row][col] == Empty)
                {
                    continue;
                }

                Texture2D texture = textures[game.board[row][col] - 1];

                Rectangle pieceRectangle = {
                    0,
                    0,
                    texture.width,
                    texture.height
                };

                Rectangle pieceDestRectangle = {
                    BOARD_LABEL_ROW_WIDTH + col * CELL_SIZE + CELL_SIZE / 2,
                    row * CELL_SIZE + CELL_SIZE / 2,
                    pieceRectangle.width,
                    pieceRectangle.height
                };

                Vector2 pieceCenter = {texture.width / 2, texture.height / 2};

                DrawTexturePro(texture, pieceRectangle, pieceDestRectangle, pieceCenter, 0, WHITE);
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 position = GetMousePosition();

            // Convert mouse position to board coordinates
            int col = (position.x - BOARD_LABEL_ROW_WIDTH) / CELL_SIZE;
            int row = position.y / CELL_SIZE;

            // Check if click is within board bounds
            if (col >= 0 && col < 8 && row >= 0 && row < 8)
            {
                selectedSquare = row * 8 + col;
            }
            else
            {
                selectedSquare = -1; // Deselect if clicking outside the board
            }
        }


        EndDrawing();
    }

    // Unload textures
    for (int i = 0; i < 12; i++)
    {
        UnloadTexture(textures[i]);
    }

    // Unload sounds
    UnloadSound(moveSound);
    UnloadSound(captureSound);
    UnloadSound(castleSound);

    CloseAudioDevice();

    CloseWindow();
    return 0;
}