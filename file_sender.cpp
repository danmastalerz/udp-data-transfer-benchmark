//
// Created by danielmastalerz on 02.12.22.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include "file_sender.h"
#include "constants.h"


file_sender::file_sender(std::string ip, int port, size_t bytes_to_send) {
    this->ip = ip;
    this->port = port;
    this->bytes_to_send = bytes_to_send;
    // create socket and bind to port
    this->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->socket_fd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }
    // bind socket to port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->port);
    addr.sin_addr.s_addr = inet_addr(this->ip.c_str());

    this->addr = addr;
    this->buffer = std::vector<char>(MAX_PACKET_SIZE);
}

void file_sender::start_sending() {
    // send bytes_to_send bytes
    size_t bytes_sent = 0;

    int msg_count = 10;

    struct mmsghdr msgvec[msg_count];
    memset(msgvec, 0, sizeof(msgvec));

    struct iovec iovec[msg_count];
    memset(iovec, 0, sizeof(iovec));

    for (int i = 0; i < msg_count; i++) {
        iovec[i].iov_base = this->buffer.data();
        iovec[i].iov_len = MAX_PACKET_SIZE;
        msgvec[i].msg_hdr.msg_iov = &iovec[i];
        msgvec[i].msg_hdr.msg_iovlen = 1;
        msgvec[i].msg_hdr.msg_name = &this->addr;
        msgvec[i].msg_hdr.msg_namelen = sizeof(this->addr);
    }

    while (true) {
        size_t bytes_to_send_now = MAX_PACKET_SIZE;
//        ssize_t bytes_sent_now = sendto(this->socket_fd, this->buffer.data(), bytes_to_send_now, 0,
//                                    (struct sockaddr *) &this->addr, sizeof(this->addr));

        // send using sendmmsg
        int bytes_sent_now = sendmmsg(this->socket_fd, msgvec, msg_count, 0);


        if (bytes_sent_now < 0) {
            std::cerr << "Error sending data" << std::endl;
            exit(1);
        }
        bytes_sent += bytes_sent_now;
    }
}
