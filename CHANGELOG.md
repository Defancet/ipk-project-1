# Changelog

## Version 0.1

     The printUsage function has been added.

## Version 0.2

    Implemented argument parsing with support for the following options:
        -h <host>
        -p <port> 
        -m <mode> 

    Added an error message in case no arguments were passed.

    Implemented parsing for only two modes: tcp and udp.


## Version 0.3

    Added support for the tcp mode

    Implemented a requirement to send a HELLO message to start communication with the server.

    Implemented a feature where sending a BYE message would result in the server sending a BYE message back and the client being disconnected.

    Added a function to catch Ctrl-c signal

    Added a message BYE to notify the user when they press Ctrl+C.

#### FIXES:

    Fixed a bug related to residual numbers in the buffer.

## Version 0.4

    Added support for the udp mode.

#### FIXES:

    Minor bug fixes.

## Version 1.0

    This is the release version.

    Added comments to each function.

#### FIXES:

    Minor bug fixes and quality of life improvements have been made.