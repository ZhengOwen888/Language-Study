/*
    Sever Model

    1. Get Address Info
        - Use getaddrinfo() to get possible local addresses to bind to.

    2. Create Socket
        - Use socket() to create a socket endpoint for listening

    3. Bind
        - Use bind() to associate the socket with relevant local addresses and local port

    4. Free Address Info
        - Use freeaddrinfo() to clean up memory allocated by getaddrinfo()

    5. Listen for incoming connections
        - Use listen() to listen for incoming connections

    6. Accept Request
        - Use accept() to get new socket descriptor to communicate back to a specific client

    7. Communicate
        - Use recv() and send() for SOCK_STREAM, or recvfrom() and sendto()for SOCK_DGRAM.
        - You can also wrap socket descriptor in a FILE stream and use all the functions associated with FILE (e.g. fgets(), fprintf...)

    8. Repeat 6 & 7
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>

#define PORT "8080" // The default port clients will be connecting to and server will be listening from
#define BACKLOG 10  // Max number of pending connections a queue can hold
#define MAX_BUFFER_SIZE 1024

#define handle_error(msg) \
    do {perror(msg); exit(-1);} while(0)

int main()
{
    /*======================
        Get Address Info
    ======================*/
    // Prepare hints to get address info
    struct addrinfo hints = {0};                    // Use {0} to zero out (empty or clear) addrinfo struct
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;    // AI_PASSIVE for binding, AI_ADDRCONFIG to get the right IPv.
    hints.ai_family = AF_UNSPEC;                    // Use IPv4 or IPv6.
    hints.ai_socktype = SOCK_STREAM;                // Use a stream socket, default protocol is TCP.

    // Get the server address info and handle potential error
    struct addrinfo *serv_addrinfo;
    int res;
    if ((res = getaddrinfo(NULL, PORT, &hints, &serv_addrinfo)) != 0)
    {
        handle_error(gai_strerror(res));
    }

    /*=================================
        Create Socket for Listening
    =================================*/
    int listen_sockfd = socket(serv_addrinfo->ai_family, serv_addrinfo->ai_socktype, serv_addrinfo->ai_protocol);
    if (listen_sockfd == -1)
    {
        handle_error("Server: socket failed.");
    }

    /*=====================
        Bind The Socket
    =====================*/
    int opt = 1;
    if (setsockopt(listen_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        handle_error("Server: setsockopt failed");
    }
    res = bind(listen_sockfd, serv_addrinfo->ai_addr, serv_addrinfo->ai_addrlen);
    if (res == -1)
    {
        handle_error("Server: bind failed.");
    }

    /*=======================
        Free Address Info
    =======================*/
    freeaddrinfo(serv_addrinfo);

    /*=====================================
        Listen for Incoming Connections
    =====================================*/
    res = listen(listen_sockfd, BACKLOG);
    if (res == -1)
    {
        handle_error("Server: listen failed.");
    }

    /*======================
        Accept Requests
    ======================*/
    struct sockaddr_in client_addr = {0};
    socklen_t addr_size;
    while (1)
    {
        int comm_sockfd = accept(listen_sockfd, NULL, NULL); // Blocking call
        if (comm_sockfd == -1)
        {
            handle_error("Server: accept failed.");
        }

        pid_t pid = fork();

        // Child Process - Takes Care of Communication with Client
        if (pid == 0)
        {
            /*=============================
                Communicate With Client
            =============================*/
            close(listen_sockfd);                          // Child does not need to listen for more connections
            FILE *recv_fd = fdopen(comm_sockfd, "r");      // Open for recieving messags from client
            FILE *send_fd = fdopen(dup(comm_sockfd), "w"); // Open for sending messages to client

            while (1)
            {
                char recv_buf[MAX_BUFFER_SIZE]; // Buffer for recieving data
                char send_buf[MAX_BUFFER_SIZE]; // Buffer for sending data

                /*
                    Wait for message from client.
                */
                fprintf(stderr, "Server Ready To Recieve Message from Client.\n");

                if (fgets(recv_buf, MAX_BUFFER_SIZE, recv_fd) == NULL)
                {
                    if (feof(recv_fd))
                    {
                        fprintf(stderr, "\nServer: Client closed the connection.\n");
                        break;
                    }
                    else if (ferror(recv_fd))
                    {
                        fprintf(stderr, "\nServer: read error.\n");
                        break;
                    }
                }

                /*
                    Clean up the recieved message.
                */
                // Remove trailing newline if present
                size_t len = strlen(recv_buf);
                if (len > 0 && recv_buf[len-1] == '\n') {
                    recv_buf[len-1] = '\0';
                }

                fprintf(stderr, "Message From Client: %s\n\n", recv_buf);

                /*
                    Prepare and send response.
                */
                snprintf(send_buf, MAX_BUFFER_SIZE, "Your message \"%s\" was recieved.\n", recv_buf);

                fprintf(send_fd, "%s", send_buf);
                fflush(send_fd); // Makes sure the full message is written to the client
            }
            fclose(recv_fd);
            fclose(send_fd);

            exit(0);
        }
        // Parent Process - Takes Care of Listening for more Requests or Connections from Clients
        else
        {
            close(comm_sockfd); // Parent does not need to communicate to clients
            int status;
            pid_t deadChild;
            do
            {
                deadChild = waitpid(-1, &status, WNOHANG);
                if (deadChild == -1)
                {
                    handle_error("Server: waitpid failed");
                }
                if (deadChild > 0)
                {
                    fprintf(stderr, "Reaped Child with PID: %d\n", deadChild);
                }
            } while (deadChild > 0);
        }
    }
    exit(0);
}