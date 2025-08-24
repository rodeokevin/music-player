#include "interface.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <locale>
#include <string>
#include <filesystem>
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"


using namespace ftxui;

// void Interface::displayUI() {
//     // auto screen = ScreenInteractive::TerminalOutput();

//     int playlistSelected = 0;
//     int playlistIndex = 0;

//     MenuOption menuOption;
//     menuOption.entries_option.transform = [&](const EntryState& s) {
//         bool isCurrentPlaylist = (s.index == musicPlayer.currentPlaylistIndex);
//         // std::string label = (isActivePlaylist ? "· " : "  ") + s.label;

//         Element prefix = text(isCurrentPlaylist ? "> " : "  ");
//         Element t = isCurrentPlaylist ? text(s.label) | color(Color::Gold1) : text(s.label);

//         // if (s.active) t |= underlined;
//         if (s.focused) {
//             t |= underlined;
//             playlistIndex = s.index;
//         }
//         return hbox({prefix, t});
//     };

//     auto tab_menu = Menu(&musicPlayer.playlistNames, &playlistSelected, menuOption);

//     bool renderHelpPage = false;

//     int songsIndex = 0;
    


//     RadioboxOption radioboxOption;
//     radioboxOption.transform = [&](const EntryState& s) {
//         // Get the currently visible playlist name
//         std::string activePlaylist = musicPlayer.playlistNames[playlistSelected];
    
//         // Get the song at this entry
//         bool isCurrentSong = 
//             (musicPlayer.currentPlaylist == activePlaylist &&
//                 s.index == musicPlayer.currentTrackIndex);
//         // std::cout << s.label << " vs " << musicPlayer.currentTrack + ".mp3" << std::endl;
    
//         Element prefix = text(isCurrentSong ? "> " : "  ");
//         std::string entry = std::filesystem::path(s.label).stem().string();
//         Element t = isCurrentSong ? text(entry) | color(Color::Gold1) : text(entry);
//         // if (s.active) t |= underlined;
//         if (s.focused) {
//             t |= underlined;
//             songsIndex = s.index;
//         }
//         return hbox({prefix, t});
//     };
   
//     std::vector<Component> radioboxes;
//     // Corresponding selected indices
//     std::vector<int> selected_indices;

//     // Populate radioboxes and indices
//     for (const auto& name : musicPlayer.playlistNames) {
//         selected_indices.push_back(0); // default selected index per playlist
//         radioboxes.push_back(Radiobox(&musicPlayer.playlists[name], &selected_indices.back(), radioboxOption));
//     }

//     auto songs = Container::Tab(radioboxes, &playlistSelected);

//     auto top_section = Container::Horizontal({
//         tab_menu,
//         songs,
//     });

//     auto player_section = Renderer([&] {
//         std::string progress;
//         {
//             std::lock_guard<std::mutex> lock(musicPlayer.musicMutex);
//             progress = formatTime(musicPlayer.music.getPlayingOffset()) +
//                         " / " +
//                         formatTime(musicPlayer.music.getDuration());
//         }
        
//         // Use static values instead
//         // std::string progress = "2:30 / 4:15";  // Static for testing
        
//         std::string track = musicPlayer.currentTrack.empty() ? "No track loaded" : musicPlayer.currentTrack;
//         std::string volume = "> Vol: " + std::to_string(static_cast<int>(musicPlayer.vol));

//         return renderHelpPage ? (
//             vbox({
//                 text("Controls:"),
//                 text("[p] Play/Pause    [s] Shuffle        [l] Loop  [m] Mute   "),
//                 text("[+] Volume Up     [-] Volume Down    [n] Next  [b] Previous"),
//                 hbox({
//                 text("[f] Seek Forward  [r] Seek Backward  [q] Quit") | flex,
//                 text("[esc] Return") | flex | align_right
//                 }),
//             }) | flex
//         ) : (
//             vbox({
//                 hbox({
//                     text(musicPlayer.currentTrack.empty() ? "No track loaded" : musicPlayer.currentTrack) | color(Color::Gold1),
//                     text(" - "),
//                     text(musicPlayer.currentArtist.empty() ? "Unknown Artist" : musicPlayer.currentArtist) | color(Color::Gold1)
//                 }),
//                 // text(musicPlayer.currentTrack.empty() ? "No track loaded" : musicPlayer.currentTrack) | color(Color::Gold1),
//                 text("> " + progress),
//                 text("> " + std::string(musicPlayer.isPaused ? "P" : " ") + " " +
//                             std::string(musicPlayer.isShuffled ? "S" : " ") + " " +
//                             std::string(musicPlayer.isPlaylistLooped ? "L" : " ")),
//                 hbox({
//                     text(volume) | flex,
//                     text("[h] Help") | flex | align_right,
//                 }),
//             }) | flex
//         );
//     });
    
//     auto layout = Container::Vertical({
//         top_section,
//         player_section,
//     });
   
//     auto renderer = Renderer(layout, [&] {
//         return vbox({
//             hbox({
//                 tab_menu->Render() | yframe | size(HEIGHT, EQUAL, 5) | size(WIDTH, LESS_THAN, 15),
//                 separatorHeavy(),
//                 songs->Render() | yframe | size(HEIGHT, EQUAL, 5),
//             }),
//             separatorHeavy(),
//             player_section->Render()
//         }) | borderHeavy;
//     });

//     // Event handler: reacts to keypresses
//     auto keyHandler = CatchEvent(renderer, [&](const Event& event) {
//         // std::cerr << "received an event!" << std::endl;
//         // if (event == Event::Custom) {
//         //     std::cerr << "received fake event" << std::endl;
//         //     bool shouldNext = false;
//         //     bool shouldReplay = false;

//         //     {
//         //         std::lock_guard<std::mutex> lock(musicPlayer.musicMutex);
//         //         if (musicPlayer.music.getStatus() == sf::Music::Status::Stopped &&
//         //             !musicPlayer.firstLoad && !musicPlayer.isPaused) 
//         //         {
//         //             shouldReplay = musicPlayer.isTrackLooped;
//         //             shouldNext   = !musicPlayer.isTrackLooped;
//         //         }
//         //     }

//         //     if (shouldReplay) {
//         //         std::lock_guard<std::mutex> lock(musicPlayer.musicMutex);
//         //         musicPlayer.music.play();
//         //     } else if (shouldNext) {
//         //         musicPlayer.nextTrack(); // nextTrack should also lock inside
//         //     }
//         //     std::cerr << "finished processing from fake event" << std::endl;
//         // }

//         if ((event == Event::Escape) && renderHelpPage) renderHelpPage = false;
//         else if (event == Event::Character('p')) musicPlayer.togglePlay();
//         else if (event == Event::Character('n')) {
//             musicPlayer.nextTrack();
//             selected_indices[playlistSelected] = std::min(selected_indices[playlistSelected]+1, (int)(musicPlayer.playlists[musicPlayer.currentPlaylist]).size()-1);
//         }
//         else if (event == Event::Character('b')) musicPlayer.prevTrack();
//         else if (event == Event::Character('s')) musicPlayer.toggleShuffle();
//         else if (event == Event::Character('l')) musicPlayer.togglePlaylistLoop();
//         else if (event == Event::Character('+')) musicPlayer.increaseVol();
//         else if (event == Event::Character('-')) musicPlayer.decreaseVol();
//         else if (event == Event::Character('m')) musicPlayer.toggleMute();
//         else if (event == Event::Character('f')) musicPlayer.seekForward();
//         else if (event == Event::Character('r')) musicPlayer.seekBackward();
//         else if (event == Event::Character('h')) renderHelpPage = true;
//         else if (event == Event::Return) {
//             // Pressed enter on a playlist
//             if (tab_menu->Focused()) {
//                 std::string prevPlaylist = musicPlayer.currentPlaylist;
//                 playlistSelected = playlistIndex;
//                 musicPlayer.currentPlaylist = musicPlayer.playlistNames[playlistSelected];
//                 musicPlayer.currentPlaylistIndex = playlistSelected;
//                 // Changes happen only if the playlist was different
//                 if (prevPlaylist != musicPlayer.currentPlaylist) {
//                     musicPlayer.currentTrackIndex = 0;
//                     if (musicPlayer.isShuffled) musicPlayer.toggleShuffle();
//                     if (musicPlayer.isPlaylistLooped) musicPlayer.togglePlaylistLoop();
//                     musicPlayer.shufflePlaylist();
//                     musicPlayer.firstLoad = true;
//                     musicPlayer.loadTrack(true);
//                     musicPlayer.isPaused = true;
//                 }
//             }
//             // Pressed enter on a song
//             else if (songs->Focused()) {
//                 std::string prevPlaylist = musicPlayer.currentPlaylist;
//                 musicPlayer.currentPlaylist = musicPlayer.playlistNames[playlistSelected];
//                 musicPlayer.currentPlaylistIndex = playlistSelected;
//                 musicPlayer.currentTrackIndex = songsIndex;
//                 if (prevPlaylist != musicPlayer.currentPlaylist) {
//                     if (musicPlayer.isShuffled) musicPlayer.toggleShuffle();
//                     if (musicPlayer.isPlaylistLooped) musicPlayer.togglePlaylistLoop();
//                 }
//                 musicPlayer.shufflePlaylist();
//                 musicPlayer.firstLoad = true;
//                 musicPlayer.loadTrack(true);
//                 if (musicPlayer.isPaused) {
//                     musicPlayer.togglePlay();
//                 }
//                 else {
//                     std::lock_guard<std::mutex> lock(musicPlayer.musicMutex);
//                     musicPlayer.music.play();
//                     musicPlayer.firstLoad = false;
//                 }
//             }
//         }
        
//         else if (event == Event::Character('q')) {
//             screen.Exit();    // exit FTXUI loop
//         } else {
//             return false;
//         }
//         return true;
//     });

//     // std::atomic<bool> running = true;
//     // std::thread refresher([&] {
//     //     while (running) {
//     //         screen.PostEvent(Event::Custom);
//     //         // musicPlayer.update();
//     //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     //     }
//     // });

//     screen.Loop(keyHandler);


//     // running = false;
//     // refresher.join();
// }



