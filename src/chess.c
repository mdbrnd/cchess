#include <stdlib.h>
#include <stdio.h>
#include "chess.h"

static bool has_king_moved(game *game, piece_color color);
static bool has_rook_moved(game *game, piece_color color, bool kingside);
static bool is_square_attacked(game *game, int x, int y, piece_color attacker_color);
static bool is_in_check(game *game, piece_color color);
static move_list get_pseudo_legal_moves(game *game, int x, int y);

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

// TODO: FEN stuff
move_list get_valid_moves(game *game, int x, int y)
{
    move_list pseudo_moves = get_pseudo_legal_moves(game, x, y);
    move_list legal_moves = {.moves = {}, .count = 0};
    piece_color piece_color = get_piece_color(game->board[y][x]);

    for (uint i = 0; i < pseudo_moves.count; i++)
    {
        move m = pseudo_moves.moves[i];

        piece_type temp_dest = game->board[m.y_to][m.x_to];
        piece_type temp_orig = game->board[m.y_from][m.x_from];
        game->board[m.y_to][m.x_to] = temp_orig;
        game->board[m.y_from][m.x_from] = EMPTY;

        if (!is_in_check(game, piece_color))
        {
            legal_moves.moves[legal_moves.count++] = m;
        }

        game->board[m.y_from][m.x_from] = temp_orig;
        game->board[m.y_to][m.x_to] = temp_dest;
    }

    return legal_moves;
}

static move_list get_pseudo_legal_moves(game *game, int x, int y)
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
            m.moves[m.count++] = (move){x, y, x, y + direction, EMPTY, EMPTY}; // Get filled in at the bottom anyway, 2x EMPTY to supress compiler warnings

            // Initial two-square move
            if (y == start_row && game->board[y + 2 * direction][x] == EMPTY)
            {
                m.moves[m.count++] = (move){x, y, x, y + 2 * direction, EMPTY, EMPTY};
            }
        }

        // En passant
        if ((moving_piece == WhitePawn && y == 3) || (moving_piece == BlackPawn && y == 4))
        {
            move last = game->move_history.moves[game->move_history.count - 1];
            piece_type last_piece = last.origin_piece;

            // Check if last move was opponent's pawn moving two squares
            if ((last_piece == BlackPawn || last_piece == WhitePawn) &&
                abs(last.y_to - last.y_from) == 2 &&
                last.y_to == y &&
                abs(last.x_to - x) == 1)
            {
                // Add en passant capture
                m.moves[m.count++] = (move){
                    x, y,                     // from
                    last.x_to, y + direction, // to
                    EMPTY, EMPTY};
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
                    m.moves[m.count++] = (move){x, y, x + dx, y + direction, EMPTY, EMPTY};
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
                    m.moves[m.count++] = (move){x, y, new_x, new_y, EMPTY, EMPTY};
                }
                else if (piece_color != get_piece_color(target))
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y, EMPTY, EMPTY};
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
                    m.moves[m.count++] = (move){x, y, new_x, new_y, EMPTY, EMPTY};
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
                    m.moves[m.count++] = (move){x, y, new_x, new_y, EMPTY, EMPTY};
                }
                else if (piece_color != get_piece_color(target))
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y, EMPTY, EMPTY};
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
                    m.moves[m.count++] = (move){x, y, new_x, new_y, EMPTY, EMPTY};
                }
                else if (piece_color != get_piece_color(target))
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y, EMPTY, EMPTY};
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

        // Normal king moves
        for (int d = 0; d < 8; d++)
        {
            int new_x = x + directions[d][0];
            int new_y = y + directions[d][1];

            if (is_within_bounds(new_x, new_y))
            {
                piece_type target = game->board[new_y][new_x];
                if (target == EMPTY || piece_color != get_piece_color(target))
                {
                    m.moves[m.count++] = (move){x, y, new_x, new_y, EMPTY, EMPTY};
                }
            }
        }

        // Castling
        if (!has_king_moved(game, piece_color))
        {
            // Kingside castling
            if (!has_rook_moved(game, piece_color, true) &&
                game->board[y][x + 1] == EMPTY &&
                game->board[y][x + 2] == EMPTY &&
                !is_in_check(game, piece_color) &&  // Current position not in check
                !is_square_attacked(game, x + 1, y, !piece_color) && // Square king passes through
                !is_square_attacked(game, x + 2, y, !piece_color))   // Final square not attacked
            {
                m.moves[m.count++] = (move){x, y, x + 2, y, EMPTY, EMPTY};
            }

            // Queenside castling
            if (!has_rook_moved(game, piece_color, false) &&
                game->board[y][x - 1] == EMPTY &&
                game->board[y][x - 2] == EMPTY &&
                game->board[y][x - 3] == EMPTY &&
                !is_in_check(game, piece_color) &&  // Current position not in check
                !is_square_attacked(game, x - 1, y, !piece_color) && // Square king passes through
                !is_square_attacked(game, x - 2, y, !piece_color))   // Final square not attacked
            {
                m.moves[m.count++] = (move){x, y, x - 2, y, EMPTY, EMPTY};
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

move_result make_move(game *game, move move)
{
    // TODO: handle overflow/wrap around
    game->move_history.moves[game->move_history.count] = move;
    game->move_history.count++;

    move_result res = None;

    piece_type moving_piece = game->board[move.y_from][move.x_from];

    // En passant
    if ((moving_piece == WhitePawn || moving_piece == BlackPawn) &&
        move.x_from != move.x_to &&                         // Diagonal move
        game->board[move.y_to][move.x_to] == EMPTY &&       // Moving to empty square
        ((moving_piece == WhitePawn && move.y_from == 3) || // White pawn on rank 5
         (moving_piece == BlackPawn && move.y_from == 4)))  // Black pawn on rank 4
    {
        // Remove the captured pawn
        game->board[move.y_from][move.x_to] = EMPTY;

        res = PieceCaptured;
    }

    game->board[move.y_from][move.x_from] = EMPTY;

    piece_type destination_piece = game->board[move.y_to][move.x_to];
    if (destination_piece != EMPTY)
    {
        res = PieceCaptured;
    }

    game->board[move.y_to][move.x_to] = moving_piece;

    // Handle castling
    if ((moving_piece == WhiteKing || moving_piece == BlackKing) &&
        abs(move.x_to - move.x_from) == 2)
    {
        int rook_row = (moving_piece == WhiteKing) ? 7 : 0;
        // Kingside castling
        if (move.x_to > move.x_from)
        {
            game->board[rook_row][7] = EMPTY;
            game->board[rook_row][5] = (moving_piece == WhiteKing) ? WhiteRook : BlackRook;
        }
        // Queenside castling
        else
        {
            game->board[rook_row][0] = EMPTY;
            game->board[rook_row][3] = (moving_piece == WhiteKing) ? WhiteRook : BlackRook;
        }

        res = Castle;
    }

    // Flip turn
    if (game->current_turn == CChessWhite)
    {
        game->current_turn = CChessBlack;
    }
    else
    {
        game->current_turn = CChessWhite;
    }

    return res;
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

    // Check for checkmate or stalemate
    bool has_legal_moves = false;
    piece_color current_color = game->current_turn;

    // Try to find at least one legal move
    for (int i = 0; i < 8 && !has_legal_moves; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (game->board[i][j] != EMPTY &&
                get_piece_color(game->board[i][j]) == current_color)
            {
                move_list moves = get_valid_moves(game, j, i);
                if (moves.count > 0)
                {
                    has_legal_moves = true;
                    break;
                }
            }
        }
    }

    if (!has_legal_moves)
    {
        if (is_in_check(game, current_color))
        {
            return (current_color == CChessWhite) ? BlackWon : WhiteWon;
        }
        return Stalemate;
    }

    return InProgress;
}

static bool has_king_moved(game *game, piece_color color)
{
    piece_type king = (color == CChessWhite) ? WhiteKing : BlackKing;
    int start_x = 4;
    int start_y = (color == CChessWhite) ? 7 : 0;

    for (uint i = 0; i < game->move_history.count; i++)
    {
        move m = game->move_history.moves[i];
        if (m.origin_piece == king &&
            m.x_from == start_x &&
            m.y_from == start_y)
        {
            return true;
        }
    }
    return false;
}

static bool has_rook_moved(game *game, piece_color color, bool kingside)
{
    piece_type rook = (color == CChessWhite) ? WhiteRook : BlackRook;
    int start_x = kingside ? 7 : 0;
    int start_y = (color == CChessWhite) ? 7 : 0;

    for (uint i = 0; i < game->move_history.count; i++)
    {
        move m = game->move_history.moves[i];
        if (m.origin_piece == rook &&
            m.x_from == start_x &&
            m.y_from == start_y)
        {
            return true;
        }
    }
    return false;
}

static bool is_square_attacked(game *game, int x, int y, piece_color attacker_color)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            piece_type piece = game->board[i][j];
            if (piece != EMPTY && get_piece_color(piece) == attacker_color)
            {
                if (piece == BlackQueen || piece == WhiteQueen)
                {
                    if (abs(j - x) == abs(i - y))
                    {
                        int dx = (x - j) > 0 ? 1 : -1;
                        int dy = (y - i) > 0 ? 1 : -1;
                        bool path_clear = true;

                        int check_x = j + dx;
                        int check_y = i + dy;
                        while (check_x != x && check_y != y)
                        {
                            if (game->board[check_y][check_x] != EMPTY)
                            {
                                path_clear = false;
                                break;
                            }
                            check_x += dx;
                            check_y += dy;
                        }

                        if (path_clear)
                        {
                            return true;
                        }
                    }

                    if (j == x || i == y)
                    {
                        bool path_clear = true;
                        if (j == x) // Same file
                        {
                            int dy = (y - i) > 0 ? 1 : -1;
                            for (int check_y = i + dy; check_y != y; check_y += dy)
                            {
                                if (game->board[check_y][x] != EMPTY)
                                {
                                    path_clear = false;
                                    break;
                                }
                            }
                        }
                        else // Same rank
                        {
                            int dx = (x - j) > 0 ? 1 : -1;
                            for (int check_x = j + dx; check_x != x; check_x += dx)
                            {
                                if (game->board[y][check_x] != EMPTY)
                                {
                                    path_clear = false;
                                    break;
                                }
                            }
                        }

                        if (path_clear)
                        {
                            return true;
                        }
                    }
                }
                else
                {
                    move_list moves = get_pseudo_legal_moves(game, j, i);
                    for (uint k = 0; k < moves.count; k++)
                    {
                        if (moves.moves[k].x_to == x && moves.moves[k].y_to == y)
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

static bool is_in_check(game *game, piece_color color)
{
    int king_x = -1, king_y = -1;
    piece_type king = (color == CChessWhite) ? WhiteKing : BlackKing;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (game->board[i][j] == king)
            {
                king_y = i;
                king_x = j;
                break;
            }
        }
        if (king_x != -1) break;
    }

    return is_square_attacked(game, king_x, king_y, !color);
}