#pragma once

// STD Includes //
#include <thread>
#include <string>

class Client {
    private:
        int _socket;
        std::string _username;
        std::thread _thread;
    
    public:
        Client(int socket, const std::string& username = "");
    
        int getClientSocket() const;
        const std::string& getUsername() const;
        std::thread& getClientThread();
    
        void setUsername(const std::string& username);
};