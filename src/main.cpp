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
    // std::cerr << "[DEBUG] MusicPlayer created" << std::endl;

    musicPlayer.loadData("./music/", "playlists.txt");
    // std::cerr << "[DEBUG] loadData completed" << std::endl;

    musicPlayer.shufflePlaylist();
    // std::cerr << "[DEBUG] shufflePlaylist completed" << std::endl;

    musicPlayer.loadTrack();
    // std::cerr << "[DEBUG] loadTrack completed" << std::endl;

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
        // std::cerr << "[DEBUG] getCurrentPlaylistIndex completed" << std::endl;
        
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
    // std::cerr << "[DEBUG] getPlaylistNames completed" << std::endl;
    
    auto tab_menu = Menu(&playlistNamesCopy, &playlistSelected, menuOption);

    bool renderHelpPage = false;

    int songsIndex = 0;
    
    RadioboxOption radioboxOption;
    radioboxOption.transform = [&](const EntryState& s) {
        // Get the currently visible playlist name
        std::string activePlaylist = playlistNamesCopy[playlistSelected];
        // std::cerr << "[DEBUG] Got activePlaylist: " << activePlaylist << std::endl;
    
        // Get the song at this entry
        std::string currentPlaylist = musicPlayer.getCurrentPlaylist();
        // std::cerr << "[DEBUG] getCurrentPlaylist completed" << std::endl;
        
        int currentTrackIndex = musicPlayer.getCurrentTrackIndex();
        // std::cerr << "[DEBUG] getCurrentTrackIndex completed" << std::endl;
        
        bool isCurrentSong = 
            (currentPlaylist == activePlaylist && s.index == currentTrackIndex);
        // std::cout << s.label << " vs " << musicPlayer.currentTrack + ".mp3" << std::endl;
    
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
   
    std::vector<Component> radioboxes;
    // Corresponding selected indices
    std::vector<int> selected_indices;

    // Populate radioboxes and indices
    std::vector<std::vector<std::string>> playlistTracksCopies;
    for (const auto& name : playlistNamesCopy) {
        selected_indices.push_back(0);
        std::vector<std::string> tracks = musicPlayer.getPlaylistTracks(name);
        // std::cerr << "[DEBUG] getPlaylistTracks completed for: " << name << std::endl;
        
        playlistTracksCopies.push_back(tracks); // Obtain a copy in a thread safe manner
        radioboxes.push_back(Radiobox(&playlistTracksCopies.back(), &selected_indices.back(), radioboxOption));
    }

    auto songs = Container::Tab(radioboxes, &playlistSelected);

    auto top_section = Container::Horizontal({
        tab_menu,
        songs,
    });

    auto player_section = Renderer([&] {
        std::string progress;
        sf::Time currentPosition = musicPlayer.getCurrentPosition();
        // std::cerr << "[DEBUG] getCurrentPosition completed" << std::endl;
        
        sf::Time duration = musicPlayer.getDuration();
        // std::cerr << "[DEBUG] getDuration completed" << std::endl;
        
        progress = formatTime(currentPosition) + " / " + formatTime(duration);
        
        std::string track = musicPlayer.getCurrentTrack();
        // std::cerr << "[DEBUG] getCurrentTrack completed" << std::endl;
        
        if (track.empty()) track = "No track loaded";
        
        std::string artist = musicPlayer.getCurrentArtist();
        // std::cerr << "[DEBUG] getCurrentArtist completed" << std::endl;
        
        float vol = musicPlayer.getVol();
        // std::cerr << "[DEBUG] getVol completed" << std::endl;
        
        std::string volume = "> Vol: " + std::to_string(static_cast<int>(vol));

        bool paused = musicPlayer.getPaused();
        // std::cerr << "[DEBUG] getPaused completed" << std::endl;
        
        bool shuffled = musicPlayer.getShuffled();
        // std::cerr << "[DEBUG] getShuffled completed" << std::endl;
        
        bool playlistLooped = musicPlayer.getPlaylistLooped();
        // std::cerr << "[DEBUG] getPlaylistLooped completed" << std::endl;

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
                // text(musicPlayer.currentTrack.empty() ? "No track loaded" : musicPlayer.currentTrack) | color(Color::Gold1),
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
        // std::cerr << "[DEBUG] finished updating UI??" << std::endl;
        return result;
    });

    // Event handler: reacts to keypresses
    auto keyHandler = CatchEvent(renderer, [&](const Event& event) {
        if ((event == Event::Escape) && renderHelpPage) {
            renderHelpPage = false;
            // std::cerr << "[DEBUG] Help page closed" << std::endl;
        }
        else if (event == Event::Character('p')) {
            musicPlayer.togglePlay();
            // std::cerr << "[DEBUG] togglePlay completed" << std::endl;
        }
        else if (event == Event::Character('n')) {
            musicPlayer.nextTrack();
            // std::cerr << "[DEBUG] nextTrack completed" << std::endl;
            
            // Not sure if this is needed?
            std::string currentPlaylist = musicPlayer.getCurrentPlaylist();
            // std::cerr << "[DEBUG] getCurrentPlaylist completed after nextTrack" << std::endl;
            
            auto currentPlaylistTracks = musicPlayer.getPlaylistTracks(currentPlaylist);
            // std::cerr << "[DEBUG] getPlaylistTracks completed after nextTrack" << std::endl;
            
            selected_indices[playlistSelected] = std::min(selected_indices[playlistSelected]+1, (int)currentPlaylistTracks.size()-1);
            // std::cerr << "[DEBUG] UI selection updated after nextTrack" << std::endl;
        }
        else if (event == Event::Character('b')) {
            musicPlayer.prevTrack();
            // std::cerr << "[DEBUG] prevTrack completed" << std::endl;
        }
        else if (event == Event::Character('s')) {
            musicPlayer.toggleShuffle();
            // std::cerr << "[DEBUG] toggleShuffle completed" << std::endl;
        }
        else if (event == Event::Character('l')) {
            musicPlayer.togglePlaylistLoop();
            // std::cerr << "[DEBUG] togglePlaylistLoop completed" << std::endl;
        }
        else if (event == Event::Character('+')) {
            musicPlayer.increaseVol();
            // std::cerr << "[DEBUG] increaseVol completed" << std::endl;
        }
        else if (event == Event::Character('-')) {
            musicPlayer.decreaseVol();
            // std::cerr << "[DEBUG] decreaseVol completed" << std::endl;
        }
        else if (event == Event::Character('m')) {
            musicPlayer.toggleMute();
            // std::cerr << "[DEBUG] toggleMute completed" << std::endl;
        }
        else if (event == Event::Character('f')) {
            musicPlayer.seekForward();
            // std::cerr << "[DEBUG] seekForward completed" << std::endl;
        }
        else if (event == Event::Character('r')) {
            musicPlayer.seekBackward();
            // std::cerr << "[DEBUG] seekBackward completed" << std::endl;
        }
        else if (event == Event::Character('h')) {
            renderHelpPage = true;
            // std::cerr << "[DEBUG] Help page opened" << std::endl;
        }
        else if (event == Event::Return) {
            // Pressed enter on a playlist
            if (tab_menu->Focused()) {
                // std::string prevPlaylist = musicPlayer.getCurrentPlaylist();
                int prevPlaylistIndex = musicPlayer.getCurrentPlaylistIndex();
                playlistSelected = playlistIndex;
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
            // std::cerr << "[DEBUG] Screen exit called" << std::endl;
        }
        else {
            // std::cerr << "[DEBUG] Not a registered key" << std::endl;
        }
        return false;
    });

    std::atomic<bool> running = true;

    // Loop loop(&screen, keyHandler);

    // while (!loop.HasQuitted()) {
    //     loop.RunOnce();
    //     screen.RequestAnimationFrame();
    //     musicPlayer.update();
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }

    std::thread updateThread([&] {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            musicPlayer.update();
            screen.RequestAnimationFrame();
            // std::cerr << "[DEBUG] requested new animation frame" << std::endl;
        }
    });

    // std::cerr << "[DEBUG] Starting FTXUI loop" << std::endl;
    screen.Loop(keyHandler);
    // std::cerr << "[DEBUG] FTXUI loop ended" << std::endl;

    running = false;
    updateThread.join();
    // std::cerr << "[DEBUG] Update thread joined" << std::endl;
    // std::cerr << "[DEBUG] Program ending normally" << std::endl;
    return 0;
}

