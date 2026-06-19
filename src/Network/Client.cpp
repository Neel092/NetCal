#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <asio.hpp>
#include <iostream>
using namespace std;

void client()
{
    // int sock = 0;
    // struct sockaddr_in server_address;
    // // char *message = "Hello, I am taking from client side";
    // char buffer[1024] = {0};

    // // 1. Create Socket
    // sock = socket(AF_INET, SOCK_STREAM, 0); // sock_stream (Socket Type – Stream)

    // // 2. Define server Address
    // server_address.sin_family = AF_INET;
    // server_address.sin_port = htons(8080);
    // //(Host To Network Short) Converts port number to network byte order

    // inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);
    // /*
    //     inet_pton(Internet Presentation To Network)
    //     AF_INET(Address Family – Internet) => IPv4
    //     "127.0.0.1" => human-readable IP (connect to same machine [localhost])
    //     server_address.sin_addr => where to store binary
    // */

    // // 3.connect
    // printf("Connecting...\n");
    // if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    // {
    //     printf("Server not running!\n");
    //     return;
    // }

    // printf("Connecting...\n");

    asio::io_context io;

    asio::ip::tcp::socket socket(io);

    asio::ip::tcp::endpoint endpoint(

        asio::ip::make_address("127.0.0.1"),

        8080

    );

    socket.connect(endpoint);

    cout << "Connected to server\n";

    while (true)
    {
        printf("Enter numeric value for calculation :- \n");
        char msg[100] = {0};
        fgets(msg, sizeof(msg), stdin);
        std::size_t msg_len = strlen(msg);

        if (msg[0] == 'Q' || msg[0] == 'q')
        {
            printf("Exiting...\n");
            socket.close();
            break;
        }
        asio::write(socket, asio::buffer(msg, msg_len));

        char data[1024] = {0};
        asio::error_code err;
        std::size_t len = socket.read_some(asio::buffer(data), err);
        if (err)
        {
            cout << "Connection closed or error: " << err.message() << endl;
            break;
        }

        // cout << "Server Reply : ";
        cout.write(data, len);
        cout << endl;
    }
}