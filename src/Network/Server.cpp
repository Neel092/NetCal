#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <asio.hpp>
using namespace std;

#include "Network/Server.hpp"
#include "Core/Parser.hpp"
#include "../include/Network/tpool.hpp"

void Process(void *arg)
{
    int client_fd = *(int *)arg;
    delete (int *)arg;

    asio::io_context io;
    asio::ip::tcp::socket socket(io);
    socket.assign(asio::ip::tcp::v4(), client_fd);

    char buffer[1025];
    char response[100];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        //  Read data
        asio::error_code error;
        size_t bytes =

            socket.read_some(

                asio::buffer(buffer, 1024),
                error

            );

        if (error)
        {
            if (error == asio::error::eof)
            {
                printf("Client disconnected.\n");
            }
            else
            {
                printf("Error reading from client: %s\n", error.message().c_str());
            }
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

        snprintf(response, sizeof(response), "Result: %d", ans);

        asio::error_code write_error;
        asio::write(socket, asio::buffer(response, strlen(response)), write_error);
        if (write_error)
        {
            printf("Write error: %s\n", write_error.message().c_str());
            break;
        }
    }

    // exit client
    asio::error_code ec;
    socket.close(ec);
    printf("Connection closed.\n");
}

void server()
{
    // Event Manager
    asio::io_context io;

    // Create Endpoint
    asio::ip::tcp::endpoint endpoint(
        asio::ip::tcp::v4(),
        8080);

    // Creating acceptor
    asio::ip::tcp::acceptor acceptor(
        io,
        endpoint);

    cout << "Server Started" << endl;

    printf("Server running...\n");

    threadpool_t pool(4);

    while (1)
    {
        // Accepting client
        asio::ip::tcp::socket socket(io);

        cout << "Waiting for Client" << endl;

        acceptor.accept(socket);
        int *client_fd = new int(socket.native_handle());
        if (*client_fd < 0)
        {
            printf("Error accepting client connection.\n");
            delete client_fd;
            continue;
        }

        printf("New Client connected...\n");

        // create thread
        pool.submit(Process, client_fd);
        /*
            thread_id => identify the client
            Process => which fun to run
            Client_fd => data to process
        */

        socket.release();
    }
    // threadpool_destory
}
