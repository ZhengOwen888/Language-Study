/*
    Sever Model

    1. Get Address Info
        - Use getaddrinfo() to get possible local addresses to bind to.

    2. Create Socket
        - Use socket() to create a socket

    3. Bind
        - Use bind() to associate the socket with relevant local addresses and local port

    4. Free Address Info
        - Use freeaddrinfo() to clean up memory allocated by getaddrinfo()

    5. Listen for incoming connections
        - Use listen() to listen for incoming connections

    6. Accept Request
        - Use accept() to get new socket descriptor to communicate back to a specific client
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main()
{
    /*======================
        Get Address Info
    ======================*/

    /*====================
        Create Socket
    ====================*/

    /*=====================
        Bind The Socket
    =====================*/

    /*=======================
        Free Address Info
    =======================*/

    /*=====================================
        Listen for Incoming Connections
    =====================================*/

    /*======================
        Accept Requests
    ======================*/

    return 0;
}