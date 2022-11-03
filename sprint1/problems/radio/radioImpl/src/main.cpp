#include "audio.h"
#include <iostream>

using namespace std::literals;

int main(int argc, char** argv) {
    Recorder recorder(ma_format_u8, 1);
    Player player(ma_format_u8, 1);

    while (true) {
        std::string str;

        std::cout << "Press Enter to record message..." << std::endl;
        std::getline(std::cin, str);

        auto rec_result = recorder.Record(65000, 1.5s);
        std::cout << "Recording done" << std::endl;

        player.PlayBuffer(rec_result.data.data(), rec_result.frames, 1.5s);
        std::cout << "Playing done" << std::endl;
    }

    return 0;
}
