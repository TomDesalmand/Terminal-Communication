#pragma once

// STD Includes //
#include <thread>

class Client {
    private:
        int _socket;
        std::thread _thread;
        std::string _username;
    public:
        int getClientSocket() const {return _socket;};
        const std::string& getUsername() { return _username; }
        std::thread& getClientThread() {return _thread;};
        
        void setClientSocket(int socket) {_socket = socket;};
        void setUsername(const std::string& username) { _username = username; }
};