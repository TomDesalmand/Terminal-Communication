#pragma once

// STD Includes //
#include <string>
#include <thread>
#include <atomic>

class Client {
    private:
        std::string _serverIp;
        int _port;
        int _socket;
        std::string _username;
        std::thread _receiverThread;
        std::atomic<bool> _running;

        void connectToServer();
        void sendUsername();
        void sendMessages();
        void receiveMessages();
    
    public:
        Client(const std::string& serverIp, int port);
        ~Client();
    
        void run();
};
