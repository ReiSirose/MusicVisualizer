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
                    ++m_totalSong;
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e){
        std::cerr << "Error: " << e.what() << '\n';
    }

    //Pre-allocate the audio object

    for(size_t i {0}; i < CACHE_SIZE; ++i){
        m_audioList[i] = std::make_unique<Audio>(44100, 1024);
        m_slotState[i].store(CacheState::Empty, std::memory_order_relaxed);
    }
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format  = ma_format_f32;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate = 44100;
    deviceConfig.dataCallback = s_data_callback;
    deviceConfig.pUserData = this;

    if(ma_device_init(nullptr, &deviceConfig, &m_device) != MA_SUCCESS){
        std::cerr << "ERROR: failed to init hardware device link." << std::endl;
    }
}

Song::~Song(){
    if(m_prefetchTask.valid()){
        m_prefetchTask.wait();
    }
    ma_device_uninit(&m_device);
}

Audio* Song::currentSong() {}

void Song::loadAudio(bool cache) {}
void Song::nextSong() {}
void Song::prevSong() {}

void Song::togglePlay() {}

void Song::setVolume() {}

void Song::s_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{}


void Song::data_callback(void* pOutput, ma_uint32 frameCount){

}
void Song::loadAudio(bool cache){
    
}