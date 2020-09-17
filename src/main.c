#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <png.h>
#include "image.h"
#include <sys/stat.h>

#define PORT 8080

int main() {
    // Create required directories
    int id = 1;
    char output_dir[100];
    snprintf(output_dir, sizeof output_dir, "%s%d", "output/", id);
    mkdir(output_dir, 0777);
    snprintf(output_dir, sizeof output_dir, "%s%d%s", "output/", id, "/R");
    mkdir(output_dir, 0777);
    snprintf(output_dir, sizeof output_dir, "%s%d%s", "output/", id, "/G");
    mkdir(output_dir, 0777);
    snprintf(output_dir, sizeof output_dir, "%s%d%s", "output/", id, "/B");
    mkdir(output_dir, 0777);
    snprintf(output_dir, sizeof output_dir, "%s%d%s", "output/", id, "/Not trusted");
    mkdir(output_dir, 0777);

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

    int server_fd, new_socket;
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

    // Saved received file in memory
    char *mem_buffer;
    size_t file_size;
    FILE *image = open_memstream(&mem_buffer, &file_size);

    // Read from socket
    char socket_buffer[1024];
    ssize_t bytes_read, totalBytes;
    read(new_socket, filename, 30);
    while((bytes_read = read(new_socket, socket_buffer, 1024)) > 0) {
        totalBytes += bytes_read;
        fwrite(socket_buffer, bytes_read, 1, image);
    }
    printf("Total bytes read: %ld bytes\n", totalBytes);

    // Classify file
    read_png_file(image);
    int result = process_png_file();
    printf("Result: %d\n", result);
    // Open file for writing
    char path[100];
    char *folder;
    switch (result) {
        case 0:
            folder = "R";
            break;
        case 1:
            folder = "G";
            break;
        case 2:
            folder = "B";
            break;
        default: folder = "Not trusted";
    }
    snprintf(path, sizeof path, "%s%d%s%s%s%s", "output/", id, "/", folder, "/", filename);
    printf("Path: %s\n", path);
    FILE *output = fopen(path, "wb");
    fwrite(mem_buffer, file_size, 1, output);
    fclose(output);
    return 0;
}

