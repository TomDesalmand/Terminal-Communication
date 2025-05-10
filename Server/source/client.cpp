// Header Files Includes //
#include "client.hpp"

Client::Client(int socket, const std::string& username)
    : _socket(socket), _username(username) {}

int Client::getClientSocket() const {
    return _socket;
}

const std::string& Client::getUsername() const {
    return _username;
}

std::thread& Client::getClientThread() {
    return _thread;
}

void Client::setUsername(const std::string& username) {
    _username = username;
}