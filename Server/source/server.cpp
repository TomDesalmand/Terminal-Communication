// Header Includes //
#include "server.hpp"
#include "packetSerializer.hpp"

// STD Includes //
#include <thread>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <algorithm>

Server* Server::_instance = nullptr;

void Server::signalHandler(int signal) {
    static_cast<void>(signal);
    if (_instance) {
        _instance->shutdown();
    }
}

Server::Server(int port) : _port(port), _running(true) {
    _instance = this;
    std::signal(SIGINT, Server::signalHandler);

    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);

    if (bind(_serverSocket, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cout << "[SERVER] Socket bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(_serverSocket, 10) < 0) {
        std::cout << "[SERVER] Socket listening failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "[SERVER] Listening on port " << _port << "...\n";
}

void Server::run() {
    acceptClients();
}

void Server::acceptClients() {
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    while (_running) {
        int clientSocket = accept(_serverSocket, (sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0 && _running) {
            std::cout << "[SERVER] Client accept failed.\n";
            continue;
        }

        Client client(clientSocket);

        {
            std::lock_guard<std::mutex> lock(_clientMutex);
            _clients.push_back(std::move(client));
            Client* clientPtr = &_clients.back();

            clientPtr->getClientThread() = std::thread(&Server::handleClient, this, clientPtr);
            clientPtr->getClientThread().detach();
        }

        if (_running) {
            std::cout << "[SERVER] Client connected.\n";
        }
    }
}


void Server::handleClient(Client* client) {
    int clientSocket = client->getClientSocket();
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));

    int nameLen = read(clientSocket, buffer, sizeof(buffer));
    if (nameLen <= 0) {
        close(clientSocket);
        return;
    }

    std::string username(buffer);
    username.erase(std::remove(username.begin(), username.end(), '\n'), username.end());
    client->setUsername(username);

    std::cout << "[SERVER] User connected: " << username << "\n";

    std::vector<uint8_t> fragmentBuffer;
    std::string messageBuffer;
    bool complete = false;

    while (_running) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));

        if (bytesRead <= 0) {
            break;
        }

        fragmentBuffer.insert(fragmentBuffer.end(), buffer, buffer + bytesRead);

        while (PacketSerializer::deserialize(fragmentBuffer, messageBuffer, complete)) {
            if (complete) {
                std::string broadcastMsg = "[" + username + "]: " + messageBuffer + "\n";
                std::cout << broadcastMsg;
                broadcastMessage(broadcastMsg, clientSocket);
                messageBuffer.clear();
            }
        }
    }

    close(clientSocket);
    std::cout << "[SERVER] User disconnected: " << username << "\n";

    std::lock_guard<std::mutex> lock(_clientMutex);
    _clients.erase(
        std::remove_if(_clients.begin(), _clients.end(),
            [clientSocket](const Client& client) {
                return client.getClientSocket() == clientSocket;
            }),
        _clients.end()
    );
}

void Server::broadcastMessage(const std::string& message, int senderSocket) {
    std::lock_guard<std::mutex> lock(_clientMutex);
    for (Client& client : _clients) {
        if (client.getClientSocket() != senderSocket) {
            auto packets = PacketSerializer::serialize(message);
            for (const auto& packet : packets) {
                send(client.getClientSocket(), packet.data(), packet.size(), 0);
            }
        }
    }
}

void Server::shutdown() {
    if (!_running){
        return;     
    }
    _running = false;
    std::cout << "[SERVER] Shutting down...\n";

    {
        std::lock_guard<std::mutex> lock(_clientMutex);
        for (Client& client : _clients) {
            close(client.getClientSocket());
        }
        _clients.clear();
    }

    if (_serverSocket >= 0) {
        close(_serverSocket);
    }

    std::cout << "[SERVER] Shutdown complete.\n";
}

Server::~Server() {
    shutdown();
}