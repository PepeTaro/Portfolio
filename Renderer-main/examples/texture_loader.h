#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "texture.h"

#include "CImg.h" // テクスチャ画像をロードするために必要な外部ライブラリ
using namespace cimg_library;

Texture LoadTexture(const char* file_path);

#endif// TEXTURE_LOADER_H
