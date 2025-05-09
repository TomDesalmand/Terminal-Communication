#pragma once

// Header Includes //
#include "client.hpp"

// STD Includes //
#include <vector>
#include <mutex>
#include <atomic>

class Server {
public:
    Server(int port);
    ~Server();

    void run();
    void shutdown();
    static void signalHandler(int signal);

private:
    void acceptClients();
    void handleClient(Client* client);
    void broadcastMessage(const std::string& message, int senderSocket);

    int _port;
    int _serverSocket;

    std::vector<Client> _clients;
    std::mutex _clientMutex;
    std::atomic<bool> _running;

    static Server* _instance;
};
