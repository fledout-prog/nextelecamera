#pragma once
#include <cstdint>
#include <cstddef>
#include <atomic>

class FrameQueue {
public:
    static constexpr int SLOT_COUNT = 3;
    FrameQueue(int width, int height);
    ~FrameQueue();
    uint8_t* getWriteSlot();
    void commitWrite();
    uint8_t* getReadSlot();
    void destroy();
private:
    struct Slot {
        uint8_t* data = nullptr;
        std::atomic<bool> ready{false};
    };
    Slot slots[SLOT_COUNT];
    size_t frameSize;
    std::atomic<int> writeIdx{0};
    int currentWriteIdx = 0; // fixed for duration of a write cycle
};