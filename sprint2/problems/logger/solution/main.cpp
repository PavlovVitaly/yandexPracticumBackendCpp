#include "my_logger.h"

#include <string_view>
#include <thread>
#include <filesystem>
#include <cassert>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std::literals;


bool CompareFiles(const std::filesystem::path& file_path, const std::filesystem::path& ref_file_path) {
    if (file_path.filename() != ref_file_path.filename()) {
        std::cout << "file_path: " << file_path << std::endl;
        std::cout << "ref_file_path: " << ref_file_path << std::endl;
        std::cout << "Filename error! " << std::endl;
        return false;
    }

    if (!std::filesystem::exists(file_path)) {
        std::cout << "file_path: " << file_path << std::endl;
        std::cout << "File not found!" << std::endl;
        return false;
    }

    auto file_size = std::filesystem::file_size(file_path);
    auto ref_file_size = std::filesystem::file_size(ref_file_path);

    if (file_size != ref_file_size) {
        std::cout << "file_path: " << file_path << std::endl;
        std::cout << "ref_file_path: " << ref_file_path << std::endl;
        std::cout << "Error: file size mismatch! "  << file_size << " != " << ref_file_size << std::endl;
        return false;
    }

    std::ifstream fs(file_path, std::ifstream::binary|std::ifstream::ate);
    std::ifstream ref_fs(ref_file_path, std::ifstream::binary|std::ifstream::ate);

    if (fs.fail() || ref_fs.fail()) {
        std::cout << "file_path: " << file_path << std::endl;
        std::cout << "ref_file_path: " << ref_file_path << std::endl;
        std::cout << "Error reading file! " << std::endl;
        return false;
    }

    return true;
}

int main() {
    const std::filesystem::path logs_path("/var/log");
    const std::filesystem::path reference_path = std::filesystem::current_path() / std::filesystem::path("cpp-backend-tests-practicum/tests/cpp/test_s02_logger/reference");

    //const std::filesystem::path logs_path("./logs");
    //const std::filesystem::path reference_path = std::filesystem::current_path() / std::filesystem::path("reference");

    Logger::GetInstance().SetTimestamp(std::chrono::system_clock::time_point{1000000s});
    LOG("Hello "sv, "world "s, 123);
    LOG(1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
        1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
        1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0);

    Logger::GetInstance().SetTimestamp(std::chrono::system_clock::time_point{10000000s});
    LOG("Brilliant logger.", " ", "I Love it");

    static const int attempts = 100000;
    for(int i = 0; i < attempts; ++i) {
        Logger::GetInstance().SetTimestamp(std::chrono::system_clock::time_point(std::chrono::seconds(10000000 + i * 100)));
        LOG("Logging attempt ", i, ". ", "I Love it");
    }

    for (const auto& entry : std::filesystem::directory_iterator{reference_path}) {
        std::filesystem::path filename = entry.path().filename();
        if(!CompareFiles(logs_path / filename, entry.path())) {
            std::cout << "Check failed!" << std::endl;
            return -1;
        }
    }

    std::cout << "Success!" << std::endl;
}
