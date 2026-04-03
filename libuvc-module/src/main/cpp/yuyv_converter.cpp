#include "yuyv_converter.h"
#include <libyuv.h>
#include <android/log.h>
#define TAG "YUYVConverter"

bool convertYUYVtoRGBA(const uint8_t* yuyv, int width, int height, uint8_t* rgba) {
    int result = libyuv::YUY2ToARGB(yuyv, width*2, rgba, width*4, width, height);
    if (result != 0) { __android_log_print(ANDROID_LOG_ERROR, TAG, "libyuv failed: %d", result); return false; }
    return true;
}