# IPK Project 1 – Calculator

        Author:  Maksim Kalutski (xkalut00)
        Date:    2023-03-21

This is a C++ program for sending and receiving data from a server using either the TCP or UDP protocol. The program is able to send data in the form of a mathematical expression and return the result of the expression.

## Written in

* [C++20 language](https://en.wikipedia.org/wiki/C%2B%2B)

## Prerequisites

Before using the program, you need to have **g++** and **make** installed on your **UNIX** operating system.

## Build
```console
$ make
```

## Usage
```console
$ ./ipkcpc -h <host> -p <port> -m <mode>
```

### Command-line arguments
| Argument | Description |
|----------|-------------|
| `-h` | server hostname or IP address |
| `-p` | server port number |
| `-m` | protocol mode (tcp or udp) |

## Usage examples
```console
$ ./ipkcpc -h 147.229.176.19 -p 10002 -m tcp
```

```console
$ ./ipkcpc -h localhost -p 2023 -m udp
```

```console
$ ./ipkcpc -h merlin.fit.vutbr.cz -p 10002 -m tcp
```


The program accepts two types of expressions - binary and textual. The binary expression is sent as a string, and the program returns the result of the expression. The textual expression is a sequence of commands that are sent to the server.

# Binary Variant

## Status Codes
| Opcode | Value |
|-------|------|
|Request |0 |
|Response |1 |

## Request Message Format
```  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +---------------+---------------+-------------------------------+
 |     Opcode    |Payload Length |          Payload Data         |
 |      (8)      |      (8)      |                               |
 +---------------+---------------+ - - - - - - - - - - - - - - - +
 :                     Payload Data continued ...                :
 + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 |                     Payload Data continued ...                |
 +---------------------------------------------------------------+
```

## Status Codes
| Status | Value |
|-------|------|
|OK	|0 |
|Error	|1 |


## Response Message Format
```  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +---------------+---------------+---------------+---------------+
 |     Opcode    |  Status Code  |Payload Length | Payload Data  |
 |      (8)      |      (8)      |      (8)      |               |
 +---------------+---------------+---------------+ - - - - - - - +
 :                     Payload Data continued ...                :
 + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 |                     Payload Data continued ...                |
 +---------------------------------------------------------------+
 ```

## Input/Output Examples
```
Input:
(+ 1 2)
(+ (+ (/ 5 5) (* 4 5)) (- 5 4))
(a b)

Output:
OK:3
OK:22
ERR:<error message>
```

# Textual Variant
## Message Format
- **hello** = "HELLO" LF
- **solve** = "SOLVE" SP query LF
- **result** = "RESULT" SP 1*DIGIT LF
- **bye** = "BYE" LF

## Input/Output Examples
```
Input:
HELLO
SOLVE (+ 1 2)
SOLVE (+ (+ (/ 5 5) (* 4 5)) (- 5 4))
BYE

Output:
HELLO
RESULT 3
RESULT 22
BYE
```

## Dependencies
The program uses the following libraries:

- `iostream`
- `stdio.h`
- `stdlib.h`
- `unistd.h`
- `string.h`
- `getopt.h`
- `csignal`

## Functions
### The program includes the following functions:

- `printUsage()`: prints the usage of the program to the console
- `signalHandler()`: catches the Ctrl-c signal and terminates the connection with the server
- `parseArguments()`: parses the command line arguments and sets the mode, host, and port variables
- `tcp()`: connects to the server using TCP protocol, sends messages, and receives responses
- `udp()`: connects to the server using UDP protocol, sends messages, and receives responses

## UML Diagram
```
                                  +------------------------+
                                  |                        |
                                  |         ipkcpp         |
                                  |                        |
                                  +------------------------+
                                  |                        |
                                  |   - parseArguments()   |
                                  |   - tcp()              |
                                  |   - udp()              |
                                  |   - signalHandler()    |
                                  |                        |
                                  +------------------------+
                                              |
                            +-----------------------------------+
                            |                                   |
                            |          NetworkManager           |
                            |                                   |
                            +-----------------------------------+
                            |                                   |
                            |   - connect()                     |
                            |   - disconnect()                  |
                            |   - sendMessage()                 |
                            |   - receiveMessage()              |
                            |                                   |
                            +-----------------------------------+
                        
```

## Exit codes
| Exit code | Number | Description |
|-----------|------|-------------|
| `EXIT_SUC` | 0 | successful execution |
| `EXIT_ARG` | 1 | incorrect usage of command-line arguments |
| `EXIT_PARCE` | 2 | error parsing command-line arguments |
| `EXIT_SIGNAL` | 3 | program terminated by user |
| `EXIT_TCP_NET` | 4 | error with TCP network |
| `EXIT_UDP_NET` | 5 | error with UDP network |


# Testing

The program has been tested on **NixOS 22.11**. The following tests were conducted:

Testing the program with correct arguments for **TCP** mode.
Testing the program with correct arguments for **UDP** mode.
To run the tests:

1. Download the source code and compile the program using the C++ compiler.
2. Open the terminal and navigate to the directory where the program is located.
3. Run the program with the correct arguments for TCP/TCP mode. The program should establish a connection with the server and display the response to the request on the console.

## Testing TCP
```
$ ./ipkcpc -h localhost -p 2023 -m tcp
INFO: Server socket: 127.0.0.1 : 2023
HELLO
HELLO
SOLVE (+ 1 5)
RESULT 6
SOLVE (- 8 9)
RESULT -1
SOLVE (* 8 7)
RESULT 56
SOLVE (/ 10 5)
RESULT 2
BYE
BYE
```

```
$ ./ipkcpc -h localhost -p 2023 -m tcp
INFO: Server socket: 127.0.0.1 : 2023
HELLO
HELLO
SOLVE (+ (+ (/ 5 5) (* 4 5)) (- 5 4))                
RESULT 22
BYE
BYE
```

```
$ ./ipkcpc -h localhost -p 2023 -m tcp
INFO: Server socket: 127.0.0.1 : 2023
AHOJ
ERROR: First message must be HELLO
```

```
$ ./ipkcpc -h localhost -p 2023 -m tcp
INFO: Server socket: 127.0.0.1 : 2023
HELLO
HELLO
^C
BYE
```

## Testing UDP
```
$ ./ipkcpc -h localhost -p 2023 -m udp
INFO: Server socket: 127.0.0.1 : 2023
(+ 1 5)
OK: 6
(- 8 9)      
OK: -1
(* 8 7)
OK: 56
(/ 10 5)
OK: 2
(a b) 
ERR: Could not parse the message
(+12)
ERR: Could not parse the message
()
ERR: Could not parse the message
HELLO
ERR: Could not parse the message
^C 
```

```
$ ./ipkcpc -h localhost -p 2023 -m udp
INFO: Server socket: 127.0.0.1 : 2023
(+ (+ (/ 5 5) (* 4 5)) (- 5 4))  
OK: 22
```

## Notes

If you are using **TCP** first you have to say **HELLO** to the server or it will not respond. Be polite... If you are using **UDP** you can send the mathematical expression right away.

:warning: **WARNING**: If you are using **TCP** you can kill the server by typing BYE or pressing `Ctrl+C` in this case you will recive **BYE** message. If you are using **UDP** you can only kill the server by pressing `Ctrl+C`. :warning:

:warning: **WARNING**: The program does not parse the mathematical expression. It is up to the user to ensure that the expression is valid. Viz `Input/Output Examples`. :warning:

:warning: **WARNING**: The program has not been tested on Windows machines. Probaly it will not work. :warning:
