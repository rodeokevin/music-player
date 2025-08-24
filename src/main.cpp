#include <SFML/Audio.hpp>
#include <filesystem>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include "musicPlayer.h"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/loop.hpp"
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

int main() {
    // ---------------------------------------------------------------------------
    // Music Player
    // ---------------------------------------------------------------------------
    MusicPlayer musicPlayer;

    musicPlayer.loadData("./music/", "playlists.txt");

    musicPlayer.shufflePlaylist();

    musicPlayer.loadTrack();

    using namespace ftxui;

    int playlistSelected = 0;
    int playlistIndex = 0;

    auto screen = ScreenInteractive::TerminalOutput();

    // ---------------------------------------------------------------------------
    // FTXUI
    // ---------------------------------------------------------------------------

    MenuOption menuOption;
    menuOption.entries_option.transform = [&](const EntryState& s) {
        int currentPlaylistIndex = musicPlayer.getCurrentPlaylistIndex();
        
        bool isCurrentPlaylist = (s.index == currentPlaylistIndex);
        // std::string label = (isActivePlaylist ? "· " : "  ") + s.label;

        Element prefix = text(isCurrentPlaylist ? "> " : "  ");
        Element t = isCurrentPlaylist ? text(s.label) | color(Color::Gold1) : text(s.label);

        // if (s.active) t |= underlined;
        if (s.focused) {
            t |= underlined;
            playlistIndex = s.index;
        }
        return hbox({prefix, t});
    };
    
    std::vector<std::string> playlistNamesCopy = musicPlayer.getPlaylistNames();
    
    auto tab_menu = Menu(&playlistNamesCopy, &playlistSelected, menuOption);

    bool renderHelpPage = false;

    int songsIndex = 0;
    
    RadioboxOption radioboxOption;
    radioboxOption.transform = [&](const EntryState& s) {
        int currentPlaylistIndex = musicPlayer.getCurrentPlaylistIndex();
        
        int currentTrackIndex = musicPlayer.getCurrentTrackIndex();
        
        // Compare's FTXUI's playlistSelected and the MusicPlayer's currentPlaylistIndex
        bool isCurrentSong = (playlistSelected == currentPlaylistIndex && s.index == currentTrackIndex);
    
        Element prefix = text(isCurrentSong ? "> " : "  ");
        std::string entry = std::filesystem::path(s.label).stem().string();
        Element t = isCurrentSong ? text(entry) | color(Color::Gold1) : text(entry);
        // if (s.active) t |= underlined;
        if (s.focused) {
            t |= underlined;
            songsIndex = s.index;
        }
        return hbox({prefix, t});
    };
    std::vector<Component> radioboxes(playlistNamesCopy.size());
    // Corresponding selected indices
    std::vector<int> selected_indices(playlistNamesCopy.size(), 0);

    // Populate radioboxes and indices
    std::vector<std::vector<std::string>> playlistsCopy(playlistNamesCopy.size());
    for (int i = 0; i < playlistNamesCopy.size(); i++) {
        playlistsCopy[i] = musicPlayer.getPlaylistTracks(i);
        radioboxes[i] = Radiobox(&playlistsCopy[i], &selected_indices[i], radioboxOption);
    }

    auto songs = Container::Tab(radioboxes, &playlistSelected);

    auto top_section = Container::Horizontal({
        tab_menu,
        songs,
    });

    auto player_section = Renderer([&] {
        std::string progress;
        sf::Time currentPosition = musicPlayer.getCurrentPosition();
        
        sf::Time duration = musicPlayer.getDuration();
        
        progress = formatTime(currentPosition) + " / " + formatTime(duration);
        
        std::string track = musicPlayer.getCurrentTrack();
        
        if (track.empty()) track = "No track loaded";
        
        std::string artist = musicPlayer.getCurrentArtist();
        
        float vol = musicPlayer.getVol();
        
        std::string volume = "> Vol: " + std::to_string(static_cast<int>(vol));

        bool paused = musicPlayer.getPaused();
        
        bool shuffled = musicPlayer.getShuffled();
        
        bool playlistLooped = musicPlayer.getPlaylistLooped();

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
                hbox({
                    text(track) | color(Color::Gold1),
                    text(" - "),
                    text(artist) | color(Color::Gold1)
                }),
                text("> " + progress),
                text("> " + std::string(paused ? "P" : " ") + " " +
                            std::string(shuffled ? "S" : " ") + " " +
                            std::string(playlistLooped ? "L" : " ")),
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

    std::atomic<bool> uiUpdateInProgress{false};
   
    auto renderer = Renderer(layout, [&] {
        auto result = vbox({
            hbox({
                tab_menu->Render() | yframe | size(HEIGHT, EQUAL, 5) | size(WIDTH, LESS_THAN, 15),
                separatorHeavy(),
                songs->Render() | yframe | size(HEIGHT, EQUAL, 5),
            }),
            separatorHeavy(),
            player_section->Render(),
        }) | borderHeavy;
        uiUpdateInProgress = false;
        return result;
    });

    // Event handler: reacts to keypresses
    auto keyHandler = CatchEvent(renderer, [&](const Event& event) {
        if ((event == Event::Escape) && renderHelpPage) {
            renderHelpPage = false;
        }
        else if (event == Event::Character('p')) {
            musicPlayer.togglePlay();
        }
        else if (event == Event::Character('n')) {
            musicPlayer.nextTrack();
            
            // Not sure if this is needed?
            int currentPlaylistIndex = musicPlayer.getCurrentPlaylistIndex();
            
            selected_indices[playlistSelected] = std::min(selected_indices[playlistSelected]+1, musicPlayer.getCurrentPlaylistSize()-1);
        }
        else if (event == Event::Character('b')) {
            musicPlayer.prevTrack();
        }
        else if (event == Event::Character('s')) {
            musicPlayer.toggleShuffle();
        }
        else if (event == Event::Character('l')) {
            musicPlayer.togglePlaylistLoop();
        }
        else if (event == Event::Character('+')) {
            musicPlayer.increaseVol();
        }
        else if (event == Event::Character('-')) {
            musicPlayer.decreaseVol();
        }
        else if (event == Event::Character('m')) {
            musicPlayer.toggleMute();
        }
        else if (event == Event::Character('f')) {
            musicPlayer.seekForward();
        }
        else if (event == Event::Character('r')) {
            musicPlayer.seekBackward();
        }
        else if (event == Event::Character('h')) {
            renderHelpPage = true;
        }
        else if (event == Event::Return) {
            // Pressed enter on a playlist
            if (tab_menu->Focused()) {
                // std::string prevPlaylist = musicPlayer.getCurrentPlaylist();
                int prevPlaylistIndex = musicPlayer.getCurrentPlaylistIndex();
                playlistIndex = playlistSelected;
                musicPlayer.setCurrentPlaylist(playlistNamesCopy[playlistSelected]);
                musicPlayer.setCurrentPlaylistIndex(playlistSelected);
                // Changes happen only if the playlist was different
                if (prevPlaylistIndex != playlistSelected) {
                    musicPlayer.setCurrentTrackIndex(0);
                    musicPlayer.setShuffle(false);
                    musicPlayer.setPlaylistLooped(false);
                    musicPlayer.shufflePlaylist();
                    musicPlayer.setFirstLoad(true);
                    musicPlayer.loadTrack();
                    musicPlayer.setPaused(true);
                }
            }
            // Pressed enter on a song
            else if (songs->Focused()) {
                // std::string prevPlaylist = musicPlayer.getCurrentPlaylist();
                int prevPlaylistIndex = musicPlayer.getCurrentPlaylistIndex();
                musicPlayer.setCurrentPlaylist(playlistNamesCopy[playlistSelected]);
                musicPlayer.setCurrentPlaylistIndex(playlistSelected);
                musicPlayer.setCurrentTrackIndex(songsIndex);
                // If the playlist changed
                if (prevPlaylistIndex != playlistSelected) {
                    musicPlayer.setShuffle(false);
                    musicPlayer.setPlaylistLooped(false);
                }
                musicPlayer.shufflePlaylist();
                musicPlayer.setFirstLoad(true);
                musicPlayer.loadTrack();
                if (musicPlayer.getPaused()) {
                    musicPlayer.togglePlay();
                }
                else {
                    musicPlayer.playLoadedTrack();
                    musicPlayer.setFirstLoad(false);
                }
            }
        }
        
        else if (event == Event::Character('q')) {
            screen.Exit();    // exit FTXUI loop
        }
        else {
        }
        return false;
    });

    std::atomic<bool> running = true;

    std::thread updateThread([&] {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            musicPlayer.update();
            screen.RequestAnimationFrame();
        }
    });

    screen.Loop(keyHandler);

    running = false;
    updateThread.join();
    return 0;
}

