#pragma once
#include <cstdint>

bool convertYUYVtoRGBA(const uint8_t* yuyv, int width, int height, uint8_t* rgba);