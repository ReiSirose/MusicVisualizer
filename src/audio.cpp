#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "kiss_fftr.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "audio.h"

Audio::Audio(const char* filename, uint32_t sampleRate, int nfftSize): m_cfg(nullptr), m_nfft(nfftSize), m_currentSampleIndex(0){
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
}

Audio::~Audio(){
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

