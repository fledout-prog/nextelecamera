#pragma once
#include <cstdint>
#include <cstddef>
#include <turbojpeg.h>

class MJPEGDecoder {
public:
    MJPEGDecoder();
    ~MJPEGDecoder();
    bool decode(const uint8_t* mjpegData, size_t mjpegSize, uint8_t* outRGBA, int width, int height);
    uint8_t* encodeToJpeg(const uint8_t* rgba, int width, int height, int quality, size_t* outSize);
private:
    tjhandle decompressHandle;
    tjhandle compressHandle;
};
