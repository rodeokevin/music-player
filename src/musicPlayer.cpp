#include "musicPlayer.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <filesystem>

// Load the playlist
void MusicPlayer::loadPlaylist() {
    for (const auto& entry : std::filesystem::directory_iterator(musicFolder)) {
        if (entry.path().extension() == ".mp3") {
            playlist.push_back(entry.path());
        }
    }  
}

void MusicPlayer::shufflePlaylist() {
    int n = playlist.size();
    playOrder.resize(n);
    for (int i = 0; i < playOrder.size(); i++) {
        playOrder[i] = i;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(playOrder.begin(), playOrder.end(), gen);

    shuffledTrackIndex = -1;
}

void MusicPlayer::loadTrack() {
    std::lock_guard<std::mutex> lock(musicMutex);
    if (isShuffled) {
        music.openFromFile(playlist[playOrder[shuffledTrackIndex]].string());
    }
    else {
        music.openFromFile(playlist[currentTrackIndex].string());
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
            currentTrackIndex = (currentTrackIndex + 1) % playlist.size();
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
            if (currentTrackIndex == playlist.size()-1) {
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
        if (shuffledTrackIndex == playlist.size()-1) {
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
    vol = music.getVolume();
    music.setVolume(std::min(vol + 5.0f, 100.0f));
}

void MusicPlayer::decreaseVol() {
    vol = music.getVolume();
    music.setVolume(std::max(vol - 5.0f, 0.0f));
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

