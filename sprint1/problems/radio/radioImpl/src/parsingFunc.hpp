/*
    Functions for packing and unpacking Recorder::RecordingResult data to buffer. 
    Structure of packed Recorder::RecordingResult:
        [0]: 0xAA header byte #1.
        [1]: 0x55 header byte #2.
        [2]: size of size_t type in bytes.
        [3] - [ (3 + [2] - 1) ]: full size of packed data (size of transmitting buffer) type in bytes.
        [ (3 + [2] - 1) ] - [ (3 + 2*[2] - 1) ]: frames field of Recorder::RecordingResult type in bytes.
        [ (3 + 2*[2] - 1) ] - [n]: data field of Recorder::RecordingResult.

    * Header 0xAA 0x55 for filtering trash data. I think no really need use checksum in this case.
*/
#pragma once

#include "audio.h"
#include <vector>
#include <array>

namespace parseF{
const size_t MAX_BUFFER_SIZE = 65100;
void FillMessage(std::vector<char>& msg, const Recorder::RecordingResult& record);
bool ParseMessage(const std::array<char, MAX_BUFFER_SIZE>& msg, Recorder::RecordingResult& record);

}