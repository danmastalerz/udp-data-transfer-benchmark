//
// Created by danielmastalerz on 02.12.22.
//

#ifndef UDP_FILE_TRANSFER_FILE_RECEIVER_H
#define UDP_FILE_TRANSFER_FILE_RECEIVER_H


#include <string>
#include <vector>
#include <netinet/in.h>

class file_receiver {
private:
    int socket_fd;
    std::string ip;
    int port;
    size_t bytes_to_send;
    struct sockaddr_in addr;
    std::vector<char> buffer;
public:
    file_receiver(std::string ip, int port, size_t bytes_to_send);
    void start_receiving();

};


#endif //UDP_FILE_TRANSFER_FILE_RECEIVER_H
