#include <stdlib.h>
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

    game.current_turn = CChessWhite;
    game.status = InProgress;
    game.move_history = (move_list){.moves = {}, .count = 0};

    // Init board with empty spaces
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            game.board[i][j] = EMPTY;
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

void reset_game(game *game)
{
    game->current_turn = CChessWhite;
    game->status = InProgress;
    game->move_history = (move_list){.moves = {}, .count = 0};

    // Init board with empty spaces
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            game->board[i][j] = EMPTY;
        }
    }

    // Set up Black pieces
    game->board[0][0] = BlackRook;
    game->board[0][1] = BlackKnight;
    game->board[0][2] = BlackBishop;
    game->board[0][3] = BlackQueen;
    game->board[0][4] = BlackKing;
    game->board[0][5] = BlackBishop;
    game->board[0][6] = BlackKnight;
    game->board[0][7] = BlackRook;

    for (int j = 0; j < 8; j++) // Black pawns
    {
        game->board[1][j] = BlackPawn;
    }

    // Set up White pieces
    game->board[7][0] = WhiteRook;
    game->board[7][1] = WhiteKnight;
    game->board[7][2] = WhiteBishop;
    game->board[7][3] = WhiteQueen;
    game->board[7][4] = WhiteKing;
    game->board[7][5] = WhiteBishop;
    game->board[7][6] = WhiteKnight;
    game->board[7][7] = WhiteRook;

    for (int j = 0; j < 8; j++) // White pawns
    {
        game->board[6][j] = WhitePawn;
    }
}

bool is_within_bounds(int x, int y)
{
    if (x >= 0 && y >= 0 && x < 8 && y < 8)
    {
        return true;
    }

    return false;
}

// TODO: castling
// TODO: en passant
// TODO: check_checkmate()
// TODO: FEN stuff
move_list get_valid_moves(game *game, int x, int y)
{
    move_list m = {.moves = {}, .count = 0};
    piece_type moving_piece = game->board[y][x];
    piece_color piece_color = get_piece_color(moving_piece);

    // Don't return moves for empty squares or pieces of wrong color
    if (moving_piece == EMPTY || piece_color != game->current_turn)
    {
        return m;
    }

    switch (moving_piece)
    {
    case WhitePawn:
    case BlackPawn:
    {
        int direction = (moving_piece == WhitePawn) ? -1 : 1;
        int start_row = (moving_piece == WhitePawn) ? 6 : 1;

        // Forward one square
        if (is_within_bounds(x, y + direction) && game->board[y + direction][x] == EMPTY)
        {
            m.moves[m.count++] = (move){x, y, x, y + direction};

            // Initial two-square move
            if (y == start_row && game->board[y + 2 * direction][x] == EMPTY)
            {
                m.moves[m.count++] = (move){x, y, x, y + 2 * direction};
            }
        }

        // Capture diagonally
        for (int dx = -1; dx <= 1; dx += 2)
        {
            if (is_within_bounds(x + dx, y + direction))
            {
                piece_type target = game->board[y + direction][x + dx];
                if (target != EMPTY && piece_color != get_piece_color(target))
                {
                    m.moves[m.count++] = (move){x, y, x + dx, y + direction};
                }
            }
        }
    }
    break;

    case WhiteBishop:
    case BlackBishop:
    {
        int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (int d = 0; d < 4; d++)
        {
            int dx = directions[d][0], dy = directions[d][1];
            int new_x = x + dx, new_y = y + dy;

            while (is_within_bounds(new_x, new_y))
            {
                piece_type target = game->board[new_y][new_x];
                if (target == EMPTY)
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y};
                }
                else if (piece_color != get_piece_color(target))
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y};
                    break;
                }
                else
                {
                    break;
                }
                new_x += dx;
                new_y += dy;
            }
        }
    }
    break;

    case WhiteKnight:
    case BlackKnight:
    {
        int moves[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
        for (int i = 0; i < 8; i++)
        {
            int new_x = x + moves[i][0];
            int new_y = y + moves[i][1];

            if (is_within_bounds(new_x, new_y))
            {
                piece_type target = game->board[new_y][new_x];
                if (target == EMPTY || piece_color != get_piece_color(target))
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y};
                }
            }
        }
    }
    break;

    case WhiteRook:
    case BlackRook:
    {
        int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
        for (int d = 0; d < 4; d++)
        {
            int dx = directions[d][0], dy = directions[d][1];
            int new_x = x + dx, new_y = y + dy;

            while (is_within_bounds(new_x, new_y))
            {
                piece_type target = game->board[new_y][new_x];
                if (target == EMPTY)
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y};
                }
                else if (piece_color != get_piece_color(target))
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y};
                    break;
                }
                else
                {
                    break;
                }
                new_x += dx;
                new_y += dy;
            }
        }
    }
    break;

    case WhiteQueen:
    case BlackQueen:
    {
        int directions[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (int d = 0; d < 8; d++)
        {
            int dx = directions[d][0], dy = directions[d][1];
            int new_x = x + dx, new_y = y + dy;

            while (is_within_bounds(new_x, new_y))
            {
                piece_type target = game->board[new_y][new_x];
                if (target == EMPTY)
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y};
                }
                else if (piece_color != get_piece_color(target))
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y};
                    break;
                }
                else
                {
                    break;
                }
                new_x += dx;
                new_y += dy;
            }
        }
    }
    break;

    case WhiteKing:
    case BlackKing:
    {
        int directions[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (int d = 0; d < 8; d++)
        {
            int new_x = x + directions[d][0];
            int new_y = y + directions[d][1];

            if (is_within_bounds(new_x, new_y))
            {
                piece_type target = game->board[new_y][new_x];
                if (target == EMPTY || piece_color != get_piece_color(target))
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y};
                }
            }
        }
    }
    break;

    default:
        break;
    }

    for (uint i = 0; i < m.count; i++)
    {
        m.moves[i].origin_piece = moving_piece;
        m.moves[i].destination_piece = game->board[m.moves[i].y_to][m.moves[i].x_to];
    }

    return m;
}

piece_color get_piece_color(piece_type piece)
{
    if (piece >= 1 && piece <= 6)
    {
        return CChessBlack;
    }

    return CChessWhite;
}

void make_move(game *game, move move)
{
    // TODO: handle overflow/wrap around
    game->move_history.moves[game->move_history.count] = move;
    game->move_history.count++;

    // Flip turn
    if (game->current_turn == CChessWhite)
    {
        game->current_turn = CChessBlack;
    }
    else
    {
        game->current_turn = CChessWhite;
    }

    piece_type moving_piece = game->board[move.y_from][move.x_from];
    game->board[move.y_from][move.x_from] = EMPTY;

    game->board[move.y_to][move.x_to] = moving_piece;
}

game_status check_game_over(game *game)
{
    // Check if white has a king. If no, then black won and vice versa
    bool has_wk = false;
    bool has_bk = false;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (game->board[i][j] == WhiteKing)
            {
                has_wk = true;
            }
            else if (game->board[i][j] == BlackKing)
            {
                has_bk = true;
            }
        }
    }

    if (has_wk && !has_bk)
    {
        return WhiteWon;
    }

    if (has_bk && !has_wk)
    {
        return BlackWon;
    }

    return InProgress;
}