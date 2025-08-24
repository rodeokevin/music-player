cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# FTXUI
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG main)
FetchContent_MakeAvailable(ftxui)

# TagLib configuration - disable problematic formats
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
set(BUILD_BINDINGS OFF CACHE BOOL "Build bindings" FORCE)

# Disable OGG support to avoid linker issues
set(WITH_VORBIS OFF CACHE BOOL "Enable Vorbis support" FORCE)
set(WITH_FLAC OFF CACHE BOOL "Enable FLAC support" FORCE)
set(WITH_OPUS OFF CACHE BOOL "Enable Opus support" FORCE)
set(WITH_SPEEX OFF CACHE BOOL "Enable Speex support" FORCE)

# Keep essential formats
set(WITH_MP4 ON CACHE BOOL "Enable MP4 support" FORCE)
set(WITH_ASF ON CACHE BOOL "Enable ASF support" FORCE)

add_subdirectory(external/taglib)

add_executable(main src/main.cpp src/musicPlayer.cpp src/interface.cpp)

target_include_directories(main PRIVATE include 
                                        external/taglib/taglib
                                        external/taglib/taglib/toolkit
                                        external/taglib/taglib/mpeg
                                        external/taglib/taglib/mpeg/id3v1
                                        external/taglib/taglib/mpeg/id3v2) 

target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics 
                                   SFML::Audio
                                   ftxui::screen
                                   ftxui::dom
                                   ftxui::component
                                   tag)