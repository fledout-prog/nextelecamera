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
    if (!mjpegData || mjpegSize == 0) return false;
    int w=0,h=0,subSamp=0,colorSpace=0;
    if (tjDecompressHeader3(decompressHandle, mjpegData, (unsigned long)mjpegSize, &w,&h,&subSamp,&colorSpace) < 0) {
        __android_log_print(ANDROID_LOG_WARN, TAG, "Header decode failed: %s", tjGetErrorStr());
        return false;
    }
    // Guard against size mismatch - TJFLAG_NOREALLOC would SIGSEGV if dimensions differ
    if (w != width || h != height) {
        __android_log_print(ANDROID_LOG_WARN, TAG, "Frame size mismatch: got %dx%d expected %dx%d - dropping frame", w, h, width, height);
        return false;
    }
    int result = tjDecompress2(decompressHandle, mjpegData, (unsigned long)mjpegSize,
                               outRGBA, width, width*4, height, TJPF_RGBA, TJFLAG_FASTDCT|TJFLAG_NOREALLOC);
    if (result < 0) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Decompress failed: %s", tjGetErrorStr());
        return false;
    }
    return true;
}

uint8_t* MJPEGDecoder::encodeToJpeg(const uint8_t* rgba, int width, int height, int quality, size_t* outSize) {
    uint8_t* jpegBuf = nullptr;
    unsigned long jpegSize = 0;
    int result = tjCompress2(compressHandle, rgba, width, width*4, height, TJPF_RGBA, &jpegBuf, &jpegSize, TJSAMP_420, quality, TJFLAG_FASTDCT);
    if (result < 0) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Compress failed: %s", tjGetErrorStr());
        return nullptr;
    }
    *outSize = (size_t)jpegSize;
    return jpegBuf;
}