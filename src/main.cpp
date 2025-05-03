#include <SFML/Audio.hpp>
#include <filesystem>
#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include "musicPlayer.h"
// #include "CLInterface.h"
#include "interface.h"


int main() {

    MusicPlayer musicPlayer("./music");
    // CLInterface clinterface(musicPlayer);
    musicPlayer.loadData();
    musicPlayer.loadPlaylist();
    musicPlayer.shufflePlaylist();
    musicPlayer.loadTrack();
    musicPlayer.poll();
    // clinterface.displayUI();
    Interface interface(musicPlayer);
    interface.displayUI();
    
    return 0;

}
