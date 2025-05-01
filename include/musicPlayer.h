#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <SFML/Audio.hpp>
#include <string>
#include <filesystem>
#include <vector>
#include <stack>
#include <thread>
#include <atomic>
#include <mutex>

using namespace std;
namespace fs = std::filesystem;


class MusicPlayer {

    std::mutex musicMutex;

    const std::string musicFolder = "./music";
    std::vector<fs::path> playlist; // Vector of paths to the music files
    

    sf::Music music;

    std::thread pollThread;
    std::atomic<bool> isRunning;
    void pollLoop();      // Function run by the thread

    public:

    std::vector<size_t> playOrder;

    bool isPaused = true; // Start as paused
    bool isShuffled = false; // Start non-shuffled
    bool isPlaylistLooped = false; // Start non-looped
    bool isTrackLooped = false; // Start non-looped
    bool isMuted = false; // Start non-muted
    bool firstLoad = true;
    float prevVol = 100.0f;
    float vol = 100.0f;
    
    int currentTrackIndex = 0;
    int shuffledTrackIndex = -1;

    MusicPlayer(const std::string &musicFolder):musicFolder{musicFolder}{};
    ~MusicPlayer() { quit(); }
    void loadPlaylist();
    void shufflePlaylist();
    void loadTrack();
    void togglePlay();
    void nextTrack();
    void prevTrack();
    void toggleShuffle();
    void togglePlaylistLoop();
    void toggleTrackLoop();
    void increaseVol();
    void decreaseVol();
    void toggleMute();
    void seekForward();
    void seekBackward();
    void poll();
    void quit();

};

#endif