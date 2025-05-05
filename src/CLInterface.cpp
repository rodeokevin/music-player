#include "CLInterface.h"
#include <iostream>
#include <codecvt>
#include <locale>
#include <string>

using namespace ftxui;
/*

*/
std::string aformatTime(sf::Time time) {
    int total_seconds = static_cast<int>(time.asSeconds());
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;

    std::ostringstream oss;
    oss << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
    return oss.str();
}

void CLInterface::displayUI() {

    bool renderHelpPage = false;

    int playlistIndex = 0;
    int songIndex = 0;

    auto screen = ScreenInteractive::TerminalOutput();

    // FOR TESTING
    int selected = 0;

    // Renderer: defines UI layout
    auto renderer = Renderer([&] {
        std::string progress = aformatTime(musicPlayer.music.getPlayingOffset()) +
                       " / " +
                       aformatTime(musicPlayer.music.getDuration());
        std::string track = musicPlayer.currentTrack.empty() ? "No track loaded" : musicPlayer.currentTrack;
        std::string volume = "> Vol: " + std::to_string(static_cast<int>(musicPlayer.vol));

        auto playlistMenu = Radiobox(&musicPlayer.playlistNames, &playlistIndex);
        auto playlistRenderer = Renderer(playlistMenu, [&] {
            return vbox({
                text("Playlists"),
                playlistMenu->Render(),
            });
        });

        auto songMenu = Radiobox(&musicPlayer.currentPlaylistTracks, &songIndex);
        auto songRenderer = Renderer(songMenu, [&] {
            return vbox({
                text("Tracks"),
            });
        });


        // TESTING PURPOSES
        std::vector<std::string> entries = {
            "entry 1",
            "entry 2",
            "entry 3",
        };
        
        auto testContent = Menu(&entries, &selected);
        auto mainContent = vbox({
            // Top: Library Section (Playlists + Tracks)
            /*
            hbox({
                playlistRenderer->Render() | flex,
                separatorHeavy(),
                songRenderer->Render() | flex,
            }) | flex,
        
            separatorHeavy(),
            */
            
            
            // Bottom: Player or Help Page
            renderHelpPage ? (
                vbox({
                    text("Controls:"),
                    text("[p] Play/Pause     [s] Shuffle         [l] Loop"),
                    text("[+] Volume Up      [-] Volume Down     [m] Mute"),
                    text("[n] Next           [b] Previous  "),
                    text("[f] Seek Forward   [r] Seek Backward"),
                    text("[h] Help           [q] Quit"),
                    separator(),
                    text("[esc] Return to main page"),
                }) | flex
            ) : (
                vbox({
                    text(musicPlayer.currentTrack.empty() ? "No track loaded" : musicPlayer.currentTrack) | color(Color::Gold1),
                    text("> " + progress),
                    text("> " + std::string(musicPlayer.isPaused ? "P" : " ") + " " +
                                std::string(musicPlayer.isShuffled ? "S" : " ") + " " +
                                std::string(musicPlayer.isPlaylistLooped ? "L" : " ")),
                    hbox({
                        text(volume) | flex,
                        text("[h] Help") | flex | align_right,
                    }),
                }) | flex
            )
        }) | borderHeavy;

        return vbox({
            testContent->Render()
        }) | borderHeavy;
    });

    // Event handler: reacts to keypresses
    auto keyHandler = CatchEvent(renderer, [&](const Event& event) {
        if ((event == Event::Escape) && renderHelpPage) renderHelpPage = false;
        else if (event == Event::Character('p')) musicPlayer.togglePlay();
        else if (event == Event::Character('n')) musicPlayer.nextTrack();
        else if (event == Event::Character('b')) musicPlayer.prevTrack();
        else if (event == Event::Character('s')) musicPlayer.toggleShuffle();
        else if (event == Event::Character('l')) musicPlayer.togglePlaylistLoop();
        else if (event == Event::Character('+')) musicPlayer.increaseVol();
        else if (event == Event::Character('-')) musicPlayer.decreaseVol();
        else if (event == Event::Character('m')) musicPlayer.toggleMute();
        else if (event == Event::Character('f')) musicPlayer.seekForward();
        else if (event == Event::Character('r')) musicPlayer.seekBackward();
        else if (event == Event::Character('h')) renderHelpPage = true;
        else if (event == Event::Character('q')) {
            musicPlayer.quit();
            screen.Exit();    // exit FTXUI loop
        } else {
            return false;
        }
        return true;
    });

    // Refresher thread: triggers UI re-render every 200ms
    std::atomic<bool> running = true;
    std::thread refresher([&] {
        while (running) {
            screen.PostEvent(ftxui::Event::Custom);  // triggers redraw
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });

    screen.Loop(keyHandler);  // Blocks until screen.Exit() is called

    running = false;
    refresher.join();
}

