package com.nextelecamera

import android.content.Context
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.util.Log
import com.nextelecamera.`native`.UVCBridge

private const val TAG = "UVCPipeline"

class UVCPipeline(private val context: Context) {
    private var isRunning = false

    fun start(device: UsbDevice, config: FormatConfig): Boolean {
        val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
        val conn = usbManager.openDevice(device)
        if (conn == null) { Log.e(TAG, "Failed to open USB device"); return false }
        isRunning = UVCBridge.nativeStart(conn.fileDescriptor, device.vendorId, device.productId, config.width, config.height, config.fps)
        return isRunning
    }

    fun stop() { if (isRunning) { UVCBridge.nativeStop(); isRunning = false } }
    fun getLatestFrame(width: Int, height: Int): ByteArray? = UVCBridge.nativeGetLatestFrame(width, height)
    fun capture(width: Int, height: Int, quality: Int = 90): ByteArray? = UVCBridge.nativeCapture(width, height, quality)
    val running: Boolean get() = isRunning
}