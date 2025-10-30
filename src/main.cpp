#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <iostream>
#include <vector>
#include <cmath>

#include "kiss_fftr.h"
#define NFFT 1024

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


/**
 * @brief Loads a specified audio file into a float vector buffer.
 * * @param filename The path to the audio file (e.g., "music.wav").
 * @param sampleRate The desired output sample rate (e.g., 44100).
 * @return std::vector<float> Vector containing the audio samples (-1.0 to 1.0).
 */
std::vector<float> load_audio(const char* filename, ma_uint32 sampleRate) {
    ma_decoder decoder;
    ma_result result;
    
    // Configure decoder to output single-channel (mono), 32-bit float samples
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, sampleRate);
    
    result = ma_decoder_init_file(filename, &config, &decoder);
    if (result != MA_SUCCESS) {
        std::cerr << "Error: Failed to load audio file: " << filename << std::endl;
        return {};
    }

    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    ma_uint64 totalSamples = totalFrames * decoder.outputChannels;

    std::vector<float> audioBuffer(totalSamples);
    
    ma_uint64 framesRead = 0;
    ma_decoder_read_pcm_frames(&decoder, audioBuffer.data(), totalFrames, &framesRead);

    if (framesRead != totalFrames) {
        std::cerr << "Warning: Read less data than expected." << std::endl;
    }

    ma_decoder_uninit(&decoder);
    std::cout << "Successfully loaded " << totalSamples << " samples from " << filename << std::endl;
    return audioBuffer;
}


int main(int argc, char* argv[]) {
    std::string audioPath = "flower_thief.mp3";
    ma_uint32 sampleRate = 44100;
    std::vector<float> audioData = load_audio(audioPath.c_str(), sampleRate);

    if (audioData.empty()) {
        std::cerr << "Exiting due to audio loading error." << std::endl;
        return 1;
    }

    // 2. Kiss FFT Setup (Using Real FFT: kiss_fftr)
    kiss_fftr_cfg cfg = kiss_fftr_alloc(NFFT, 0 /* is_inverse_fft=0 */, nullptr, nullptr);

    if (cfg == nullptr) {
        std::cerr << "Error: Failed to allocate Kiss FFT configuration." << std::endl;
        return 1;
    }
    
    // --- FFT Input/Output Buffers ---
    // FFT Input: NFFT real samples
    kiss_fft_scalar input_frame[NFFT];
    
    // FFT Output: (NFFT/2 + 1) complex frequency bins
    kiss_fft_cpx output_spectrum[NFFT / 2 + 1];

    size_t offset_in_seconds = 5;
    size_t sample_offset = offset_in_seconds * sampleRate;
    // 3. Process the first frame (NFFT samples)
    if (audioData.size() >= sample_offset + NFFT) {
        std::copy(
            audioData.begin() + sample_offset, 
            audioData.begin() + sample_offset + NFFT, 
            input_frame
        );

        // Perform the FFT (Time Domain -> Frequency Domain)
        kiss_fftr(cfg, input_frame, output_spectrum);
        
        // 4. Calculate and Display Magnitudes
        std::cout << "\nFrequency Spectrum (First Frame):\n";
        for (int i = 0; i < NFFT / 2 + 1; ++i) {
            // Magnitude = sqrt(Real^2 + Imaginary^2)
            float magnitude = std::sqrt(
                output_spectrum[i].r * output_spectrum[i].r + 
                output_spectrum[i].i * output_spectrum[i].i
            );
            
            // Calculate the actual frequency for this bin
            float freq = (float)i * sampleRate / NFFT;

            // This magnitude is the data you map to visual elements!
            if (i < 10 || i % 50 == 0) { // Print only a few bins to avoid spam
                 std::cout << "Bin " << i << " (" << freq << " Hz): Magnitude = " << magnitude << std::endl;
            }
        }
    } else {
        std::cerr << "Audio file is too short to process one FFT frame (" << NFFT << " samples needed)." << std::endl;
    }

    kiss_fft_free(cfg);
    
    std::cout << "\nAudio processing complete. You now have the fundamental frequency data." << std::endl;

    return 0;
}
