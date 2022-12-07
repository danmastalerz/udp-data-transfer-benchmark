//
// Created by danielmastalerz on 02.12.22.
//

#include <iostream>
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include "file_receiver.h"
#include "constants.h"

file_receiver::file_receiver(std::string ip, int port, size_t bytes_to_send) {
    this->ip = ip;
    this->port = port;
    this->bytes_to_send = bytes_to_send;
    // create socket and bind to port
    this->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->socket_fd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    //set socket as reusable
    int optval = 1;
    setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // bind socket to port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->port);
    addr.sin_addr.s_addr = inet_addr(this->ip.c_str());
    if (bind(this->socket_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        std::cerr << "Error binding socket to port" << std::endl;
        exit(1);
    }
    this->addr = addr;
    this->buffer = std::vector<char>(MAX_PACKET_SIZE);
}

void file_receiver::start_receiving() {
    // receive bytes_to_send bytes
    size_t bytes_received = 0;

    // Create an instance of a timer.
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    size_t bytes_received_in_last_second = 0;

    int msg_count = 10;


    while (true) {
        size_t bytes_to_receive_now = MAX_PACKET_SIZE;

        struct sockaddr_in sender_addr;
        socklen_t sender_addr_len = sizeof(sender_addr);
//        ssize_t bytes_received_now = recvfrom(this->socket_fd, this->buffer.data(), bytes_to_receive_now, 0,
//                                              (struct sockaddr *) &sender_addr, &sender_addr_len);

        // Use recvmmsg to receive many packets in one syscall.
        struct mmsghdr msgvec[msg_count];
        memset(msgvec, 0, sizeof(msgvec));

        struct iovec iovec[msg_count];
        memset(iovec, 0, sizeof(iovec));

        for (int i = 0; i < msg_count; i++) {
            msgvec[i].msg_hdr.msg_iov = &iovec[i];
            msgvec[i].msg_hdr.msg_iovlen = 1;

            iovec[i].iov_base = this->buffer.data();
            iovec[i].iov_len = bytes_to_receive_now;
        }

        ssize_t datagrams_received = recvmmsg(this->socket_fd, msgvec, msg_count, 0, NULL);

        // Calculate the total number of bytes received.
        ssize_t bytes_received_now = 0;
        for (int i = 0; i < datagrams_received; i++) {
            bytes_received_now += msgvec[i].msg_len;
        }

        if (bytes_received_now < 0) {
            std::cerr << "Error receiving data" << std::endl;
            exit(1);
        }
        bytes_received += bytes_received_now;
        bytes_received_in_last_second += bytes_received_now;

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() >= 1) {
            std::cout << "Speed of receiving: " << bytes_received_in_last_second / (1024.0 * 1024.0) << " MB/s"
                      << std::endl;
            bytes_received_in_last_second = 0;
            begin = std::chrono::steady_clock::now();
        }

    }
}
