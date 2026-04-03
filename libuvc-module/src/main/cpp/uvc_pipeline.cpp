#include <jni.h>
#include <android/log.h>
#include <libuvc/libuvc.h>
#include "mjpeg_decoder.h"
#include "yuyv_converter.h"
#include "frame_queue.h"

#define TAG "UVCPipeline"

static uvc_context_t*       g_ctx     = nullptr;
static uvc_device_handle_t* g_devh    = nullptr;
static uvc_stream_ctrl_t    g_ctrl    = {};
static FrameQueue*          g_queue   = nullptr;
static MJPEGDecoder*        g_decoder = nullptr;
static int                  g_width   = 0;
static int                  g_height  = 0;
static bool                 g_isMJPEG = true;

static void frameCallback(uvc_frame_t* frame, void*) {
    if (!g_queue || !frame || !frame->data) return;
    uint8_t* slot = g_queue->getWriteSlot();
    if (!slot) return;
    if (g_isMJPEG) {
        g_decoder->decode(static_cast<uint8_t*>(frame->data), frame->data_bytes, slot, g_width, g_height);
    } else {
        convertYUYVtoRGBA(static_cast<uint8_t*>(frame->data), g_width, g_height, slot);
    }
    g_queue->commitWrite();
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_nextelecamera_native_UVCBridge_nativeStart(JNIEnv*, jobject, jint fd, jint, jint, jint width, jint height, jint fps) {
    g_width = width; g_height = height;
    g_queue = new FrameQueue(width, height);
    g_decoder = new MJPEGDecoder();
    if (uvc_init(&g_ctx, nullptr) != UVC_SUCCESS) return JNI_FALSE;
    if (uvc_wrap(fd, g_ctx, &g_devh) != UVC_SUCCESS) { uvc_exit(g_ctx); g_ctx=nullptr; return JNI_FALSE; }
    uvc_error_t res = uvc_get_stream_ctrl_format_size(g_devh, &g_ctrl, UVC_FRAME_FORMAT_MJPEG, width, height, fps);
    if (res == UVC_SUCCESS) { g_isMJPEG = true; }
    else {
        res = uvc_get_stream_ctrl_format_size(g_devh, &g_ctrl, UVC_FRAME_FORMAT_YUYV, width, height, fps);
        if (res != UVC_SUCCESS) { uvc_close(g_devh); uvc_exit(g_ctx); g_devh=nullptr; g_ctx=nullptr; return JNI_FALSE; }
        g_isMJPEG = false;
    }
    if (uvc_start_streaming(g_devh, &g_ctrl, frameCallback, nullptr, 0) != UVC_SUCCESS) {
        uvc_close(g_devh); uvc_exit(g_ctx); g_devh=nullptr; g_ctx=nullptr; return JNI_FALSE;
    }
    return JNI_TRUE;
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_nextelecamera_native_UVCBridge_nativeGetLatestFrame(JNIEnv* env, jobject, jint width, jint height) {
    if (!g_queue) return nullptr;
    uint8_t* slot = g_queue->getReadSlot();
    if (!slot) return nullptr;
    size_t size = (size_t)width * height * 4;
    jbyteArray arr = env->NewByteArray((jsize)size);
    if (arr) env->SetByteArrayRegion(arr, 0, (jsize)size, (jbyte*)slot);
    return arr;
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_nextelecamera_native_UVCBridge_nativeCapture(JNIEnv* env, jobject, jint width, jint height, jint quality) {
    if (!g_queue || !g_decoder) return nullptr;
    uint8_t* slot = g_queue->getReadSlot();
    if (!slot) return nullptr;
    size_t jpegSize = 0;
    uint8_t* jpegBuf = g_decoder->encodeToJpeg(slot, width, height, quality, &jpegSize);
    if (!jpegBuf || !jpegSize) return nullptr;
    jbyteArray arr = env->NewByteArray((jsize)jpegSize);
    if (arr) env->SetByteArrayRegion(arr, 0, (jsize)jpegSize, (jbyte*)jpegBuf);
    tjFree(jpegBuf);
    return arr;
}

extern "C" JNIEXPORT void JNICALL
Java_com_nextelecamera_native_UVCBridge_nativeStop(JNIEnv*, jobject) {
    if (g_devh) { uvc_stop_streaming(g_devh); uvc_close(g_devh); g_devh=nullptr; }
    if (g_ctx)  { uvc_exit(g_ctx); g_ctx=nullptr; }
    delete g_queue; g_queue=nullptr;
    delete g_decoder; g_decoder=nullptr;
}