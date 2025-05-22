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

    MusicPlayer musicPlayer("./music/");
    // CLInterface clinterface(musicPlayer);
    // std::cout << "test0" << std::endl;
    musicPlayer.loadData();
    // std::cout << "test1" << std::endl;
    musicPlayer.currentPlaylist = musicPlayer.playlistNames[musicPlayer.currentPlaylistIndex];
    // std::cout << "test2" << std::endl;
    // musicPlayer.loadPlaylist();
    musicPlayer.shufflePlaylist();
    musicPlayer.loadTrack();
    // musicPlayer.loadTrack();
    musicPlayer.poll();
    // clinterface.displayUI();
    Interface interface(musicPlayer);
    interface.displayUI();
    /*
    char c;
     while (true) {
         std::cout << "Shuffled: " << musicPlayer.isShuffled << ", "
                   << "Looped: " << musicPlayer.isPlaylistLooped << ", "
                   << "Paused: " << musicPlayer.isPaused << ", "
                   << "Current index: " << musicPlayer.currentTrackIndex << ", "
                   << "Shuffled index: " << musicPlayer.shuffledTrackIndex << std::endl;
                   for (auto i : musicPlayer.playOrder) {
                     std::cout << i << ", ";
                   }
                   std::cout << std::endl;
         c = _getch();
         switch (c) {
             // Play/Pause
             case 'p':
                 musicPlayer.togglePlay();
                 break;
             // Next track
             case 'n':
                 musicPlayer.nextTrack();
                 break;
             // Previous track
             case 'b':
                 musicPlayer.prevTrack();
                 break;
             // Shuffle
             case 's':
                 musicPlayer.toggleShuffle();
                 break;
             // Loop playlist
             case 'l':
                 musicPlayer.togglePlaylistLoop();
                 break;
             // Volume up (5%)
             case '+':
                 musicPlayer.increaseVol();
                 break;
             // Volume down (5%)
             case '-':
                 musicPlayer.decreaseVol();
                 break;
             // Mute/Unmute
             case 'm':
                 musicPlayer.toggleMute();
                 break;
             // Seek forward (5s)
             case 'f':
                 musicPlayer.seekForward();
                 break;
             // Seek backward (5s)
             case 'r':
                 musicPlayer.seekBackward();
                 break;
             // Quit the program
             case 'q':
                 musicPlayer.quit();
                 return 0;
     
             default:
                 std::cout << "Unknown command" << std::endl;
                 break;
         }
     }
    */
    
 
     // musicPlayer.quit();
     return 0;

}
