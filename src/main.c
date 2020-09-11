#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080

int main() {
    char filename[] = "output/configuracion.config";
    printf("Path: %s\n", filename);
    FILE *input = fopen(filename, "r");
    if (input == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    } else {
        char ch;
        while ((ch = fgetc(input)) != EOF)
            printf("%c", ch);
        fclose(input);
    }

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                             (socklen_t *) &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Open file for writing
    char path[30];
    time_t t;
    srand((unsigned) time(&t));
    int8_t id = rand();
    snprintf(path, sizeof path, "%s%d%s", "output/image", id, ".png");
    FILE *output = fopen(path, "wb");

    // Write to file from socket
    char buffer[1024] = {0};
    printf("Start writing", NULL);
    ssize_t bytes_read;
    int i = 0;
    while((bytes_read = read(new_socket, buffer, 1024)) > 0) {
        printf("Still writing file %d\n Bytes Read: %d\n", i, bytes_read);
        fwrite(buffer, sizeof(buffer), 1, output);
        i++;
    }
    printf("Finished writing file %d\n Bytes Read: %d\n", i, bytes_read);
    fclose(output);
    printf("Server: File received\n");

    char response[] = "Server: File received\n";
    send(new_socket, response, strlen(response), 0);
    return 0;
}
