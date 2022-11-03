#include "parsingFunc.hpp"
#include <algorithm>

namespace parseF{

void FillMessage(std::vector<char>& msg, const Recorder::RecordingResult& record){
    msg.clear();
    msg.push_back(0xAA); // header byte #1 (byte #0).
    msg.push_back(0x55); // header byte #2 (byte #1).
    const char size_tSizeInBytes = sizeof(size_t); 
    msg.push_back(sizeof(size_t)); // size_t size in bytes (byte #2).
    /*Allocate message size in bytes to buffer*/
    for(size_t i = 0; i < size_tSizeInBytes; ++i){
        msg.push_back(0);
    }
    /*Write frames to buffer*/
    char* framesToCharArr = (char*)(&record.frames);
    for(size_t i = 0; i < size_tSizeInBytes; ++i){
        msg.push_back(framesToCharArr[i]);
    }
    /*Write data to buffer*/
    std::copy(record.data.begin(), record.data.end(), std::back_inserter(msg));
    size_t msgSize = msg.size();
    /*Write message size in bytes to buffer*/
    char* msgSizeToCharArr = (char*)(&msgSize);
    for(size_t i = 0; i < size_tSizeInBytes; ++i){
        msg[3 + i] = msgSizeToCharArr[i];
    }
}

bool ParseMessage(const std::array<char, MAX_BUFFER_SIZE>& msg, Recorder::RecordingResult& record){
    /*Filtering trash data and clients that use size_t which is not the same size as size_t on the server side.*/
    if(msg[0] != static_cast<char>(0xAA) or
        msg[1] != static_cast<char>(0x55) or
        msg[2] != static_cast<char>(sizeof(size_t))) return false; 
    /*Write frames data*/
    size_t* frames = (size_t*)(&msg[3 + msg[2]]);
    record.frames = *frames;
    /*Calculating of data size*/
    size_t* msgSize = (size_t*)(&msg[3]);
    size_t headSize = 3 + 2*msg[2]; // Size of header bytes + byte with size of size_t type + message size value + frames value.
    size_t dataSize = *msgSize - headSize;
    /*Write data*/
    for (size_t i = 0; i < dataSize; ++i){
        record.data.push_back(msg[headSize + i]);
    }
    return true;
};

}