#ifndef COMMAND_INTERPRETER_H
#define COMMAND_INTERPRETER_H

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <regex>
#include <sstream>

using namespace std;

enum class CommandType {
    Play,
    Next,
    Previous,
    Shuffle,
    LoopSong,
    IncreaseVol,
    DecreaseVol,
    Mute,
    Unknown
};

class CommandInterpreter {

    const unordered_set<string> commands = {
        "p", // Play/Pause
        "n", // Next Track
        "b", // Previous Track
        "s", // Shuffle
        "l", // Loop song
        "+", // Increase volume by 5%
        "-", // Decrease violume by 5%
        "m" // Mute
    };

    public:
    CommandInterpreter() {};
    ~CommandInterpreter() = default;

    CommandType parseCommand(char c);
};

#endif
