package com.nextelecamera.native

object UVCBridge {
    init { System.loadLibrary("uvc_pipeline") }
    external fun nativeStart()
    external fun nativeStop()
    external fun nativeGetLatestFrame()
    external fun nativeCapture()
}