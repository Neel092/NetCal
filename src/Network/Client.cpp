#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void client()
{
    int sock = 0;
    struct sockaddr_in server_address;
    // char *message = "Hello, I am taking from client side";
    char buffer[1024] = {0};

    // 1. Create Socket
    sock = socket(AF_INET, SOCK_STREAM, 0); // sock_stream (Socket Type – Stream)

    // 2. Define server Address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    //(Host To Network Short) Converts port number to network byte order

    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);
    /*
        inet_pton(Internet Presentation To Network)
        AF_INET(Address Family – Internet) => IPv4
        "127.0.0.1" => human-readable IP (connect to same machine [localhost])
        server_address.sin_addr => where to store binary
    */

    // 3.connect
    printf("Connecting...\n");
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Server not running!\n");
        return;
    }

    printf("Connecting...\n");

    // 4. Take input
    while (1)
    {

        printf("Enter numeric Value for calulation :- \n");
        char msg[100] = {0};
        scanf("%s", msg);
        if (msg[0] == 'Q' || msg[0] == 'q')
        {

            send(sock, msg, strlen(msg), 0);
            printf("Exiting...\n");
            close(sock);
            break;
        }

        // 5. Send data
        send(sock, msg, strlen(msg), 0);

        // 6. Read response
        read(sock, buffer, 1024);
        printf("Server replied: %s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
    }

    close(sock);
}