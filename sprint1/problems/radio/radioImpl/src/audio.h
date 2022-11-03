#pragma once

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <algorithm>
#include <thread>
#include <chrono>
#include <vector>

class Recorder {
    static void Callback(ma_device* pDevice, void* pOutput, const void* pInput,
                         ma_uint32 frameCount) {
        Recorder* recorder = reinterpret_cast<Recorder*>(pDevice->pUserData);

        recorder->SaveBuffer(pInput, frameCount);
    }

    void SaveBuffer(const void* pInput, ma_uint32 frameCount) {
        size_t size = std::min(static_cast<size_t>(frameCount * frame_size_),
                               buffer_.size() - current_off_);

        std::copy_n(reinterpret_cast<const char*>(pInput), size, buffer_.data() + current_off_);

        current_off_ += size;
    }

public:
    Recorder(ma_format format, int channels) {
        ma_device_config device_config;

        device_config = ma_device_config_init(ma_device_type_capture);
        device_config.capture.pDeviceID = NULL;
        device_config.capture.format = format;
        device_config.capture.channels = channels;
        device_config.sampleRate = 44100;
        device_config.dataCallback = Callback;
        device_config.pUserData = this;

        frame_size_ = ma_get_bytes_per_frame(format, channels);
        init_result_ = ma_device_init(NULL, &device_config, &device_);
    }

    ~Recorder() {
        ma_device_uninit(&device_);
    }

    struct RecordingResult {
        std::vector<char> data;
        size_t frames;
    };

    template <typename Rep, typename Period>
    RecordingResult Record(size_t max_frames, std::chrono::duration<Rep, Period> dur) {
        current_off_ = 0;

        buffer_.resize(max_frames * frame_size_);

        ma_device_start(&device_);
        std::this_thread::sleep_for(dur);
        ma_device_stop(&device_);

        return {std::move(buffer_), current_off_ / frame_size_};
    }

    int GetFrameSize() const {
        return frame_size_;
    }

private:
    ma_device device_;
    ma_result init_result_;
    int frame_size_;

    std::vector<char> buffer_;

    size_t current_off_;
};

class Player {
    static void Callback(ma_device* pDevice, void* pOutput, const void* pInput,
                         ma_uint32 frameCount) {
        Player* player = reinterpret_cast<Player*>(pDevice->pUserData);

        player->FillBuffer(pOutput, frameCount);
    }

    void FillBuffer(void* pOutput, ma_uint32 frameCount) {
        size_t size = std::min(static_cast<size_t>(frameCount * frame_size_),
                               max_frame_ * frame_size_ - current_off_);

        std::copy_n(current_buffer_ + current_off_, size, reinterpret_cast<char*>(pOutput));

        current_off_ += size;
    }

public:
    Player(ma_format format, int channels) {
        ma_device_config device_config;

        device_config = ma_device_config_init(ma_device_type_playback);
        device_config.playback.pDeviceID = NULL;
        device_config.playback.format = format;
        device_config.playback.channels = channels;
        device_config.sampleRate = 44100;
        device_config.dataCallback = Callback;
        device_config.pUserData = this;

        frame_size_ = ma_get_bytes_per_frame(format, channels);
        init_result_ = ma_device_init(NULL, &device_config, &device_);
    }

    ~Player() {
        ma_device_uninit(&device_);
    }

    template <typename Rep, typename Period>
    void PlayBuffer(const char* data, size_t frames, std::chrono::duration<Rep, Period> dur) {
        current_buffer_ = data;
        current_off_ = 0;
        max_frame_ = frames;

        ma_device_start(&device_);
        std::this_thread::sleep_for(dur);
        ma_device_stop(&device_);
    }

    int GetFrameSize() const {
        return frame_size_;
    }

private:
    ma_device device_;
    ma_result init_result_;
    int frame_size_;

    const char* current_buffer_;
    size_t current_off_;
    size_t max_frame_;
};
