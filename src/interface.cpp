#include "interface.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <codecvt>
#include <locale>
#include <string>
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"

std::string formatTime(sf::Time time) {
    int total_seconds = static_cast<int>(time.asSeconds());
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;

    std::ostringstream oss;
    oss << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
    return oss.str();
}

using namespace ftxui;

void Interface::displayUI() {
    auto screen = ScreenInteractive::TerminalOutput();

    int playlistSelected = 0;
    MenuOption menuOption;
    menuOption.entries_option.transform = [&](const EntryState& s) {
        bool isCurrentPlaylist = (s.index == musicPlayer.currentPlaylistIndex);
        // std::string label = (isActivePlaylist ? "· " : "  ") + s.label;

        Element prefix = text(isCurrentPlaylist ? "> " : "  ");
        Element t = isCurrentPlaylist ? text(s.label) | color(Color::Gold1) : text(s.label);

        // if (s.active) t |= underlined;
        if (s.focused) t |= underlined;
        return hbox({prefix, t});
    };

    auto tab_menu = Menu(&musicPlayer.playlistNames, &playlistSelected, menuOption);

    bool renderHelpPage = false;


    RadioboxOption radioboxOption;
    radioboxOption.transform = [&](const EntryState& s) {
        // Get the currently visible playlist name
        std::string activePlaylist = musicPlayer.playlistNames[playlistSelected];
    
        // Get the song at this entry
        bool isCurrentSong = 
            (musicPlayer.currentPlaylist == activePlaylist &&
                s.index == musicPlayer.currentTrackIndex);
        // std::cout << s.label << " vs " << musicPlayer.currentTrack + ".mp3" << std::endl;
    
        Element prefix = text(isCurrentSong ? "> " : "  ");
        std::string entry = std::filesystem::path(s.label).stem().string();
        Element t = isCurrentSong ? text(entry) | color(Color::Gold1) : text(entry);
        // if (s.active) t |= underlined;
        if (s.focused) t |= underlined;
        return hbox({prefix, t});
    };
   
    std::vector<Component> radioboxes;
    // Corresponding selected indices
    std::vector<int> selected_indices;

    // Populate radioboxes and indices
    for (const auto& name : musicPlayer.playlistNames) {
        selected_indices.push_back(0); // default selected index per playlist
        radioboxes.push_back(Radiobox(&musicPlayer.playlists[name], &selected_indices.back(), radioboxOption));
    }

    auto songs = Container::Tab(radioboxes, &playlistSelected);

    auto top_section = Container::Horizontal({
        tab_menu,
        songs,
    });

    auto player_section = Renderer([&] {
        std::string progress = formatTime(musicPlayer.music.getPlayingOffset()) +
                       " / " +
                       formatTime(musicPlayer.music.getDuration());
        std::string track = musicPlayer.currentTrack.empty() ? "No track loaded" : musicPlayer.currentTrack;
        std::string volume = "> Vol: " + std::to_string(static_cast<int>(musicPlayer.vol));

        return renderHelpPage ? (
            vbox({
                text("Controls:"),
                text("[p] Play/Pause    [s] Shuffle        [l] Loop  [m] Mute   "),
                text("[+] Volume Up     [-] Volume Down    [n] Next  [b] Previous"),
                hbox({
                text("[f] Seek Forward  [r] Seek Backward  [q] Quit") | flex,
                text("[esc] Return") | flex | align_right
                }),
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
        );
    });
    
    auto layout = Container::Vertical({
        top_section,
        player_section,
    });
   
    auto renderer = Renderer(layout, [&] {
        return vbox({
            hbox({
                tab_menu->Render(),
                separatorHeavy(),
                songs->Render() | yframe | size(HEIGHT, LESS_THAN, 5),
            }),
            separatorHeavy(),
            player_section->Render()
        }) | borderHeavy;
    });

    // Event handler: reacts to keypresses
    auto keyHandler = CatchEvent(renderer, [&](const Event& event) {
        if ((event == Event::Escape) && renderHelpPage) renderHelpPage = false;
        else if (event == Event::Character('p')) musicPlayer.togglePlay();
        else if (event == Event::Character('n')) {
            musicPlayer.nextTrack();
            selected_indices[playlistSelected] = min(selected_indices[playlistSelected]+1, (int)(musicPlayer.playlists[musicPlayer.currentPlaylist]).size()-1);
        }
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

    std::atomic<bool> running = true;
    std::thread refresher([&] {
        while (running) {
            screen.PostEvent(Event::Custom);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });

    screen.Loop(keyHandler);

    running = false;
    refresher.join();
}



