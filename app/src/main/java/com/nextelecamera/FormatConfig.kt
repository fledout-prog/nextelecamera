package com.nextelecamera

enum class UvcFormat { MJPEG, YUYV }

data class FormatConfig(
    val format: UvcFormat,
    val width: Int,
    val height: Int,
    val fps: Int
) {
    companion object {
        fun selectBest(width: Int, height: Int, fps: Int = 30): FormatConfig {
            val format = if (width >= 1280) UvcFormat.MJPEG else UvcFormat.YUYV
            return FormatConfig(format, width, height, fps)
        }
    }
}