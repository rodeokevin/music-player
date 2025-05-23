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
#include <unordered_map>

using namespace std;
namespace fs = std::filesystem;


class MusicPlayer {

    std::mutex musicMutex;

    const std::string musicFolder = "./music";
    std::vector<std::string> files; // Vector of the .mp3 files

    std::thread pollThread;
    std::atomic<bool> isRunning = false;
    void pollLoop();      // Function run by the thread

    public:

    sf::Music music;
    std::vector<size_t> playOrder;
    std::vector<std::string> playlistNames;
    std::unordered_map<std::string, std::vector<std::string>> playlists;
    
    std::vector<std::string> currentPlaylistTracks;

    bool isPaused = true; // Start as paused
    bool isShuffled = false; // Start non-shuffled
    bool isPlaylistLooped = false; // Start non-looped
    bool isTrackLooped = false; // Start non-looped
    bool isMuted = false; // Start non-muted
    bool firstLoad = true;
    float prevVol = 100.0f;
    float vol = 100.0f;
    
    std::string currentTrack = "";
    std::string currentPlaylist = "";
    int currentTrackIndex = 0;
    int currentPlaylistIndex = 0;
    int shuffledTrackIndex = -1;

    MusicPlayer(const std::string &musicFolder):musicFolder{musicFolder}{};
    ~MusicPlayer() { quit(); }
    void loadData(); // Load the data at beginning
    void shufflePlaylist();
    void loadTrack(bool setTrack = false);
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