#pragma once

// STD Includes //
#include <string>
#include <vector>
#include <cstdint>

class PacketSerializer {
    public:
        static std::vector<std::vector<uint8_t>> serialize(const std::string& message);
        static bool deserialize(std::vector<uint8_t>& buffer, std::string& accumulated, bool& isComplete);
};