#ifndef AUDIO_H
#define AUDIO_H
#include <vector>
#include <cstdint>
#include <atomic>
#include <mutex>
#include "miniaudio.h"
#include "kiss_fft.h"
struct kiss_fftr_state;
using kiss_fftr_cfg = kiss_fftr_state*;

class Audio {
public:
    Audio(const char* filename, uint32_t sampleRate, int fftSize);
    ~Audio();

    void Update();
    void setVolume(float volume);
    // getter
    kiss_fftr_cfg& getConfig() {return m_cfg;}
    const std::vector<float>& getFrequencyMagnitude () const {return m_frequencyOut;}
    int getNFFT () const {return m_nfft;}
    const std::vector<float>& getPCM () const {return m_pcmData;}

private:
    // FFT member variable
    kiss_fftr_cfg m_cfg;
    int m_nfft;
    // FFT member function
    std::vector<float> m_pcmData;
    std::vector<float> m_frequencyOut;
    std::vector <kiss_fft_scalar> m_fftIn;
    std::vector <kiss_fft_cpx> m_fftOut;

    // Audio Playback member variable
    ma_device m_device;
    ma_uint32 m_sampleRate;
    ma_uint32 m_channels;
    std::atomic<size_t> m_playbackSampleIndex;    
    size_t m_visualSampleIndex;
    // Audio Playback member function

    std::mutex m_mutex;
    static void s_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    // The C++ member function callback
    void data_callback(void* pOutput, ma_uint32 frameCount);
};

#endif