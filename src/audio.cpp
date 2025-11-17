#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "kiss_fftr.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "audio.h"


Audio::Audio(const char* filename, uint32_t sampleRate, int nfftSize): 
    m_cfg(nullptr), 
    m_nfft(nfftSize), 
    m_currentSampleIndex(0),
    m_sampleRate(sampleRate),
    m_channels(1),
    m_playbackSampleIndex(0)
{
    ma_decoder decoder;
    ma_result result;

    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, sampleRate);
    result = ma_decoder_init_file(filename, &config, &decoder);
    if (result != MA_SUCCESS) {
        std::cerr << "Error: Failed to load audio file: " << filename << std::endl;
        return;
    }

    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    ma_uint64 totalSamples = totalFrames * decoder.outputChannels;
    m_pcmData.resize(totalSamples);

    ma_uint64 framesRead = 0;
    ma_decoder_read_pcm_frames(&decoder, m_pcmData.data(), totalFrames, &framesRead);

    if (framesRead != totalFrames) {
        std::cerr << "Warning: Read less data than expected." << std::endl;
    }
    ma_decoder_uninit(&decoder);

    m_cfg = kiss_fftr_alloc(nfftSize, 0 , nullptr, nullptr);
    if (m_cfg == nullptr) {
        std::cerr << "Error: Failed to allocate Kiss FFT configuration." << std::endl;
        return;
    }

    m_fftIn.resize(m_nfft);
    m_fftOut.resize(m_nfft / 2 + 1);
    m_frequencyOut.resize(m_nfft / 2 + 1);

    // Initialize audio devices 
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = ma_format_f32;
    deviceConfig.playback.channels = m_channels;
    deviceConfig.sampleRate        = m_sampleRate;
    deviceConfig.dataCallback      = s_data_callback;
    deviceConfig.pUserData         = this; // Pass 'this' pointer
    if (ma_device_init(NULL, &deviceConfig, &m_device) != MA_SUCCESS) {
        std::cerr << "Error: Failed to initialize playback device." << std::endl;
        return;
    }

    if (ma_device_start(&m_device) != MA_SUCCESS) {
        std::cerr << "Error: Failed to start playback device." << std::endl;
        ma_device_uninit(&m_device);
        return;
    }

    std::cout << "Audio playback started." << std::endl;
}

Audio::~Audio(){
    ma_device_uninit(&m_device);
    kiss_fft_free(m_cfg);
}

void Audio::Update(){
    if(!m_cfg ||m_pcmData.empty()){
        return;
    }

    if(m_currentSampleIndex + m_nfft > m_pcmData.size()){
        std::fill(m_frequencyOut.begin(), m_frequencyOut.end(), 0.0f);
        return;
    }

    std::copy(m_pcmData.begin() + m_currentSampleIndex, m_pcmData.begin() + m_currentSampleIndex + m_nfft, m_fftIn.data());

    m_currentSampleIndex += m_nfft;

    kiss_fftr(m_cfg, m_fftIn.data(), m_fftOut.data());
    
    for(size_t i = 0; i < m_frequencyOut.size(); ++i){
        float real {m_fftOut.at(i).r};
        float imagine {m_fftOut.at(i).i};
        m_frequencyOut[i] = std::sqrt(real * real + imagine * imagine);
    }

}


void Audio::s_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    // Get the 'this' pointer we stored in pUserData
    Audio* pAudio = (Audio*)pDevice->pUserData;
    if (pAudio) {
        pAudio->data_callback(pOutput, frameCount);
    }
}

void Audio::data_callback(void* pOutput, ma_uint32 frameCount)
{
    float* pOutputF32 = (float*)pOutput;
    ma_uint32 samplesToRead = frameCount * m_channels;

    if (m_playbackSampleIndex + samplesToRead > m_pcmData.size()) {
        // --- End of song ---
        ma_uint32 remainingSamples = m_pcmData.size() - m_playbackSampleIndex;
        std::copy(m_pcmData.data() + m_playbackSampleIndex, m_pcmData.data() + m_pcmData.size(), pOutputF32);
        
        // Fill the rest with silence
        for (ma_uint32 i = remainingSamples; i < samplesToRead; ++i) {
            pOutputF32[i] = 0.0f;
        }
        m_playbackSampleIndex += remainingSamples; // Stop at the end
    } else {
        // --- Normal playback ---
        std::copy(m_pcmData.data() + m_playbackSampleIndex, m_pcmData.data() + m_playbackSampleIndex + samplesToRead, pOutputF32);
        m_playbackSampleIndex += samplesToRead;
    }
}