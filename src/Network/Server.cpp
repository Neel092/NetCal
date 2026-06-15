#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "Network/Server.hpp"
#include "Core/Parser.hpp"
#include "../include/Network/tpool.hpp"

void Process(void *arg)
{
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[1024];
    char response[100];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        //  Read data
        int bytes = read(client_fd, buffer, sizeof(buffer) - 1);

        if (bytes <= 0)
        {
            printf("Client disconnected abruptly.\n");
            break;
        }

        buffer[bytes] = '\0';

        //  Exit condition
        if (buffer[0] == 'Q' || buffer[0] == 'q')
        {
            printf("Client exited gracefully.\n");
            break;
        }

        // Process data
        int ans = Client_Server(buffer);

        // Send response
        sprintf(response, "%d", ans);
        send(client_fd, response, strlen(response), 0);
    }

    // exit client
    close(client_fd);
    printf("Connection closed.\n");
}
void server()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char response[100] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;         // AF_INET tells which Ipv4 is in use
    address.sin_addr.s_addr = INADDR_ANY; // beacuse of the all system will connect like localHost,wiFi IP and Ethernet IP , INADDR_ANY(Internet Address – Any)
    address.sin_port = htons(8080);       // mark to port 8080

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 10);

    printf("Server running...\n");

    threadpool_t *pool = threadpool_create(4);

    while (1)
    {
        // Accepting client
        int *client_fd = new int; // dynamic storage is use for every new client

        *client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

        if (*client_fd < 0)
        {
            printf("Error accepting client connection.\n");
            free(client_fd);
            delete client_fd;
            continue;
        }

        printf("New Client connected...\n");

        // initialization new thread for new client (POSIX Threads) => posix(Portable Operating System Interface)
        // pthread_t thread_id;

        // create thread
        threadpool_submit(pool, Process, client_fd);
        /*
            thread_id => identify the client
            Process => which fun to run
            Client_fd => data to process
        */
    }
    threadpool_destroy(pool);
    // threadpool_destory
    close(server_fd);
}
