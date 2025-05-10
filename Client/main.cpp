#include "client.hpp"

int main() {
    Client client("127.0.0.1", 8080);
    client.run();
    return 0;
}
