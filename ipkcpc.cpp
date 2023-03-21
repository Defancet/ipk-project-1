/**
 * IPK Project 1 - Calculator
 * @author
 *   xkalut00, Maksim Kalutski
 *
 * @file ipkcpc.cpp
 * @brief TCP/UDP client for calculator server
 * @date 21.03.2023
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <csignal>

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

#define MAX_BUFFER_SIZE 1024

#define EXIT_SUC 0
#define EXIT_ARG 1
#define EXIT_PARCE 2
#define EXIT_SIGNAL 3
#define EXIT_TCP_NET 4
#define EXIT_UDP_NET 5

int client_socket;

/**
 * Prints usage of the program
 */
void printUsage() {
    cout << "Usage: ipkcpc -h <host> -p <port> -m <mode>\n";
    cout << "  -h <host> - server hostname or IP address\n";
    cout << "  -p <port> - server port number\n";
    cout << "  -m <mode> - protocol mode (tcp or udp)\n";
    exit(EXIT_ARG);
}

/**
 * Function to catch Ctrl-c signal
 */
void signalHandler(int) {
    cout << endl << "BYE" << endl;
    send(client_socket, "BYE\n", strlen("BYE\n"), 0);
    close(client_socket);
    exit(EXIT_SIGNAL);
}

/**
 * Parses arguments from command line
 * @param argc number of arguments
 * @param argv array of arguments
 * @param mode protocol mode
 * @param host server hostname or IP address
 * @param port server port number
 */
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
                    cerr << "ERROR: Option -" << (char) optopt << " requires an argument" << endl;
                    printUsage();
                    break;
                }
                else {
                    cerr << "ERROR: Unknown option -" << (char) optopt << endl;
                    printUsage();
                    break;
                }
            default:
                printUsage();
        }
    }

    if ((*host).empty() || !port_set || (*mode).empty()) {
        cerr << "ERROR: Missing required argument" << endl;
        printUsage();
    }
}

/**
 * TCP client function to connect to server send messages and receive responses
 * @param host server hostname or IP address
 * @param port server port number
 * @param client_socket socket descriptor
 * @param buffer buffer for sending and receiving messages
 */
void tcp(const string& host, const uint16_t& port, int& client_socket, char* buffer) {
    struct hostent *server = gethostbyname(host.c_str());
    if (server == nullptr) {
        cerr << "ERROR: No such host" << endl;
        exit(EXIT_TCP_NET);
    }

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "ERROR: Could not create socket" << endl;
        exit(EXIT_TCP_NET);
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
        cerr << "ERROR: Could not connect to server" << endl;
        exit(EXIT_TCP_NET);
    }

    signal(SIGINT, signalHandler);

    bool hello = false;

    while (true) {
        memset(buffer, 0, MAX_BUFFER_SIZE);

        if (fgets(buffer, MAX_BUFFER_SIZE, stdin) != NULL) {
        }

        if (!hello && strcmp(buffer, "HELLO\n") != 0) {
            cerr << "ERROR: First message must be HELLO" << endl;
            send(client_socket, "BYE\n", strlen("BYE\n"), 0);
            close(client_socket);
            exit(EXIT_PARCE);
        } else {
            hello = true;
        }

        int bytestx = send(client_socket, buffer, strlen(buffer), 0);
        if (bytestx < 0) {
            cerr << "ERROR: Could not send data to server" << endl;
            exit(EXIT_TCP_NET);
        }

        memset(buffer, 0, MAX_BUFFER_SIZE);

        int bytesrx = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytesrx < 0) {
            cerr << "ERROR: Could not receive data from server" << endl;
            exit(EXIT_TCP_NET);
        }

        cout << buffer;

        if (strcmp(buffer, "BYE\n") == 0) {
            break;
        }
    }
    close(client_socket);
    exit(EXIT_SUC);
}

/**
 * UDP client function to connect to server send messages and receive responses
 * @param host server hostname or IP address
 * @param port server port number
 * @param client_socket socket descriptor
 * @param buffer buffer for sending and receiving messages
 */
void udp(const string& host, const uint16_t& port, int& client_socket, char* buffer) {
    struct hostent *server = gethostbyname(host.c_str());
    if (server == nullptr) {
        cerr << "ERROR: No such host" << endl;
        exit(EXIT_UDP_NET);
    }

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "ERROR: Could not create socket" << endl;
        exit(EXIT_UDP_NET);
    }

    socklen_t serverlen;
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
    server_address.sin_port = htons(port);

    serverlen = sizeof(server_address);
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_address.sin_addr), ip_str, INET_ADDRSTRLEN);
    cout << "INFO: Server socket: " << ip_str << " : " << ntohs(server_address.sin_port) << endl;

    while (true) {
        string expression;
        getline(cin, expression);

        const size_t headerSize = 2;
        string send_DGram(headerSize + expression.length(), '\0');
        send_DGram[0] = '\0';
        send_DGram[1] = static_cast<char>(expression.length());
        copy(expression.begin(), expression.end(), send_DGram.begin() + headerSize);

        int bytestx = sendto(client_socket, send_DGram.c_str(), send_DGram.length(), 0, (struct sockaddr *) &server_address, serverlen);
        if (bytestx < 0) {
            cerr << "ERROR: Could not send data to server" << endl;
            exit(EXIT_UDP_NET);
        }

        memset(buffer, 0, MAX_BUFFER_SIZE);

        int bytesrx = recvfrom(client_socket, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *) &server_address, &serverlen);
        if (bytesrx < 0) {
            cerr << "ERROR: Could not receive data from server" << endl;
            exit(EXIT_UDP_NET);;
        }

        int status = buffer[1];
        string result = &buffer[3];
        if (status == 0) {
            cout << "OK: " << result << endl;
        } else {
            cerr << "ERR: " << result << endl;
        }
    }
}

int main(int argc, char** argv) {
    uint16_t port;
    string host, mode;

    char buffer[MAX_BUFFER_SIZE];

    parseArguments(argc, argv, &mode, &host, &port);

    if (mode == "tcp") {
        tcp(host, port, client_socket, buffer);
    } else if (mode == "udp") {
        udp(host, port, client_socket, buffer);
    }

    close(client_socket);
    return EXIT_SUC;
}
