#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <musicPlayer.h>
 
class Interface {

    MusicPlayer &musicPlayer; // Reference to the MusicPlayer object

    public:
    Interface(MusicPlayer &musicPlayer) : musicPlayer(musicPlayer) {}
    ~Interface() = default;
    void clearScreen();
    void drawUI();
    void drawProgress();
};

#endif