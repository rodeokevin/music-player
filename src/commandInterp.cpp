#include "commndInterp.h"

#include <iostream>
#include <regex>
#include <sstream>

using namespace std;

CommandType CommandInterpreter::parseCommand(char c) {
    switch (c) {
        case 'p':
            return CommandType::Play;
        case 'n':
            return CommandType::Next;
        case 'b':
            return CommandType::Previous;
        case 's':
            return CommandType::Shuffle;
        case 'l':
            return CommandType::LoopSong;
        case '+':
            return CommandType::IncreaseVol;
        case '-':
            return CommandType::DecreaseVol;
        case 'm':
            return CommandType::Mute;
        default:
            return CommandType::Unknown;
    }
}
