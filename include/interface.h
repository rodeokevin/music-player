#ifndef INTERFACE_H
#define INTERFACE_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <thread>
#include <vector>
#include <string>
#include "musicPlayer.h"
 
class Interface {

    MusicPlayer& musicPlayer;

public:
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::TerminalOutput();
    //  = ScreenInteractive::TerminalOutput();
    std::vector<std::string> stuff;
    int currentSelection = 0;

    std::thread uiThread;
    std::atomic<bool> isRunning = false;

    bool isHelpPage = false;
    Interface(MusicPlayer& player):musicPlayer(player) {}
    void constructUI();
    void displayUI();
};

#endif