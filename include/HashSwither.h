#ifndef HASHSWITCHER_H
#define HASHSWITCHER_H
#include <iostream>
#include <string_view>
#include <cstdint>
#include <Msg.h>
class ProcessMsgSwitcher 
{
    bool ProcessMsg(CE::tcp::Msg& msg)
    {
        uint32_t inputId = msg.GetMsgID();
        switch (inputId) 
        {
            case StringSwitcher::hash("ValidateIVCmd"):
                std::cout << "Action: Starting the process.\n";
                return true;
            default:
                std::cout << "Action: Unknown command received.\n";
                return false;
        }
    }

};

class StringSwitcher {
public:
    // FNV-1a 32-bit hash engine running at compile time or runtime
    static constexpr uint32_t hash(std::string_view str) 
    {
        uint32_t hash = 2166136261u;
        for (char c : str) {
            hash ^= static_cast<uint32_t>(c);
            hash *= 16777619u;
        }
        return hash;
    }

    // Executes action based on the string hash match
    void processCommand(std::string_view command) {
        // Runtime hashing of the input variable
        switch (hash(command)) {
            // Compile-time hashing of the case string literals
            case hash("start"):
                std::cout << "Starting the engine...\n";
                break;
                
            case hash("stop"):
                std::cout << "Stopping the system...\n";
                break;
                
            case hash("pause"):
                std::cout << "System paused.\n";
                break;
                
            default:
                std::cout << "Unknown command: " << command << "\n";
                break;
        }
    }
};


#endif // HASHSWITCHER_H