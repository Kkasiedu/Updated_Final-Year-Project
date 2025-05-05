#include "SocketListener.h"
#include <iostream>

SocketListener::SocketListener(const std::string& address, int port, const std::string& saveDir)
    : address(address), port(port), saveDirectory(saveDir), isRunning(false) {
}

SocketListener::~SocketListener() {
    stop();
}

void SocketListener::start() {
    if (isRunning) return;

    isRunning = true;
    listenerThread = std::thread(&SocketListener::listen, this);
}

void SocketListener::stop() {
    if (!isRunning) return;

    isRunning = false;
    if (listenerThread.joinable()) {
        listenerThread.join();
    }
}

void SocketListener::setOnImageReceivedCallback(const std::function<void(const std::string&)>& callback) {
    onImageReceivedCallback = callback;
}

void SocketListener::listen() {
    // Simulate listening for incoming connections
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Listening on " << address << ":" << port << " and saving to " << saveDirectory << std::endl;

        // Simulate receiving an image
        if (onImageReceivedCallback) {
            onImageReceivedCallback(saveDirectory + "/received_image.jpg");
        }
    }
}
