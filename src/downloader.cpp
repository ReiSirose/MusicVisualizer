#include <iostream>
#include <string>
#include <array>
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

// Fallback macros in case CMake definitions are missing during raw compilation
#ifndef YT_DLP_PATH
#define YT_DLP_PATH "yt-dlp"
#endif

#ifndef FFMPEG_PATH
#define FFMPEG_PATH "ffmpeg"
#endif

void downloadAudio(const std::string& url, const std::string& customName = "") {
    // 2. Resolve Paths
    fs::path buildDir = fs::current_path();
    fs::path songDir = buildDir.parent_path() / "song";

    if (!fs::exists(songDir)) {
        fs::create_directories(songDir);
    }

    std::string filenameTemplate = customName.empty() ? "%(title)s" : customName;
    // 3. Construct Command
    std::string command = std::string(YT_DLP_PATH) + 
                          " -x --audio-format mp3 -o \"" + songDir.string() + "/" + filenameTemplate + ".%(ext)s\"" +
                          " --ffmpeg-location " + std::string(FFMPEG_PATH) + " " + url + " 2>&1";

    std::array<char, 128> buffer;
    
    // 4. Execute via Pipe
#ifdef _WIN32
    auto pipe = _popen(command.c_str(), "r");
#else
    auto pipe = popen(command.c_str(), "r");
#endif

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    // Read and print output stream
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        std::cout << buffer.data();
    }

#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [options]\n";
        std::cout << "Try '" << argv[0] << " -help' for more information.\n";
        return 1;
    }

    std::string url = "";
    std::string customName = "";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "-help") {
            std::cout << "Music Visualizer Downloader\n\n";
            std::cout << "Usage: " << argv[0] << " -add <url> [-n <name>]\n\n";
            std::cout << "Options:\n";
            std::cout << "  -add <url>     Download and extract audio from the given YouTube URL.\n";
            std::cout << "  -n, -name      (Optional) Specify a custom filename for the output MP3.\n";
            std::cout << "  -h, -help      Show this help message and exit.\n";
            return 0;
        } 
        else if (arg == "-add" && i + 1 < argc) {
            url = argv[++i];
        } 
        else if ((arg == "-n" || arg == "-name") && i + 1 < argc) {
            customName = argv[++i];
        } 
        else {
            std::cerr << "Unknown or incomplete argument: " << arg << "\n";
            return 1;
        }
    }

    // Execute if URL was found
    if (!url.empty()) {
        try {
            downloadAudio(url, customName);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error: Missing -add <url> argument.\n";
        return 1;
    }

    return 0;
}