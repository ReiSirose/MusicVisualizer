#ifndef SONG_H
#define SONG_H
#include <vector>
#include <string>
#include <string_view>
#include <array>
#include <memory>
#include <atomic>
#include <future>


enum class CacheState {Empty, Loading, Ready};
enum class Direction {Forward, Backward, None};

class Audio;

class Song {
public:
    Song(std::string_view playListPath);
    ~Song();

    void loadAudio(bool cache = false);
    void nextSong ();
    void prevSong ();
    void togglePlay();
    void setVolume(float volume);

    Audio* currentSong();

private:
    static void s_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    void data_callback(void* pOutput, ma_uint32 frameCount);
    void prefetchWorker(size_t targetSongIndex);

    std::vector<std::string> m_playList;
    std::string m_playListPath;

    static constexpr size_t CACHE_SIZE {6};
    std::array<std::unique_ptr<Audio>, CACHE_SIZE> m_audioList;
    std::array<std::atomic<CacheState>, CACHE_SIZE> m_slotState;

    std::future<void> m_prefetchTask;

    size_t m_playListIndex;
    size_t m_totalSong;
    Direction m_direction;

    ma_device m_device;
    std::atomic<bool> m_isPlaying;
    float m_volume;
};


#endif

