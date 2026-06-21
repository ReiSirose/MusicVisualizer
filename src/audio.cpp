#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "kiss_fftr.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <atomic>
#include <mutex>
#include "audio.h"


Audio::Audio(uint32_t sampleRate, int nfftSize): 
    m_cfg(nullptr),
    m_nfft(nfftSize),
    m_sampleRate(sampleRate),
    m_channels(1),
    m_playbackSampleIndex(0),
    m_visualSampleIndex(0),
    m_frontBufferIndex(0)
{
    m_cfg = kiss_fftr_alloc(nfftSize, 0 , nullptr, nullptr);
    if (m_cfg == nullptr) {
        std::cerr << "Error: Failed to allocate Kiss FFT configuration." << std::endl;
        return;
    }
    m_fftIn.resize(m_nfft);
    m_fftOut.resize(m_nfft / 2 + 1);

    for(auto& buffer : m_frequencyBuffers){
        buffer.resize(m_nfft / 2 + 1, 0.0f);
    }
}

Audio::~Audio(){
    if(m_cfg){
        kiss_fft_free(m_cfg);
    }
}

void Audio::Update(){
    if(!m_cfg ||m_pcmData.empty()){
        return;
    }
    size_t currentPlaybackIndex = m_playbackSampleIndex.load(std::memory_order_relaxed);

    size_t frontIdx = m_frontBufferIndex.load(std::memory_order_relaxed);
    size_t backIdx = 1 - frontIdx;
    auto& backBuffer = m_frequencyBuffers[backIdx];

    if(currentPlaybackIndex + m_nfft > m_pcmData.size()){
        std::fill(backBuffer.begin(), backBuffer.end(), 0.0f);
        m_frontBufferIndex.store(backIdx, std::memory_order_release);
        return;
    }
    std::copy(
        m_pcmData.begin() + currentPlaybackIndex,
        m_pcmData.begin() + currentPlaybackIndex + m_nfft,
        m_fftIn.begin()
    );

    kiss_fftr(m_cfg, m_fftIn.data(), m_fftOut.data());
    for(size_t i = 0; i < backBuffer.size(); ++i){
        float real {m_fftOut.at(i).r};
        float imagine {m_fftOut.at(i).i};
        backBuffer[i] = std::sqrt(real * real + imagine * imagine);
    }

    m_frontBufferIndex.store(backIdx, std::memory_order_release);
    m_visualSampleIndex.store(currentPlaybackIndex, std::memory_order_relaxed);
}

bool Audio::reload(const std::string& filename){
    ma_decoder decoder;
    ma_result result;
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, m_channels, m_sampleRate);
    result = ma_decoder_init_file(filename.c_str(), &config, &decoder);
    if(result != MA_SUCCESS){
        std::cerr << "Error: Failed to load audio file: " << filename << std::endl;
        return false;
    }
    ma_uint64 totalFrames {0};
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    size_t totalSamples = totalFrames * m_channels;

    m_pcmData.clear();
    m_pcmData.resize(totalFrames);
    ma_uint64 framesRead {0};
    ma_decoder_read_pcm_frames(&decoder, m_pcmData.data(),totalFrames, &framesRead);
    if(framesRead != totalFrames){
        std::cerr << "Warning: Read less data than expected for " << filename << std::endl;
        return false;
    }
    ma_decoder_uninit(&decoder);
    m_playbackSampleIndex.store(0, std::memory_order_relaxed);
    m_visualSampleIndex.store(0, std::memory_order_relaxed);
}

size_t Audio::readSamples(float *pOutput, size_t samplesToRead){
    size_t currentIndex = m_playbackSampleIndex.load(std::memory_order_relaxed);
    if(currentIndex >= m_pcmData.size() || m_pcmData.empty()){
        return 0;
    }

    size_t availableSample = m_pcmData.size() - currentIndex;
    size_t actualRead = std::min(samplesToRead, availableSample);

    std::copy(m_pcmData.data() + currentIndex, m_pcmData.data() + currentIndex + actualRead, pOutput);
    m_playbackSampleIndex.fetch_add(actualRead, std::memory_order_relaxed);
    return actualRead;
}

// void Audio::s_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
// {
//     // Get the 'this' pointer we stored in pUserData
//     Audio* pAudio = (Audio*)pDevice->pUserData;
//     if (pAudio) {
//         pAudio->data_callback(pOutput, frameCount);
//     }
// }

// void Audio::data_callback(void* pOutput, ma_uint32 frameCount)
// {
//     float* pOutputF32 = (float*)pOutput;
//     ma_uint32 samplesToRead = frameCount * m_channels;
    
//     if (m_playbackSampleIndex + samplesToRead > m_pcmData.size()) {
//         // --- End of song ---
//         ma_uint32 remainingSamples = m_pcmData.size() - m_playbackSampleIndex;
//         std::copy(m_pcmData.data() + m_playbackSampleIndex, m_pcmData.data() + m_pcmData.size(), pOutputF32);
        
//         // Fill the rest with silence
//         for (ma_uint32 i = remainingSamples; i < samplesToRead; ++i) {
//             pOutputF32[i] = 0.0f;
//         }
//         m_playbackSampleIndex += remainingSamples; // Stop at the end
//     } else {
//         // --- Normal playback ---
//         std::copy(m_pcmData.data() + m_playbackSampleIndex, m_pcmData.data() + m_playbackSampleIndex + samplesToRead, pOutputF32);
//         m_playbackSampleIndex += samplesToRead;
//     }
// }