#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "image.h"
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 8080

void read_config_file(char trusted[][15], char accepted[][15]){
    char filename[] = "output/configuracion.config";
    printf("Path: %s\n", filename);
    FILE *input = fopen(filename, "r");

    if (input == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    } else {
        char *line;
        size_t len = 0;

        getline(&line, &len, input);
        if (strcmp(line, "trusted=\n") == 0) {
            int i = 0;
            while (1) {
                getline(&line, &len, input);
                if (strcmp(line, "accepted=\n") == 0) {
                    break;
                } else {
                    line[strlen(line) - 1] = '\0';
                    strcpy(trusted[i], line);
                    printf("Adding trusted: %s\n", trusted[i]);
                    i++;
                }
            }
            i = 0;
            while (getline(&line, &len, input) != -1) {
                line[strlen(line) - 1] = '\0';
                printf("Adding accepted: %s\n", line);
                strcpy(accepted[i], line);
                i++;
            }
        } else {
            printf("Wrong format\n");
            printf("%s\n", line);
            printf("trusted=");
        }

        fclose(input);
    }
}

void read_socket_into_file(int socket, FILE *file){
    // Read from socket
    char socket_buffer[1024];
    ssize_t bytes_read, totalBytes = 0;
    while ((bytes_read = read(socket, socket_buffer, 1024)) > 0) {
        totalBytes += bytes_read;
        fwrite(socket_buffer, bytes_read, 1, file);
    }
    printf("Total bytes read: %ld bytes\n", totalBytes);
}

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

    // Read config file
    char trusted[20][15];
    char accepted[20][15];
    read_config_file(trusted, accepted);

    // Create socket
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

    char *ip_address = inet_ntoa(address.sin_addr);
    printf("IP address is: %s\n", ip_address);
    bool ip_found = false;

    for(int i = 0; trusted[i][0] != '\0'; i++){
        if(strcmp(trusted[i], ip_address) == 0){
            // Saved received file in memory
            char *mem_buffer;
            size_t file_size;
            FILE *image = open_memstream(&mem_buffer, &file_size);
            char filename[30];
            read(new_socket, filename, 30);
            read_socket_into_file(new_socket, image);
            printf("Filename: %s\n", filename);

            // Classify file
            read_png_file(image);
            int result = process_png_file();
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
                default:
                    printf("Error");
                    return -1;
            }
            printf("Result: %s\n", folder);
            snprintf(path, sizeof path, "%s%d%s%s%s%s", "output/", id, "/", folder, "/", filename);
            printf("Path: %s\n", path);
            FILE *output = fopen(path, "wb");
            fwrite(mem_buffer, file_size, 1, output);
            fclose(output);
            ip_found = true;
            break;
        }
    }
    if(!ip_found) {
        for (int i = 0; accepted[i][0] != '\0'; i++) {
            if (strcmp(accepted[i], ip_address) == 0) {
                char path[100];
                char filename[30];
                read(new_socket, filename, 30);
                snprintf(path, sizeof path, "%s%d%s%s", "output/", id, "/Not trusted/", filename);
                printf("Path: %s\n", path);
                FILE *output = fopen(path, "wb");
                read_socket_into_file(new_socket, output);
                fclose(output);
                break;
            }
        }
    }
    close(new_socket);
    printf("Connection closed\n");
    return 0;
}