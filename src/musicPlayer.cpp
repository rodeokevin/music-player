#include "musicPlayer.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <filesystem>
#include <fstream>
#include <string>
#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>

std::string getArtist(const std::string &filePath) {
    TagLib::FileRef f(filePath.c_str());
    if (!f.isNull() && f.tag()) {
        return f.tag()->artist().to8Bit(true);
    }
    return "Unknown Artist";
}

std::string getTitle(const std::string &filePath) {
    TagLib::FileRef f(filePath.c_str());
    if (!f.isNull() && f.tag()) {
        return f.tag()->title().toCString(true);
    }
    return "Unknown Title";
}

// bool MusicPlayer::isPlaying() const {
//     std::lock_guard<std::mutex> lock(musicMutex);
//     return music.getStatus() == sf::Music::Playing;
// }

void MusicPlayer::loadData(std::string musicFolder, std::string playlistPath) {
    std::lock_guard<std::mutex> lock(musicMutex);
    musicFolder = musicFolder;
    std::ifstream file{playlistPath};
    if (!file) {
        return;
    }
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
    currentPlaylist = playlistNames.size() > 0 ? playlistNames[0] : "";
}

void MusicPlayer::shufflePlaylist() {
    std::lock_guard<std::mutex> lock(musicMutex);
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

void MusicPlayer::shufflePlaylistInternal() {
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
    } else {
        toLoad = musicFolder + playlists[currentPlaylist][currentTrackIndex];
    }
    if (!music.openFromFile(toLoad)) {
        return;
    }
    currentTrack = getTitle(toLoad);
    currentArtist = getArtist(toLoad);
}

void MusicPlayer::loadTrackInternal() {
    std::string toLoad;
    if (isShuffled) {
        toLoad = musicFolder + playlists[currentPlaylist][playOrder[shuffledTrackIndex]];
    } else {
        toLoad = musicFolder + playlists[currentPlaylist][currentTrackIndex];
    }
    if (!music.openFromFile(toLoad)) {
        return;
    }
    currentTrack = getTitle(toLoad);
    currentArtist = getArtist(toLoad);
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
    std::lock_guard<std::mutex> lock(musicMutex);
    // If not shuffled
    if (!isShuffled) {
        // If the playlist is looped
        if (isPlaylistLooped) {
            currentTrackIndex = (currentTrackIndex + 1) % playlists[currentPlaylist].size();
            loadTrackInternal();
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
                loadTrackInternal();
                firstLoad = true;
                isPaused = true;
            }
            else {
                currentTrackIndex++;
                loadTrackInternal();
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
            shufflePlaylistInternal();
            shuffledTrackIndex = 0;
            loadTrackInternal();
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
            loadTrackInternal();
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

void MusicPlayer::nextTrackInternal() {
    // If not shuffled
    if (!isShuffled) {
        // If the playlist is looped
        if (isPlaylistLooped) {
            currentTrackIndex = (currentTrackIndex + 1) % playlists[currentPlaylist].size();
            loadTrackInternal();
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
                loadTrackInternal();
                firstLoad = true;
                isPaused = true;
            }
            else {
                currentTrackIndex++;
                loadTrackInternal();
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
            shufflePlaylistInternal();
            shuffledTrackIndex = 0;
            loadTrackInternal();
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
            loadTrackInternal();
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
    std::lock_guard<std::mutex> lock(musicMutex);
    // If not shuffled
    if (!isShuffled) {
        // Only go to previous if not already at start of playlist
        if (currentTrackIndex != 0) {
            currentTrackIndex--;
            loadTrackInternal();
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
            loadTrackInternal();
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
    std::lock_guard<std::mutex> lock(musicMutex);
    isShuffled = !isShuffled;
}

void MusicPlayer::togglePlaylistLoop() {
    std::lock_guard<std::mutex> lock(musicMutex);
    isPlaylistLooped = !isPlaylistLooped;
}

void MusicPlayer::toggleTrackLoop() {
    std::lock_guard<std::mutex> lock(musicMutex);
    isTrackLooped = !isTrackLooped;
}

void MusicPlayer::increaseVol() {
    std::lock_guard<std::mutex> lock(musicMutex);
    vol = std::min(vol + 5.0f, 100.0f);
    music.setVolume(vol);
}

void MusicPlayer::decreaseVol() {
    std::lock_guard<std::mutex> lock(musicMutex);
    vol = std::max(vol - 5.0f, 0.0f);
    music.setVolume(vol);
}

void MusicPlayer::toggleMute() {
    std::lock_guard<std::mutex> lock(musicMutex);
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

void MusicPlayer::update() {
    std::lock_guard<std::mutex> lock(musicMutex);
    if (music.getStatus() == sf::Music::Status::Stopped && !firstLoad && !isPaused) {
        if (isTrackLooped) {
            music.play();
        } else {
            nextTrackInternal();
        }
    }
}

void MusicPlayer::playLoadedTrack() {
    std::lock_guard<std::mutex> lock(musicMutex);
    music.play();
}
