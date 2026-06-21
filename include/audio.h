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
    Audio(uint32_t sampleRate, int fftSize);
    ~Audio();

    void Update();
    bool reload(const std::string& filename);
    size_t readSamples(float* pOutput, size_t sampleToRead);

    kiss_fftr_cfg& getConfig() {return m_cfg;}
    int getNFFT () const {return m_nfft;}
    const std::vector<float>& getPCM () const {return m_pcmData;}
    size_t  getSampleIndex() {return m_playbackSampleIndex.load(std::memory_order_relaxed);}
    const std::vector<float>& getFrequencyMagnitude () const {
        size_t frontIdx = m_frontBufferIndex.load(std::memory_order_acquire);
        return m_frequencyBuffers[frontIdx];
    }

    void setSampleIndex(size_t index) {m_playbackSampleIndex.store(index, std::memory_order_relaxed);}

private:
    // FFT member variable
    kiss_fftr_cfg m_cfg;
    int m_nfft;
    std::vector<float> m_pcmData;
    std::vector <kiss_fft_scalar> m_fftIn;
    std::vector <kiss_fft_cpx> m_fftOut;

    // Audio Playback member variable
    ma_uint32 m_sampleRate;
    ma_uint32 m_channels;
    std::atomic<size_t> m_playbackSampleIndex;    
    std::atomic<size_t> m_visualSampleIndex;

    // Lock-free primitive
    static constexpr size_t NUM_BUFFERS {2};
    std::array <std::vector<float>, NUM_BUFFERS> m_frequencyBuffers;
    std::atomic<size_t> m_frontBufferIndex;

    // static void s_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    // // The C++ member function callback
    // void data_callback(void* pOutput, ma_uint32 frameCount);
};

#endif