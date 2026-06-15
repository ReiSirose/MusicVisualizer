#ifndef SONG_H
#define SONG_H
#include <vector>
#include <string>
#include <string_view>
#include <array>
#include <memory>
#include <atomic>
#include <future>
#include "audio.h"

enum class CacheState {Empty, Loading, Ready};
enum class Direction {Forward, Backward, None};


class Song {
public:
    Song(std::string_view playListPath);
    ~Song();

    void loadAudio(bool cache = false);
    void nextSong ();
    void prevSong ();
    Audio* currentSong();

private:
    void prefetchWorker(size_t targetSongIndex);
    std::vector<std::string> m_playList;
    std::string m_playListPath;

    static constexpr size_t CACHE_SIZE {5};
    std::array<std::unique_ptr<Audio>, CACHE_SIZE> m_audioList;
    std::array<std::atomic<CacheState>, CACHE_SIZE> m_slotState;

    std::unique_ptr<Audio> m_swapSpace;
    std::future<void> m_prefetchTask;

    int m_swapSpaceSongIndex;
    size_t m_playListIndex;
    size_t m_totalSong;
    Direction m_direction;

};


#endif

