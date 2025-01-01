#include "renderer.h"

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

