#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <SFML/Audio.hpp>
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <mutex>

class MusicPlayer {
private:
    // General data
    std::string musicFolder = "./music/";
    std::vector<std::string> files; // Vector of the .mp3 files

    // Music player
    sf::Music music;

    // Bools for state
    bool isPaused = true;
    bool isShuffled = false;
    bool isPlaylistLooped = false;
    bool isTrackLooped = false;
    bool isMuted = false;
    bool firstLoad = true;


    float currentPosition{0.0f};
    float duration{0.0f};

    // Volume values
    float prevVol = 100.0f;
    float vol = 100.0f;

    // Mutex
    mutable std::mutex musicMutex;
    
    // Playlist data
    std::string currentPlaylist{""};
    int currentPlaylistIndex{0};
    std::vector<size_t> playOrder;
    std::vector<std::string> playlistNames;
    std::unordered_map<std::string, std::vector<std::string>> playlists;
    std::vector<std::string> currentPlaylistTracks;

    // Track data
    int currentTrackIndex{0};
    int shuffledTrackIndex{-1};
    std::string currentTrack{""};
    std::string currentArtist{""};

    void nextTrackInternal(); // Only used for update to prevent deadlock
    void loadTrackInternal(); // Only used for nextTrack to prevent deadlock
    void shufflePlaylistInternal(); // Only used for nextTrack to prevent deadlock
    
public:

    MusicPlayer(){}
    ~MusicPlayer() {
        std::lock_guard<std::mutex> lock(musicMutex);
        music.stop();
    }
    // bool isPlaying() const;
    void loadData(std::string musicFolder, std::string playlistPath); // Load the data at beginning
    void shufflePlaylist(); // Shuffles the current playlist
    void loadTrack(bool setTrack = false); // loads the track into the sf::Music object to prepare for playing
    void togglePlay(); // Play/pause
    void nextTrack();
    void prevTrack();
    void increaseVol();
    void decreaseVol();
    void toggleShuffle();
    void togglePlaylistLoop();
    void toggleTrackLoop();
    void toggleMute();
    void seekForward();
    void seekBackward();
    void update(); // Checks if we need to start playing the next track once the current track is done
    void playLoadedTrack();

    // Getters
    float getVol() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return vol" << std::endl;
        return vol;
    }
    float getPrevVol() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return prevVol" << std::endl;
        return prevVol;
    }
    sf::Time getDuration() const{
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return duration" << std::endl;
        return music.getDuration();
    }
    sf::Time getCurrentPosition() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return position" << std::endl;
        return music.getPlayingOffset();
    }
    bool getPaused() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return isPaused" << std::endl;
        return isPaused;
    }
    bool getShuffled() const { 
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return isShuffled" << std::endl;
        return isShuffled;
    }
    bool getPlaylistLooped() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return isLooped" << std::endl;
        return isPlaylistLooped;
    }
    bool getTrackLoop() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return isTrackLooped" << std::endl;
        return isTrackLooped;
    }
    bool getMuted() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return isMuted" << std::endl;
        return isMuted;
    }
    bool getFirstLoad() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return firstLoad" << std::endl;
        return firstLoad;
    }

    std::string getCurrentPlaylist() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return current playlist" << std::endl;
        return currentPlaylist;
    }
    int getCurrentPlaylistIndex() const {
        std::cerr << "[musicPlayer] trying to return curr playlist index (pre-lock)" << std::endl;
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return curr playlist index (post-lock)" << std::endl;
        return currentPlaylistIndex;
    }
    int getCurrentTrackIndex() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return current track index" << std::endl;
        return currentTrackIndex;
    }
    int getShuffledTrackIndex() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return shuffled track index" << std::endl;
        return shuffledTrackIndex;
    }
    std::string getCurrentTrack() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return current track" << std::endl;
        return currentTrack;
    }
    std::string getCurrentArtist() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return current artist" << std::endl;
        return currentArtist;
    }

    std::vector<std::string> getPlaylistNames() const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return playlist names" << std::endl;
        return playlistNames;
    }

    std::vector<std::string> getPlaylistTracks(const std::string& playlistName) const {
        std::lock_guard<std::mutex> lock(musicMutex);
        std::cerr << "[musicPlayer] return playlist tracks" << std::endl;
        auto it = playlists.find(playlistName);
        if (it != playlists.end()) {
            return it->second; // Returns a copy
        }
        return {};
    }

    // Setters
    void setCurrentPosition(float pos);
    void setCurrentPlaylist(const std::string &playlist) {
        std::lock_guard<std::mutex> lock(musicMutex);
        currentPlaylist = playlist;
    }
    void setCurrentPlaylistIndex(int playlistIndex) {
        std::lock_guard<std::mutex> lock(musicMutex);
        currentPlaylist = playlistIndex;
    }
    void setCurrentTrackIndex(int index) {
        std::lock_guard<std::mutex> lock(musicMutex);
        currentTrackIndex = index;
    }
    void setShuffle(bool shuffle) {
        std::lock_guard<std::mutex> lock(musicMutex);
        isShuffled = shuffle;
    }
    void setPlaylistLooped(bool looped) {
        std::lock_guard<std::mutex> lock(musicMutex);
        isPlaylistLooped = looped;
    }
    void setFirstLoad(bool firstLoad) {
        std::lock_guard<std::mutex> lock(musicMutex);
        firstLoad = firstLoad;
    }
    void setPaused(bool paused) {
        std::lock_guard<std::mutex> lock(musicMutex);
        isPaused = paused;
    }
};

#endif
