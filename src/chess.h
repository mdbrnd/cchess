#ifndef CHESS_H
#define CHESS_H

#define MAX_MOVES 1024

#include <stdbool.h>

typedef unsigned int uint;

typedef enum
{
    EMPTY = 0,
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
    WhiteKing = 12
} piece_type;

typedef enum
{
    CChessWhite,
    CChessBlack
} piece_color;

typedef enum
{
    InProgress,
    WhiteWon,
    BlackWon,
    Stalemate
} game_status;

extern const char *piece_strings[];

typedef struct
{
    int x_from;
    int y_from;
    int x_to;
    int y_to;
    piece_type origin_piece;
    piece_type destination_piece;
} move;

typedef struct
{
    move moves[MAX_MOVES];
    uint count;
} move_list;

typedef enum
{
    None,
    PieceCaptured,
    Castle
} move_result;

typedef struct
{
    piece_type board[8][8];
    piece_color current_turn;
    game_status status;
    move_list move_history;
} game;

game init_game();

void reset_game(game *game);

bool is_within_bounds(int x, int y);

move_list get_valid_moves(game *game, int x, int y);

move_result make_move(game *game, move move);

void undo_last_move(game *game);

piece_color get_piece_color(piece_type piece);

game_status check_game_over(game *game);

bool import_FEN(game *game, const char *fen);

void export_FEN(game *game, char *str_buffer);

#endif