package com.nextelecamera.`native`

/**
 * JNI bridge to the native UVC pipeline (uvc_pipeline.so).
 * Wraps: libusb · libuvc · libjpeg-turbo · libyuv
 */
object UVCBridge {
    init { System.loadLibrary("uvc_pipeline") }
    external fun nativeStart(fd: Int, vid: Int, pid: Int, width: Int, height: Int, fps: Int): Boolean
    external fun nativeStop()
    external fun nativeGetLatestFrame(width: Int, height: Int): ByteArray?
    external fun nativeCapture(width: Int, height: Int, quality: Int): ByteArray?
}