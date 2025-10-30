#include "kiss_fftr.h"
#include <vector>


class Audio {
public:
    Audio();
    ~Audio();

    // getter
    kiss_fftr_cfg& getConfig() {return m_cfg;}
    const std::vector<float>& getFrequencyOut () {return m_frequencyOut;}
    int getNFFT () {return m_nfft;}
    const std::vector<float>& getPCM () {return m_pcmData;}

private:
    kiss_fftr_cfg m_cfg;
    int m_nfft;
    std::vector<float> m_pcmData;
    std::vector<float> m_frequencyOut;
};