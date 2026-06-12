#ifndef SONG_H
#define SONG_H
#include <vector>
#include <string>
#include "audio.h"
class Song {
public:
    Song(std::string_view playListPath);
    ~Song();

    void loadAudio(bool cache = false);
    void nextSong ();
    void prevSong ();
    auto& currentSong() {return m_audioList.at(m_playListIndex);}

private:
    std::vector<std::string> m_playList;
    std::string m_playListPath;
    std::vector<Audio> m_audioList;
    size_t m_playListIndex;
    size_t m_totalSong;
};


#endif


/*
Plan is to read the folder and parse it into m_playList

*/

