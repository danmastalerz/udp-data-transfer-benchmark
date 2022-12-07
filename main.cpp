#include <iostream>
#include "file_sender.h"
#include "file_receiver.h"

int main(int argc, char *argv[]) {

    std::string ip = "127.0.0.1";
    int port = 5113;
    std::string file_path = "default_file.txt";
    std::string mode = "receive";
    size_t bytes_to_send = 1000*1000*1000;

    // -ip <ip>, -p <port>, -f <file_path>, -m <send|receive> -b <bytes_to_send>
    for (int i = 1; i < argc; i++)  {
        if (std::string(argv[i]) == "-m") {
            mode = argv[++i];
        }
    }

    if (mode == "send") {
        file_sender sender(ip, port, bytes_to_send);
        sender.start_sending();
    } else if (mode == "receive") {
        file_receiver receiver(ip, port, bytes_to_send);
        receiver.start_receiving();
    } else {
        std::cerr << "Invalid mode" << std::endl;
        exit(1);
    }

    return 0;
}
