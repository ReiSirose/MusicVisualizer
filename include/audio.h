#ifndef AUDIO_H
#define AUDIO_H
#include <vector>
#include <cstdint>

#include "kiss_fft.h"
struct kiss_fftr_state;
using kiss_fftr_cfg = kiss_fftr_state*;

class Audio {
public:
    Audio(const char* filename, uint32_t sampleRate, int fftSize);
    ~Audio();

    void Update();

    // getter
    kiss_fftr_cfg& getConfig() {return m_cfg;}
    const std::vector<float>& getFrequencyMagnitude () const {return m_frequencyOut;}
    int getNFFT () const {return m_nfft;}
    const std::vector<float>& getPCM () const {return m_pcmData;}

private:
    kiss_fftr_cfg m_cfg;
    int m_nfft;
    size_t m_currentSampleIndex;


    std::vector<float> m_pcmData;
    std::vector<float> m_frequencyOut;

    std::vector <kiss_fft_scalar> m_fftIn;
    std::vector <kiss_fft_cpx> m_fftOut;
};

#endif