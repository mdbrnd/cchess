#ifndef RENDERER_H
#define RENDERER_H

#include <stdio.h>
#include "chess.h"
#include "raylib.h"

void get_piece_path(piece_type piece, char *path, size_t size);

Texture2D get_piece_texture(piece_type piece);

#endif