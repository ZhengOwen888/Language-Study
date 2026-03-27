/*
    Client Model

    1. Get Address Info
        - Use getaddrinfo() to get the possible address of a remote host.

    2. Create Socket
        - Use socket() to create a new socket endpoint for communication.

    3. Connect to remote host
        - Use connect() to connect the host socket to the remote host socket.

    4. Free Address Info
        - Use freeaddrinfo() to clean up memeory allocated by getaddrinfo()

    5. Communicate
        - Use recv() and send() for SOCK_STREAM, or recvfrom() and sendto()for SOCK_DGRAM.
        - You can also wrap socket descriptor in a FILE stream and use all the functions associated with FILE (e.g. fgets(), fprintf...)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>

#define PORT "8080"
#define DEFAULT_HOST "localhost"
#define MAX_BUFFER_SIZE 100000

#define handle_error(msg) \
    do { perror(msg); exit(-1);} while (0)

    int main(int argc, char *argv[])
    {
        /*======================
            Get Address Info
        ======================*/
        // Set the hostname
        char *hostname = DEFAULT_HOST;
        if (argc > 1)
        {
            hostname = argv[1];
        }

        // Prepare hints to get address info
        struct addrinfo hints = {0};
        hints.ai_flags = AI_ADDRCONFIG;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        // Get the server address info and handle potential error
        struct addrinfo *serv_addrinfo;
        int res;
        if ((res = getaddrinfo(hostname, PORT, &hints, &serv_addrinfo)) != 0)
        {
            handle_error(gai_strerror(res));
        }

        /*====================================================
            Create Socket for Connection and Communication
        =====================================================*/
        int comm_sockfd = socket(serv_addrinfo->ai_family, serv_addrinfo->ai_socktype, serv_addrinfo->ai_protocol);
        if (comm_sockfd == -1)
        {
            handle_error("Client: socket failed.");
        }

        /*============================
            Connect to Remote Host
        ============================*/
        res = connect(comm_sockfd,serv_addrinfo->ai_addr, serv_addrinfo->ai_addrlen);
        if (res == -1)
        {
            handle_error("Client: connect failed.");
        }

        /*=======================
            Free Address Info
        =======================*/
        freeaddrinfo(serv_addrinfo);


        /*==================================
            Communicate with Remote Host
        ==================================*/
        FILE *recv_fd = fdopen(comm_sockfd, "r");
        FILE *send_fd = fdopen(dup(comm_sockfd), "w");

        while (1)
        {
            char recv_buf[MAX_BUFFER_SIZE];
            char send_buf[MAX_BUFFER_SIZE];

            /*
                Prepare the data to be sent.
            */
            fprintf(stderr, "Enter Data to send to Server: ");

            // Get the data that needs to be sent from stdin (user input)
            if (fgets(send_buf, MAX_BUFFER_SIZE, stdin) == NULL)
            {
                // Done with input (Ctrl - D)
                if (feof(stdin))
                {
                    fprintf(stderr, "\nClient: Closing connection with Server.");
                    break;
                }
                else if (ferror(stdin))
                {
                    fprintf(stderr, "\nClient: read error.\n");
                    break;
                }
            }
            /*
                Send the ready data to remote host.
            */
            fprintf(send_fd, "%s", send_buf);
            fflush(send_fd); // Makes sure full message is sent to the remote host.

            /*
                Recive reponse or data from remote host.
            */
            if (fgets(recv_buf, MAX_BUFFER_SIZE, recv_fd) == NULL)
            {
                // Server closed its send (write) end
                if (feof(recv_fd))
                {
                    fprintf(stderr, "\nClient: Server Closed the Connection.\n");
                    break;
                }
                else if (ferror(recv_fd))
                {
                    fprintf(stderr, "\nClient: read error.\n");
                    break;
                }
            }

            /*
                Display response from remote host.
            */
            fprintf(stderr, "Server Response: %s\n", recv_buf);
        }

        /*
            You do not need to do close(comm_fd)

            fclose(recv_fd) already closes comm_fd for us, because we opened it with fdopen(comm_fd, "r").
            fclose(send_fd) just closes the file stream for writing to the same socket.
        */
        fclose(recv_fd);
        fclose(send_fd);

        exit(0);
    }