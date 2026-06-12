#include <vector>
#include <string.h>
#include "audio.h"
#include "song.h"
#include <iostream>
#include <filesystem>

// TODO: read the song folder and populate the m_playList
Song::Song(std::string_view playListPath) : m_playListIndex{0}, m_playListPath{playListPath}{
    std::filesystem::path directoryPath {playListPath};
    try {
        if(std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)){

            auto dist = std::distance(std::filesystem::directory_iterator(directoryPath), std::filesystem::directory_iterator{});
            m_playList.reserve(dist);

            for(const auto& entry: std::filesystem::directory_iterator(directoryPath)){
                if (entry.is_regular_file()){
                    m_playList.push_back(std::move(entry.path().filename().string()));
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e){
        std::cerr << "Error: " << e.what() << '\n';
    }
}

Song::~Song(){

}