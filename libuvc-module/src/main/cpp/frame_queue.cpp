#include "frame_queue.h"
#include <android/log.h>
#define TAG "FrameQueue"

FrameQueue::FrameQueue(int width, int height) {
    frameSize = static_cast<size_t>(width) * height * 4;
    for (int i = 0; i < SLOT_COUNT; i++) {
        slots[i].data = new uint8_t[frameSize];
        slots[i].ready.store(false);
    }
    writeIdx.store(0);
    currentWriteIdx = 0;
}

FrameQueue::~FrameQueue() { destroy(); }

void FrameQueue::destroy() {
    for (int i = 0; i < SLOT_COUNT; i++) {
        delete[] slots[i].data;
        slots[i].data = nullptr;
        slots[i].ready.store(false);
    }
}

uint8_t* FrameQueue::getWriteSlot() {
    // Lock the slot index for this entire write cycle - prevents race with commitWrite()
    currentWriteIdx = writeIdx.load(std::memory_order_relaxed) % SLOT_COUNT;
    slots[currentWriteIdx].ready.store(false, std::memory_order_relaxed);
    return slots[currentWriteIdx].data;
}

void FrameQueue::commitWrite() {
    // Use the same index captured in getWriteSlot()
    slots[currentWriteIdx].ready.store(true, std::memory_order_release);
    writeIdx.fetch_add(1, std::memory_order_relaxed);
}

uint8_t* FrameQueue::getReadSlot() {
    int w = writeIdx.load(std::memory_order_relaxed);
    int latest = ((w - 1) % SLOT_COUNT + SLOT_COUNT) % SLOT_COUNT;
    if (slots[latest].ready.load(std::memory_order_acquire)) return slots[latest].data;
    return nullptr;
}