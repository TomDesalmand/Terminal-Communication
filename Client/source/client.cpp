// Header Files Includes //
#include "client.hpp"
#include "packetSerializer.hpp"

// STD Includes //
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

Client::Client(const std::string& serverIp, int port)
    : _serverIp(serverIp), _port(port), _socket(-1), _running(true) {}

void Client::run() {
    connectToServer();
    sendUsername();
    _receiverThread = std::thread(&Client::receiveMessages, this);
    sendMessages();
}

void Client::connectToServer() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);

    if (inet_pton(AF_INET, _serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid server address\n";
        exit(EXIT_FAILURE);
    }

    if (connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }
}

void Client::sendUsername() {
    std::cout << "Enter your username: ";
    std::getline(std::cin, _username);
    std::string userMsg = _username + "\n";
    send(_socket, userMsg.c_str(), userMsg.size(), 0);
}

void Client::sendMessages() {
    std::string input;
    while (_running) {
        std::getline(std::cin, input);
        if (input == "exit") {
            _running = false;
            std::string exitMsg = "exit\n";
            send(_socket, exitMsg.c_str(), exitMsg.size(), 0);
            break;
        }

        std::string formattedMsg = "[" + _username + "]: " + input + "\n";
        auto packets = PacketSerializer::serialize(input);

        for (const auto& packet : packets) {
            send(_socket, packet.data(), packet.size(), 0);
        }

        std::cout << "\33[A\33[2K\r" << formattedMsg << std::flush;
    }
}

void Client::receiveMessages() {
    char buffer[1024];
    std::string messageBuffer;
    while (_running) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(_socket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            std::cout << "[Disconnected from server]\n";
            _running = false;
            break;
        }

        std::vector<uint8_t> fragment(buffer, buffer + bytesRead);
        bool complete = false;
        if (PacketSerializer::deserialize(fragment, messageBuffer, complete)) {
            if (complete) {
                std::cout << messageBuffer;
                messageBuffer.clear();
            }
        }
    }
}

Client::~Client() {
    _running = false;
    if (_receiverThread.joinable())
        _receiverThread.join();
    if (_socket >= 0)
        close(_socket);
    std::cout << "[Client exited]\n";
}