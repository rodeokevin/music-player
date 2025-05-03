#ifndef CLINTERFACE_H
#define CLINTERFACE_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <thread>
#include "musicPlayer.h"

class CLInterface {
    MusicPlayer& musicPlayer;

    std::thread uiThread;
    std::atomic<bool> isRunning = false;
    void UILoop();

    public:
    bool isHelpPage = false;
    CLInterface(MusicPlayer& player):musicPlayer(player) {}

    void displayUI();
};

#endif
