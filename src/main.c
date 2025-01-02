#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "chess.h"
#include "renderer.h"

int main()
{
    const int CELL_SIZE = 160;
    const int BOARD_LABEL_WIDTH = 50; // Area where numbers and letters are (1-8, A-H)
    const int PADDING = BOARD_LABEL_WIDTH / 3;
    const int FONT_SIZE = CELL_SIZE / 5;
    const int GAME_OVER_TIME = 3000;

    const int SCREEN_WIDTH = CELL_SIZE * 8 + BOARD_LABEL_WIDTH;
    const int SCREEN_HEIGHT = CELL_SIZE * 8 + BOARD_LABEL_WIDTH;
    const Color CELL_COLOR_1 = {150, 77, 34, 255};
    const Color CELL_COLOR_2 = {238, 220, 151, 255};
    const Color SIDEBAR_COLOR = {21, 10, 4, 255};
    const Color HIGHLIGHT_COLOR = {255, 255, 0, 200};
    const Color TEXT_BACKGROUND = {0, 0, 0, 200};

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
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
    move_list valid_moves = {.moves = {}, .count = 0};

    double game_over_timer = 0;

    while (!WindowShouldClose())
    {
        if (game.status == WhiteWon)
        {
            SetWindowTitle("Chess - White Won!");
        }
        else if (game.status == BlackWon)
        {
            SetWindowTitle("Chess - Black Won!");
        }
        else if (game.status == Stalemate)
        {
            SetWindowTitle("Chess - Draw!");
        }

        else if (game.current_turn == CChessWhite)
        {
            SetWindowTitle("Chess - White's Turn");
        }
        else
        {
            SetWindowTitle("Chess - Black's Turn");
        }

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
                    DrawRectangle(
                        BOARD_LABEL_WIDTH + x * CELL_SIZE,
                        y * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE,
                        HIGHLIGHT_COLOR);

                    continue;
                }

                if ((y + x) % 2 == 0)
                {
                    DrawRectangle(BOARD_LABEL_WIDTH + x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, CELL_COLOR_1);
                }
                else
                {
                    DrawRectangle(BOARD_LABEL_WIDTH + x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, CELL_COLOR_2);
                }
            }
        }

        // Draw numbers
        for (int i = 0; i < 8; i++)
        {
            char num[2]; // 1 byte for char itself, 1 for string termination char
            snprintf(num, sizeof(num), "%d", 8 - i);
            DrawText(num, PADDING, CELL_SIZE / 2 + i * CELL_SIZE, FONT_SIZE, WHITE);
        }

        // Draw letters
        for (int i = 0; i < 8; i++)
        {
            char letter[2];
            snprintf(letter, sizeof(letter), "%c", 'A' + i);
            DrawText(letter, BOARD_LABEL_WIDTH + CELL_SIZE / 2 + i * CELL_SIZE, SCREEN_HEIGHT - 2.3 * PADDING, FONT_SIZE, WHITE);
        }

        // Draw pieces
        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                if (game.board[row][col] == EMPTY)
                {
                    continue;
                }

                Texture2D texture = textures[game.board[row][col] - 1];

                Rectangle pieceRectangle = {
                    0,
                    0,
                    texture.width,
                    texture.height};

                Rectangle pieceDestRectangle = {
                    BOARD_LABEL_WIDTH + col * CELL_SIZE + CELL_SIZE / 2,
                    row * CELL_SIZE + CELL_SIZE / 2,
                    pieceRectangle.width,
                    pieceRectangle.height};

                Vector2 pieceCenter = {texture.width / 2, texture.height / 2};

                DrawTexturePro(texture, pieceRectangle, pieceDestRectangle, pieceCenter, 0, WHITE);
            }
        }

        // Draw possible moves
        if (selectedSquare != -1)
        {
            int x = selectedSquare % 8;
            int y = selectedSquare / 8;

            valid_moves = get_valid_moves(&game, x, y);

            for (uint i = 0; i < valid_moves.count; i++)
            {
                int circleX = BOARD_LABEL_WIDTH + (valid_moves.moves[i].x_to * CELL_SIZE) + (CELL_SIZE / 2);
                int circleY = valid_moves.moves[i].y_to * CELL_SIZE + (CELL_SIZE / 2);
                DrawCircle(circleX, circleY, CELL_SIZE / 8, HIGHLIGHT_COLOR);
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && game.status == InProgress)
        {
            Vector2 position = GetMousePosition();
            int x_clicked = (position.x - BOARD_LABEL_WIDTH) / CELL_SIZE;
            int y_clicked = position.y / CELL_SIZE;

            if (is_within_bounds(x_clicked, y_clicked))
            {
                // First check if we're trying to complete a move
                if (selectedSquare != -1)
                {
                    bool move_made = false;
                    for (uint i = 0; i < valid_moves.count; i++)
                    {
                        if (valid_moves.moves[i].x_to == x_clicked && valid_moves.moves[i].y_to == y_clicked)
                        {
                            move_result result = make_move(&game, valid_moves.moves[i]);
                            if (result == None)
                            {
                                PlaySound(moveSound);
                            }
                            else if (result == PieceCaptured)
                            {
                                PlaySound(captureSound);
                            }
                            else if (result == Castle)
                            {
                                PlaySound(castleSound);
                            }

                            selectedSquare = -1;
                            move_made = true;

                            game_status status = check_game_over(&game);
                            if (status == WhiteWon)
                            {
                                game.status = WhiteWon;
                                game_over_timer = GetTime();
                                printf("White Won!\n");
                            }
                            else if (status == BlackWon)
                            {
                                game.status = BlackWon;
                                game_over_timer = GetTime();
                                printf("Black Won!\n");
                            }
                            break;
                        }
                    }

                    // If no move was made, try selecting new piece
                    if (!move_made)
                    {
                        piece_type clicked_piece = game.board[y_clicked][x_clicked];
                        piece_color piece_color = get_piece_color(clicked_piece);

                        if (clicked_piece != EMPTY && piece_color == game.current_turn)
                        {
                            printf("Selected Piece. Piece Color: %d\n", piece_color);
                            selectedSquare = y_clicked * 8 + x_clicked;
                        }
                        else
                        {
                            selectedSquare = -1; // Deselect if clicking invalid square
                        }
                    }
                }
                else // No piece selected, try selecting one
                {
                    piece_type clicked_piece = game.board[y_clicked][x_clicked];
                    piece_color piece_color = get_piece_color(clicked_piece);

                    if (clicked_piece != EMPTY && piece_color == game.current_turn)
                    {
                        printf("Selected Piece. Piece Color: %d\n", piece_color);
                        selectedSquare = y_clicked * 8 + x_clicked;
                    }
                }
            }
            else
            {
                selectedSquare = -1; // Deselect if clicking outside the board
            }

            printf("Piece on that square: %d\n", game.board[y_clicked][x_clicked]);
        }

        // Draw game over message
        if (game.status == WhiteWon || game.status == BlackWon)
        {
            if ((GetTime() - game_over_timer) * 1000 > GAME_OVER_TIME)
            {
                reset_game(&game);
            }
            else
            {
                const char *winner_text = "White Wins!";
                if (game.status == BlackWon)
                {
                    winner_text = "Black Wins!";
                }
                else if (game.status == Stalemate)
                {
                    winner_text = "Draw!";
                }

                int messageFontSize = (int)(FONT_SIZE * 2.5f);
                int textWidth = MeasureText(winner_text, messageFontSize);
                int textHeight = messageFontSize;
                int padding = 40;
                int boxWidth = textWidth + padding * 2;
                int boxHeight = textHeight + padding * 2;
                int boxX = SCREEN_WIDTH / 2 - boxWidth / 2;
                int boxY = SCREEN_HEIGHT / 2 - boxHeight / 2;

                DrawRectangle(boxX, boxY, boxWidth, boxHeight, TEXT_BACKGROUND);

                int textX = SCREEN_WIDTH / 2 - textWidth / 2;
                int textY = SCREEN_HEIGHT / 2 - textHeight / 2;
                DrawText(winner_text, textX, textY, messageFontSize, WHITE);
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