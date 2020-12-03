
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "../queue.h"
#include "cakelog/cakelog.h"

char* read_data_file(const char *dict_file) {

    const int dictionary_fd = open(dict_file, O_RDONLY);
    if (dictionary_fd == -1) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    struct stat dict_stats;

    if (fstat(dictionary_fd, &dict_stats) == -1) {
        perror("fstat()");
        exit(EXIT_FAILURE);
    }

    const long file_size = dict_stats.st_size;

    const long buffer_size = file_size + 1;
    char* buffer = malloc(buffer_size);
     
    ssize_t bytes_read;
    if((bytes_read = read(dictionary_fd, buffer, file_size)) != file_size) {
        perror("read()");
        exit(EXIT_FAILURE);
    }

    close(dictionary_fd);

    buffer[file_size] = '\0';

    return buffer;
    
}

Queue* build_queue(char* buffer, long capacity, long increment) {
 
    Queue* q = create_queue(capacity, increment);
    
    long index = 0;
    char * word = strtok(buffer, "\n\0");
    
    while( word != NULL) {

        enqueue(q, word);
        word = strtok(NULL, "\n\0");

    }

    return q;
}

int main(int argc, char** argv) {

    cakelog_initialise(argv[0], true);

    char* buffer = read_data_file(argv[1]);

    cakelog("start");

    Queue* q = build_queue(buffer, 500000, 1);

    cakelog("stop");

    printf("capacity: %ld, increments: %ld, head: %ld, tail: %ld\n", q->capacity, q->increments, q->head, q->tail);

    char* item;
    while( (item = (char*)dequeue(q)) != NULL) {
        // printf("%s\n", item);
    }

    printf("capacity: %ld, increments: %ld, head: %ld, tail: %ld\n", q->capacity, q->increments, q->head, q->tail);

    destroy_queue(q);

    cakelog_stop();

}