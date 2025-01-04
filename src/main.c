#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "chess.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void get_piece_path(piece_type piece, char *path, size_t size);
Texture2D get_piece_texture(piece_type piece);

const int CELL_SIZE = 110;
const int BOARD_LABEL_WIDTH = 50; // Area where numbers and letters are (1-8, A-H)
const int PADDING = BOARD_LABEL_WIDTH / 3;
const int FONT_SIZE = CELL_SIZE / 5;
const int GAME_OVER_TIME = 3000;
const int MENU_BAR_HEIGHT = 60;
const int SCREEN_WIDTH = CELL_SIZE * 8 + BOARD_LABEL_WIDTH;
const int SCREEN_HEIGHT = MENU_BAR_HEIGHT + CELL_SIZE * 8 + BOARD_LABEL_WIDTH;
const int BUTTON_HEIGHT = 60;
const int BUTTON_WIDTH = SCREEN_WIDTH / 4;
const int ICON_BUTTON_HEIGHT = 60;
const int ICON_BUTTON_WIDTH = SCREEN_WIDTH / 4 + 5;
const int NOTIFICATION_DURATION = 2000; // Duration in milliseconds

const Color CELL_COLOR_1 = {150, 77, 34, 255};
const Color CELL_COLOR_2 = {238, 220, 151, 255};
const Color SIDEBAR_COLOR = {21, 10, 4, 255};
const Color HIGHLIGHT_COLOR = {255, 255, 0, 200};
const Color TEXT_BACKGROUND = {0, 0, 0, 200};

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess");
    InitAudioDevice();
    SetTargetFPS(60);

    game g = init_game();

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
    move_list validMoves = {.moves = {}, .count = 0};

    double gameOverTimer = 0;
    double notificationTimer = 0;
    bool showNotification = false;
    const char *notificationText = NULL;

    bool showFenDialog = false;
    bool isExportMode = false;
    char fenString[100] = ""; // Buffer for FEN string

    const char *promotionLabels[] = {"Queen", "Rook", "Bishop", "Knight"};
    piece_type promotionChoicesWhite[] = {WhiteQueen, WhiteRook, WhiteBishop, WhiteKnight};
    piece_type promotionChoicesBlack[] = {BlackQueen, BlackRook, BlackBishop, BlackKnight};
    bool showPromotionDialog = false;
    piece_color promotionColor = CChessWhite;
    int promotionX = -1;
    int promotionY = -1;
    int selectedPromotionOption = -1;

    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);

    while (!WindowShouldClose())
    {
        if (g.status == WhiteWon)
        {
            SetWindowTitle("Chess - White Won!");
        }
        else if (g.status == BlackWon)
        {
            SetWindowTitle("Chess - Black Won!");
        }
        else if (g.status == Stalemate)
        {
            SetWindowTitle("Chess - Draw!");
        }

        else if (g.current_turn == CChessWhite)
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
                        MENU_BAR_HEIGHT + y * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE,
                        HIGHLIGHT_COLOR);

                    continue;
                }

                if ((y + x) % 2 == 0)
                {
                    DrawRectangle(BOARD_LABEL_WIDTH + x * CELL_SIZE, MENU_BAR_HEIGHT + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, CELL_COLOR_1);
                }
                else
                {
                    DrawRectangle(BOARD_LABEL_WIDTH + x * CELL_SIZE, MENU_BAR_HEIGHT + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, CELL_COLOR_2);
                }
            }
        }

        // Draw numbers
        for (int i = 0; i < 8; i++)
        {
            char num[2]; // 1 byte for char itself, 1 for string termination char
            snprintf(num, sizeof(num), "%d", 8 - i);
            DrawText(num, PADDING, MENU_BAR_HEIGHT + i * CELL_SIZE + CELL_SIZE / 2, FONT_SIZE, WHITE);
        }

        // Draw letters
        for (int i = 0; i < 8; i++)
        {
            char letter[2];
            snprintf(letter, sizeof(letter), "%c", 'A' + i);
            DrawText(letter, BOARD_LABEL_WIDTH + CELL_SIZE / 2 + i * CELL_SIZE - 10, SCREEN_HEIGHT - 2.3 * PADDING, FONT_SIZE, WHITE);
        }

        // Draw pieces
        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                if (g.board[row][col] == EMPTY)
                {
                    continue;
                }

                Texture2D texture = textures[g.board[row][col] - 1];

                Rectangle pieceRectangle = {
                    0,
                    0,
                    texture.width,
                    texture.height};

                Rectangle pieceDestRectangle = {
                    BOARD_LABEL_WIDTH + col * CELL_SIZE + CELL_SIZE / 2,
                    MENU_BAR_HEIGHT + row * CELL_SIZE + CELL_SIZE / 2,
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

            validMoves = get_valid_moves(&g, x, y);

            for (uint i = 0; i < validMoves.count; i++)
            {
                int circleX = BOARD_LABEL_WIDTH + (validMoves.moves[i].x_to * CELL_SIZE) + (CELL_SIZE / 2);
                int circleY = MENU_BAR_HEIGHT + validMoves.moves[i].y_to * CELL_SIZE + (CELL_SIZE / 2);
                DrawCircle(circleX, circleY, CELL_SIZE / 8, HIGHLIGHT_COLOR);
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && g.status == InProgress && !showPromotionDialog)
        {
            Vector2 position = GetMousePosition();
            int x_clicked = (position.x - BOARD_LABEL_WIDTH) / CELL_SIZE;
            int y_clicked = (position.y - MENU_BAR_HEIGHT) / CELL_SIZE;

            if (is_within_bounds(x_clicked, y_clicked) && position.x >= BOARD_LABEL_WIDTH && position.y >= MENU_BAR_HEIGHT && position.y <= SCREEN_HEIGHT - BOARD_LABEL_WIDTH)
            {
                // First check if we're trying to complete a move
                if (selectedSquare != -1)
                {
                    bool move_made = false;
                    for (uint i = 0; i < validMoves.count; i++)
                    {
                        move m = validMoves.moves[i];
                        if (m.x_to == x_clicked && m.y_to == y_clicked)
                        {
                            promotionColor = g.current_turn;

                            move_result result = make_move(&g, m);
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
                            else if (result == Promotion)
                            {
                                // Store the location of the pawn that needs promotion
                                promotionX = m.x_to;
                                promotionY = m.y_to;
                                showPromotionDialog = true;

                                PlaySound(moveSound);
                            }

                            selectedSquare = -1;
                            move_made = true;

                            game_status status = check_game_over(&g);
                            if (status == WhiteWon)
                            {
                                g.status = WhiteWon;
                                gameOverTimer = GetTime();
                                printf("White Won!\n");
                            }
                            else if (status == BlackWon)
                            {
                                g.status = BlackWon;
                                gameOverTimer = GetTime();
                                printf("Black Won!\n");
                            }
                            break;
                        }
                    }

                    // If no move was made, try selecting new piece
                    if (!move_made)
                    {
                        piece_type clicked_piece = g.board[y_clicked][x_clicked];
                        piece_color piece_color = get_piece_color(clicked_piece);

                        if (clicked_piece != EMPTY && piece_color == g.current_turn)
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
                    piece_type clicked_piece = g.board[y_clicked][x_clicked];
                    piece_color piece_color = get_piece_color(clicked_piece);

                    if (clicked_piece != EMPTY && piece_color == g.current_turn)
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

            printf("Piece on that square: %d\n", g.board[y_clicked][x_clicked]);
        }

        // Draw game over message
        if (g.status == WhiteWon || g.status == BlackWon)
        {
            if ((GetTime() - gameOverTimer) * 1000 > GAME_OVER_TIME)
            {
                reset_game(&g);
            }
            else
            {
                const char *winner_text = "White Wins!";
                if (g.status == BlackWon)
                {
                    winner_text = "Black Wins!";
                }
                else if (g.status == Stalemate)
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

        // Draw GUI in menu bar
        Rectangle resetBtn = (Rectangle){0, 0, BUTTON_WIDTH, BUTTON_HEIGHT};
        if (GuiButton(resetBtn, "Reset Board") && !showPromotionDialog)
        {
            reset_game(&g);
            selectedSquare = -1;
        }

        Rectangle importBtn = (Rectangle){resetBtn.x + resetBtn.width,
                                          0, BUTTON_WIDTH, BUTTON_HEIGHT};
        if (GuiButton(importBtn, "Import FEN") && !showPromotionDialog)
        {
            showFenDialog = true;
            isExportMode = false;
            memset(fenString, 0, sizeof(fenString));
        }

        Rectangle exportBtn = (Rectangle){importBtn.x + importBtn.width,
                                          0, BUTTON_WIDTH, BUTTON_HEIGHT};
        if (GuiButton(exportBtn, "Export FEN") && !showPromotionDialog)
        {
            export_FEN(&g, fenString);
            SetClipboardText(fenString);
            notificationText = "FEN copied to clipboard!";
            notificationTimer = GetTime();
            showNotification = true;
        }

        Rectangle undoBtn = (Rectangle){exportBtn.x + exportBtn.width,
                                        0, ICON_BUTTON_WIDTH, ICON_BUTTON_HEIGHT};
        if (GuiButton(undoBtn, "<- Undo Move") && !showPromotionDialog)
        {
            undo_last_move(&g);
            selectedSquare = -1;
        }

        // Rectangle redoBtn = (Rectangle){undoBtn.x + undoBtn.width,
        //                                 0, ICON_BUTTON_WIDTH, ICON_BUTTON_HEIGHT};
        // if (GuiButton(redoBtn, ">"))
        // {
        //     undo_last_move(&game);
        //     selectedSquare = -1;
        // }

        if (showFenDialog && !showPromotionDialog)
        {
            Rectangle dialogBounds = (Rectangle){SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 50, 400, 100};

            // Handle paste (Ctrl+V)
            if (!isExportMode && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V))
            {
                const char *clipText = GetClipboardText();
                if (clipText != NULL)
                {
                    // Ensure we don't overflow the buffer
                    strncpy(fenString, clipText, sizeof(fenString) - 1);
                    fenString[sizeof(fenString) - 1] = '\0';
                }
            }

            // Draw the text box
            if (isExportMode)
            {
                GuiTextBox(dialogBounds, fenString, sizeof(fenString), false); // Read-only in export mode
            }
            else
            {
                GuiTextBox(dialogBounds, fenString, sizeof(fenString), true); // Editable in import mode
            }

            // Draw buttons
            Rectangle cancelBtn = (Rectangle){dialogBounds.x,
                                              dialogBounds.y + dialogBounds.height + 10,
                                              120, 30};
            Rectangle okBtn = (Rectangle){dialogBounds.x + dialogBounds.width - 120,
                                          dialogBounds.y + dialogBounds.height + 10,
                                          120, 30};

            if (GuiButton(cancelBtn, "Cancel") || IsKeyPressed(KEY_ESCAPE))
            {
                showFenDialog = false;
                isExportMode = false;
                memset(fenString, 0, sizeof(fenString));
            }

            if (GuiButton(okBtn, "OK") || IsKeyPressed(KEY_ENTER))
            {
                if (!isExportMode && strlen(fenString) > 0)
                {
                    game game_before_import = g; // Create backup copy
                    if (import_FEN(&g, fenString))
                    {
                        showFenDialog = false;
                    }
                    else
                    {
                        g = game_before_import; // Restore the backup if import fails
                        notificationText = "Invalid FEN string!";
                        notificationTimer = GetTime();
                        showNotification = true;
                    }
                }
                showFenDialog = false;
                isExportMode = false;
                memset(fenString, 0, sizeof(fenString));
            }
        }

        if (showNotification && !showPromotionDialog)
        {
            if ((GetTime() - notificationTimer) * 1000 > NOTIFICATION_DURATION)
            {
                showNotification = false;
            }
            else
            {
                int messageFontSize = FONT_SIZE;
                int textWidth = MeasureText(notificationText, messageFontSize);
                int textHeight = messageFontSize;
                int padding = 20;
                int boxWidth = textWidth + padding * 2;
                int boxHeight = textHeight + padding * 2;
                int boxX = SCREEN_WIDTH / 2 - boxWidth / 2;
                int boxY = MENU_BAR_HEIGHT + 10;

                DrawRectangle(boxX, boxY, boxWidth, boxHeight, TEXT_BACKGROUND);
                DrawText(notificationText,
                         boxX + padding,
                         boxY + padding,
                         messageFontSize,
                         WHITE);
            }
        }

        if (showPromotionDialog)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.4f));

            Rectangle dialogRect = {
                SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 100, 300, 200};

            GuiWindowBox(dialogRect, "Promote Pawn");

            float buttonWidth = 80;
            float buttonHeight = 30;
            float spacing = 10;

            float startX = dialogRect.x + (dialogRect.width - buttonWidth) / 2;
            float startY = dialogRect.y + 40;

            for (int i = 0; i < 4; i++)
            {
                Rectangle btnRect = {
                    startX,
                    startY + i * (buttonHeight + spacing),
                    buttonWidth,
                    buttonHeight};

                if (GuiButton(btnRect, promotionLabels[i]))
                {
                    selectedPromotionOption = i;
                }
            }

            if (selectedPromotionOption != -1)
            {
                if (promotionColor == CChessWhite)
                {
                    g.board[promotionY][promotionX] = promotionChoicesWhite[selectedPromotionOption];
                }
                else
                {
                    g.board[promotionY][promotionX] = promotionChoicesBlack[selectedPromotionOption];
                }

                showPromotionDialog = false;
                selectedPromotionOption = -1;
                promotionX = promotionY = -1;
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

void get_piece_path(piece_type piece, char *path, size_t size)
{
    snprintf(path, size, "assets/highres/%s.png", piece_strings[piece]);
}

// Don't forget to unload texture after game loop
Texture2D get_piece_texture(piece_type piece)
{
    char path[128];
    get_piece_path(piece, path, sizeof(path));
    Image image = LoadImage(path);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    return texture; // We don't have to use buffers and can just return it since the texture is stored on the GPU and not the stack
}