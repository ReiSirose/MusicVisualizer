#include <iostream>
#include <filesystem>
#include <algorithm>
#include "audio.h"
#include "song.h"

Song::Song(std::string_view playListPath) : m_playListIndex{0}, m_playListPath{playListPath}, 
                                            m_direction{Direction::None}, m_isPlaying{false}, m_volume{1.0f}
{
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

Audio* Song::currentSong() {
    size_t currentSlot = m_playListIndex % CACHE_SIZE;
    if(m_slotState[currentSlot].load(std::memory_order_acquire) == CacheState::Ready){
        return m_audioList[currentSlot].get();
    }
    return nullptr;
}

void Song::loadAudio(bool cache) {}
void Song::nextSong() {
    if(m_totalSong == 0){
        return;
    }
    m_playListIndex = (m_playListIndex + 1) % m_totalSong;
    m_direction = Direction::Forward;
    loadAudio();
    
}
void Song::prevSong() {
    if(m_totalSong == 0){
        return;
    }
    m_playListIndex = (m_playListIndex + m_totalSong - 1) % m_totalSong;
    m_direction = Direction::Forward;
    loadAudio();
}

void Song::togglePlay() {
    if(m_isPlaying.load(std::memory_order_relaxed)){
        ma_device_stop(&m_device);
        m_isPlaying.store(false, std::memory_order_relaxed);
    }
    else {
        if(ma_device_start(&m_device) == MA_SUCCESS){
            m_isPlaying.store(true, std::memory_order_relaxed);
        }
    }
}

void Song::setVolume(float volume) {
    m_volume = volume;
    ma_device_get_master_volume(&m_device, &m_volume);
}

void Song::s_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    Song* pSong = (Song*)pDevice->pUserData;
    if(pSong) {
        pSong->data_callback(pOutput, frameCount);
    }
}


void Song::data_callback(void* pOutput, ma_uint32 frameCount){
    float* pOutputF32 = (float*)pOutput;
    size_t currentSlot = m_playListIndex % CACHE_SIZE;

    if(m_slotState[currentSlot].load(std::memory_order_acquire) == CacheState::Ready){
        size_t read = m_audioList[currentSlot]->readSamples(pOutputF32, frameCount);
        if(read < frameCount){
            std::fill(pOutputF32 + read, pOutputF32 + frameCount, 0.0f);
        }
    }
    else {
        // Cache missed or loading
        std::fill(pOutputF32, pOutputF32 + frameCount, 0.0f);
    }
}

void Song::loadAudio(bool cache){
    if(m_totalSong == 0) return;

    if (m_prefetchTask.valid() && m_prefetchTask.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        return; 
    }
    
    size_t targetIdx = m_playListIndex;
    m_prefetchTask = std::async(std::launch::async, [this, targetIdx]() {
        prefetchWorker(targetIdx);
    });
}