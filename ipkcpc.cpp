#include <iostream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <getopt.h>
#include <netdb.h>

using namespace std;

void printUsage() {
    cout << "Usage: ipkcpc -h <host> -p <port> -m <mode>\n";
    cout << "  -h <host> - server hostname or IP address\n";
    cout << "  -p <port> - server port number\n";
    cout << "  -m <mode> - protocol mode (tcp or udp)\n";
}

void parseArguments(int argc, char** argv, string& mode, string& host, int& port) {
    int opt;
    port = -1;
    host = mode = "";

    while ((opt = getopt(argc, argv, "h:p:m:")) != -1) {
        switch (opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'm':
                if (strcmp(optarg, "tcp") == 0) {
                    mode = "tcp";
                } else if (strcmp(optarg, "udp") == 0) {
                    mode = "udp";
                } else {
                    cerr << "Invalid mode. Use tcp or udp." << endl;
                    printUsage();
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                printUsage();
                exit(EXIT_FAILURE);
        }
    }

    if (host == "" || port < 0 || port > 65535 || mode == "") {
        printUsage();
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {
    int port;
    string host, mode;

    parseArguments(argc, argv, mode, host, port);
}
