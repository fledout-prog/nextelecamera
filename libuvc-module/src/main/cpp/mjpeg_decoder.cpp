#include "mjpeg_decoder.h"
#include <android/log.h>
#define TAG "MJPEGDecoder"

MJPEGDecoder::MJPEGDecoder() {
    decompressHandle = tjInitDecompress();
    compressHandle = tjInitCompress();
}

MJPEGDecoder::~MJPEGDecoder() {
    if (decompressHandle) tjDestroy(decompressHandle);
    if (compressHandle) tjDestroy(compressHandle);
}

bool MJPEGDecoder::decode(const uint8_t* mjpegData, size_t mjpegSize, uint8_t* outRGBA, int width, int height) {
    int w=0,h=0,subSamp=0,colorSpace=0;
    if (tjDecompressHeader3(decompressHandle, mjpegData, (unsigned long)mjpegSize, &w,&h,&subSamp,&colorSpace) < 0) return false;
    int result = tjDecompress2(decompressHandle, mjpegData, (unsigned long)mjpegSize, outRGBA, width, width*4, height, TJPF_RGBA, TJFLAG_FASTDCT|TJFLAG_NOREALLOC);
    return result >= 0;
}

uint8_t* MJPEGDecoder::encodeToJpeg(const uint8_t* rgba, int width, int height, int quality, size_t* outSize) {
    uint8_t* jpegBuf = nullptr;
    unsigned long jpegSize = 0;
    int result = tjCompress2(compressHandle, rgba, width, width*4, height, TJPF_RGBA, &jpegBuf, &jpegSize, TJSAMP_420, quality, TJFLAG_FASTDCT);
    if (result < 0) return nullptr;
    *outSize = (size_t)jpegSize;
    return jpegBuf;
}