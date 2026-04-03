cmake_minimum_required(VERSION 3.22)
project(uvc_pipeline CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(uvc_pipeline SHARED
    uvc_pipeline.cpp
    mjpeg_decoder.cpp
    yuyv_converter.cpp
    frame_queue.cpp
)

find_library(log-lib log)
find_library(android-lib android)
find_library(gles2-lib GLESv2)

set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

macro(import_prebuilt_lib TARGET_NAME LIB_FILENAME)
    set(_LIB_PATH "${JNI_LIBS_DIR}/${LIB_FILENAME}")
    if(EXISTS "${_LIB_PATH}")
        add_library(${TARGET_NAME} SHARED IMPORTED)
        set_target_properties(${TARGET_NAME} PROPERTIES IMPORTED_LOCATION "${_LIB_PATH}")
        message(STATUS "Found prebuilt: ${_LIB_PATH}")
    else()
        message(WARNING "Prebuilt NOT found: ${_LIB_PATH}. See jniLibs/README.md.")
        add_library(${TARGET_NAME} INTERFACE)
    endif()
endmacro()

import_prebuilt_lib(jpeg-turbo "libjpeg-turbo.so")
import_prebuilt_lib(yuv "libyuv.so")
import_prebuilt_lib(usb1 "libusb1.0.so")
import_prebuilt_lib(uvc "libuvc.so")

target_include_directories(uvc_pipeline PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(uvc_pipeline
    jpeg-turbo yuv usb1 uvc
    ${gles2-lib}
    ${log-lib}
    ${android-lib}
)