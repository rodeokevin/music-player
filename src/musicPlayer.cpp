#include "musicPlayer.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <filesystem>
#include <fstream>
#include <string>

void MusicPlayer::loadData() {
    std::ifstream file{"./music/playlists.txt"};
    std::string line;
    std::string current;
    while (std::getline(file, line)) {
        if (line.substr(0, 10) == ".PLAYLIST ") {
            std::string name = line.substr(10);
            playlistNames.push_back(name); // add a playlist
            current = name;
        }
        else {
            if (!line.empty()) {
                playlists[current].push_back(line); // add track to the corresponding playlist
            }
        }
    }
    // Get all the songs
    /*
    std::cout << "start" << std::endl;
    for (const auto& entry : std::filesystem::directory_iterator(musicFolder)) {
        std::cout << entry << std::endl;
        if (entry.path().extension() == ".mp3") {
            files.push_back(entry.path().string());
        }
    }
    std::cout << "end" << std::endl;
    */
    
}
/*
void MusicPlayer::loadPlaylistNames() {
    std::ifstream file{"./music/playlists.txt"};
    std::string line;
    playlistNames.clear();

    while (std::getline(file, line)) {
        if (line.substr(0, 10) == ".PLAYLIST ") {
            std::string name = line.substr(10);
            playlistNames.push_back(name);
        }
    }
}

void MusicPlayer::loadPlaylistTracks() {
    std::ifstream file{"./music/playlists.txt"};
    std::string line;
    bool found = false;

    currentPlaylistTracks.clear();

    while (std::getline(file, line)) {
        if (line.substr(0, 10) == ".PLAYLIST ") {
            if (!found) {
                found = (line.substr(10) == currentPlaylist);
            continue;
            }
            else break;
        }
        if (found && !line.empty()) {
            currentPlaylistTracks.push_back(line);
        }
    }
}
*/

void MusicPlayer::shufflePlaylist() {
    int n = playlists[currentPlaylist].size();
    playOrder.resize(n);
    for (int i = 0; i < playOrder.size(); i++) {
        playOrder[i] = i;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(playOrder.begin(), playOrder.end(), gen);

    shuffledTrackIndex = -1;
}

void MusicPlayer::loadTrack(bool setTrack) {
    std::lock_guard<std::mutex> lock(musicMutex);
    std::string toLoad;
    if (isShuffled && !setTrack) {
        toLoad = musicFolder + playlists[currentPlaylist][playOrder[shuffledTrackIndex]];
        music.openFromFile(toLoad);
        currentTrack = std::filesystem::path(toLoad).stem().string();
    }
    else {
        toLoad = musicFolder + playlists[currentPlaylist][currentTrackIndex];
        music.openFromFile(toLoad);
        currentTrack = std::filesystem::path(toLoad).stem().string();
    }
}


void MusicPlayer::togglePlay() {
    std::lock_guard<std::mutex> lock(musicMutex);
    if (isPaused) {
        music.play();
        firstLoad = false;
    } else {
        music.pause();
    }
    isPaused = !isPaused;
}

void MusicPlayer::nextTrack() {
    // If not shuffled
    if (!isShuffled) {
        // If the playlist is looped
        if (isPlaylistLooped) {
            currentTrackIndex = (currentTrackIndex + 1) % playlists[currentPlaylist].size();
            loadTrack();
            if (!isPaused) {
                music.play();
            }
            else {
                firstLoad = true;
            }
        }
        // If the playlist is not looped
        else {
            // End of playlist
            if (currentTrackIndex == playlists[currentPlaylist].size()-1) {
                currentTrackIndex = 0;
                loadTrack();
                firstLoad = true;
                isPaused = true;
            }
            else {
                currentTrackIndex++;
                loadTrack();
                if (!isPaused) {
                    music.play();
                }
                else {
                    firstLoad = true;
                }
            }
        }
        
    }
    // If shuffled
    else {
        // End of playlist
        if (shuffledTrackIndex == playlists[currentPlaylist].size()-1) {
            shufflePlaylist();
            shuffledTrackIndex = 0;
            loadTrack();
            // If the playlist is looped
            if (isPlaylistLooped && !isPaused) {
                music.play();
            }
            else {
                firstLoad = true;
                isPaused = true;
            }
        }
        // Not end of playlist
        else {
            shuffledTrackIndex++;
            loadTrack();
            if (!isPaused) {
                music.play();
            }
            else {
                firstLoad = true;
            }
        }
        currentTrackIndex = playOrder[shuffledTrackIndex];
    }
}

void MusicPlayer::prevTrack() {
    // If not shuffled
    if (!isShuffled) {
        // Only go to previous if not already at start of playlist
        if (currentTrackIndex != 0) {
            currentTrackIndex--;
            loadTrack();
            if (!isPaused) {
                music.play();
            }
            else {
                firstLoad = true;
            }
        }
    }
    // If shuffled
    else {
        // Only go to previous if not already at start of playlist
        if (shuffledTrackIndex != 0) {
            currentTrackIndex = playOrder[--shuffledTrackIndex];
            loadTrack();
            if (!isPaused) {
                music.play();
            }
            else {
                firstLoad = true;
            }
        }
    }
}

void MusicPlayer::toggleShuffle() {
    isShuffled = !isShuffled;
}

void MusicPlayer::togglePlaylistLoop() {
    isPlaylistLooped = !isPlaylistLooped;
}

void MusicPlayer::toggleTrackLoop() {
    isTrackLooped = !isTrackLooped;
}

void MusicPlayer::increaseVol() {
    vol = min(vol + 5.0f, 100.0f);
    music.setVolume(vol);
}

void MusicPlayer::decreaseVol() {
    vol = max(vol - 5.0f, 0.0f);
    music.setVolume(vol);
}

void MusicPlayer::toggleMute() {
    if (isMuted) {
        music.setVolume(prevVol);
        isMuted = !isMuted;
    }
    else {
        prevVol = music.getVolume();
        music.setVolume(0.0f);
        isMuted = !isMuted;
    }
}

void MusicPlayer::seekForward() {
    std::lock_guard<std::mutex> lock(musicMutex);
    music.setPlayingOffset(music.getPlayingOffset() + sf::seconds(5));
}

void MusicPlayer::seekBackward() {
    std::lock_guard<std::mutex> lock(musicMutex);
    sf::Time pos = music.getPlayingOffset() - sf::seconds(5);
    if (pos < sf::Time::Zero) pos = sf::Time::Zero;
    music.setPlayingOffset(pos);
}

void MusicPlayer::poll() {
    if (isRunning) return;
    isRunning = true;
    pollThread = std::thread(&MusicPlayer::pollLoop, this);
}

// Private
void MusicPlayer::pollLoop() {
    while (isRunning) {
        if (music.getStatus() == sf::Music::Status::Stopped && !firstLoad && !isPaused) {
            nextTrack();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void MusicPlayer::quit() {
    if (!isRunning) return;
    isRunning = false;
    if (pollThread.joinable())
        pollThread.join();
}

