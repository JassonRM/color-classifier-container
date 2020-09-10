#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    printf("Hello world!\n");
    char filename[] = "output/configuracion.config";
    printf("Path: %s\n", filename);
    FILE *input = fopen(filename, "r");
    if(input == NULL){
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }else{
        char ch;
        while((ch = fgetc(input)) != EOF)
            printf("%c", ch);
        fclose(input);
    }
    char path[30];
    time_t t;
    srand((unsigned) time(&t));
    int id = rand();
    snprintf(path, sizeof path, "%s%d%s", "output/example", id, ".txt");
    FILE *output = fopen(path, "w");
    fprintf(output, "Testing");
    fclose(output);
    return 0;
}
