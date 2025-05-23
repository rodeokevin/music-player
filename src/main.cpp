#include <SFML/Audio.hpp>
#include <filesystem>
#include <iostream>
#include <vector>
#include <string>
#include "musicPlayer.h"
#include "interface.h"


int main() {

    MusicPlayer musicPlayer("./music/"); 
    musicPlayer.loadData();
    musicPlayer.currentPlaylist = musicPlayer.playlistNames[musicPlayer.currentPlaylistIndex];
    musicPlayer.shufflePlaylist();
    musicPlayer.loadTrack();
    musicPlayer.poll();
    Interface interface(musicPlayer);
    interface.displayUI();
     return 0;

}
