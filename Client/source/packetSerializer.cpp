// Header Files Includes //
#include "packetSerializer.hpp"

std::vector<std::vector<uint8_t>> PacketSerializer::serialize(const std::string& message) {
    std::vector<std::vector<uint8_t>> packets;
    size_t totalLen = message.size();
    size_t offset = 0;

    while (offset < totalLen) {
        size_t chunkSize = std::min(static_cast<const size_t>(255), totalLen - offset);
        bool isLast = (offset + chunkSize) == totalLen;

        std::vector<uint8_t> fragment;
        fragment.push_back(isLast ? 1 : 0);
        fragment.push_back(static_cast<uint8_t>(chunkSize));
        fragment.insert(fragment.end(), message.begin() + offset, message.begin() + offset + chunkSize);

        packets.push_back(std::move(fragment));
        offset += chunkSize;
    }

    return packets;
}

bool PacketSerializer::deserialize(std::vector<uint8_t>& buffer, std::string& accumulated, bool& isComplete) {
    while (buffer.size() >= 2) {
        bool isLast = buffer[0];
        uint8_t len = buffer[1];

        if (buffer.size() < 2 + len)
            break;

        accumulated.append(reinterpret_cast<const char*>(buffer.data() + 2), len);
        buffer.erase(buffer.begin(), buffer.begin() + 2 + len);

        if (isLast) {
            isComplete = true;
            return true;
        }
    }

    isComplete = false;
    return false;
}