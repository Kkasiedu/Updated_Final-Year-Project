#ifndef SOCKETLISTENER_H
#define SOCKETLISTENER_H

#include <string>
#include <thread>
#include <functional>

class SocketListener {
public:
    // Constructor matching the arguments in GUI.cpp
    SocketListener(const std::string& address, int port, const std::string& saveDir);

    // Destructor
    ~SocketListener();

    // Start and stop methods
    void start();
    void stop();

    // Callback for when an image is received
    void setOnImageReceivedCallback(const std::function<void(const std::string&)>& callback);

private:
    std::string address;
    int port;
    std::string saveDirectory;
    std::thread listenerThread;
    bool isRunning;
    std::function<void(const std::string&)> onImageReceivedCallback;

    void listen();
};

#endif // SOCKETLISTENER_H
