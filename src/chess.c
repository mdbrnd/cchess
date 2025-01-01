#include "chess.h"

const char *piece_strings[] = {
    "empty",
    "black_pawn",
    "black_rook",
    "black_knight",
    "black_bishop",
    "black_queen",
    "black_king",
    "white_pawn",
    "white_rook",
    "white_knight",
    "white_bishop",
    "white_queen",
    "white_king"};

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

bool is_within_bounds(int x, int y)
{
    if (x >= 0 && y >= 0 && x < 8 && y < 8)
    {
        return true;
    }

    return false;
}

move_list get_valid_moves(game *game, int x, int y) {
    move_list m = { .moves = {}, .count = 2 };

    // First dummy move: move piece one square right
    m.moves[0] = (move){x, y, x, y + 1};

    // Second dummy move: move piece one square down
    m.moves[1] = (move){x, y, x + 1, y};

    return m;
}

bool is_valid_move(move move)
{
}

void make_move(game *game, move move)
{
}