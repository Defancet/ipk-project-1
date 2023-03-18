#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

using namespace std;

#define MAX_BUFFER_SIZE 1024

#define EXIT_ARG 1
#define EXIT_NET 2
#define EXIT_SYS 3
#define EXIT_COM 4

void printUsage() {
    cout << "Usage: ipkcpc -h <host> -p <port> -m <mode>\n";
    cout << "  -h <host> - server hostname or IP address\n";
    cout << "  -p <port> - server port number\n";
    cout << "  -m <mode> - protocol mode (tcp or udp)\n";
    exit(EXIT_ARG);
}

void parseArguments(int argc, char** argv, string* mode, string* host, uint16_t* port) {
    if (argc == 1) {
        printUsage();
    }
    int opt;
    bool port_set = false;
    while ((opt = getopt(argc, argv, "h:p:m:")) != -1) {
        switch (opt) {
            case 'h':
                *host = optarg;
                break;
            case 'p': {
                char *endptr;
                long port_raw = strtol(optarg, &endptr, 10);
                if (*endptr != '\0' || port_raw < 0 || port_raw > 65535) {
                    cerr << "ERROR: -p <port> must be a number between 0 and 65535" << endl;
                    printUsage();
                }
                *port = (uint16_t) port_raw;
                port_set = true;
            } break;
            case 'm':
                if (strcmp(optarg, "tcp") == 0) {
                    *mode = "tcp";
                } else if (strcmp(optarg, "udp") == 0) {
                    *mode = "udp";
                } else {
                    cerr << "ERROR: -m <mode> must be either tcp or udp" << endl;
                    printUsage();
                }
                break;
            case '?':
                if (optopt == 'h' || optopt == 'p' || optopt == 'm') {
                    cerr << "ERROR: option -" << (char) optopt << " requires an argument" << endl;
                }
                else {
                    cerr << "ERROR: unknown option -" << (char) optopt << endl;
                }
            default:
                printUsage();
        }
    }

    if ((*host).empty() || !port_set || (*mode).empty()) {
        cerr << "ERROR: missing required argument" << endl;
        printUsage();
    }
}

void tcp(const string& host, const uint16_t& port, int& client_socket, char* buffer) {
    int bytestx, bytesrx = 0;

    struct hostent *server = gethostbyname(host.c_str());
    if (server == nullptr) {
        cerr << "ERROR: no such host" << endl;
        exit(EXIT_NET);
    }

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Error: could not create socket" << endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset((char *) &server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    memcpy((char *) &server_address.sin_addr.s_addr, server->h_addr, (size_t) server->h_length);
    server_address.sin_port = htons(port);

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_address.sin_addr), ip_str, INET_ADDRSTRLEN);
    cout << "INFO: Server socket: " << ip_str << " : " << ntohs(server_address.sin_port) << endl;

    if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    {
        cerr << "Error: could not connect to server" << endl;
        exit(EXIT_NET);
    }

    bool hello = false;

    do {
        bzero(buffer, MAX_BUFFER_SIZE);
        cin.getline(buffer, MAX_BUFFER_SIZE);

        if (!hello && strcmp(buffer, "HELLO") != 0) {
            cerr << "ERROR: first message must be HELLO" << endl;
            exit(EXIT_FAILURE);
        } else {
            hello = true;
        }


        bytestx = send(client_socket, buffer, strlen(buffer), 0);
        if (bytestx < 0) {
            cerr << "Error: could not send data to server" << endl;
            exit(EXIT_NET);
        }

        bzero(buffer, MAX_BUFFER_SIZE);

        bytesrx = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytesrx < 0) {
            cerr << "Error: could not receive data from server" << endl;
            exit(EXIT_NET);
        }

        cout << buffer;

    } while (strcmp(buffer, "BYE\n") != 0);
}

void udp(const string& host, const uint16_t& port, int& client_socket, char* buffer) {
    //TODO
}

int main(int argc, char** argv) {
    uint16_t port;
    string host, mode;

    int client_socket;
    char buffer[MAX_BUFFER_SIZE];

    parseArguments(argc, argv, &mode, &host, &port);

    if (mode == "tcp") {
        tcp(host, port, client_socket, buffer);
    } else if (mode == "udp") {
        udp(host, port, client_socket, buffer);
    }

    close(client_socket);
    return EXIT_SUCCESS;
}
