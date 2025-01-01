#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>

typedef unsigned int uint;

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

extern const char *piece_strings[];

typedef struct
{
    int x_from;
    int y_from;
    int x_to;
    int y_to;
} move;

typedef struct
{
    piece_type board[8][8];
    uint current_turn;
    move move_history[100];
} game;

typedef struct
{
    move moves[64]; // Max amount of possible moves for a single piece (Queen)
    uint count;
} move_list;

game init_game();

bool is_within_bounds(int x, int y);

move_list get_valid_moves(game *game, int x, int y);

bool is_valid_move(move move);

void make_move(game *game, move move);

#endif