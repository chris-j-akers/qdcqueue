#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "../qdcqueue.h"

// This function will read in a file of words separated by newline. The idea
// being that all these words are to be added to the queue. It returns a pointer
// to the buffer containing all those words.
//
// Here, we could read in the data a word at a time and add each one to the test
// queue as we go, but it's quicker to just gulp the lot into memory and sift
// through it there. This is, after all, just a test of the queue.

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

// Just a loop that scans through the buffer using strtok to get each word and
// then enqueue() it.

QDCQueue* populate_queue(char* buffer, QDCQueue* q) {
     
    long index = 0;
    char * word = strtok(buffer, "\n\0");
    
    while( word != NULL) {

        enqueue(q, word);
        word = strtok(NULL, "\n\0");

    }

    return q;
}

// Print out some stats about the queue. Called every so often in main() to
// check progress.

void print_queue_info(QDCQueue* q) {
    printf("capacity: %ld, grow_by: %ld, head: %ld, tail: %ld, length: %ld\n", q->capacity, q->grow_by, q->head, q->tail, q->tail-q->head);
}

// Run some tests on the queue

int main(int argc, char** argv) {

    printf("building new queue with capacity of 5 items\n");

    QDCQueue* q = create_queue(5, 10);
    print_queue_info(q);

    printf("reading datafile into buffer and adding items to queue\n");
    char* buffer = read_data_file(argv[1]);
    q = populate_queue(buffer, q);

    print_queue_info(q);

    printf("dequeueing 5 items\n");
    for (int i=0; i<5; i++) {
        printf("\titem: %s\n", (char*)dequeue(q));
    }

    print_queue_info(q);

    printf("Shrinking queue\n");

    q = shrink_queue(q);

    print_queue_info(q);

    printf("peeking next item in queue: %s\n", (char*)peek(q));

    printf("dequeing rest of items\n");
    char* item;
    while( (item = dequeue(q)) != NULL) {
        printf("\titem: %s\n", item);
    }
    printf("done!\n");
    print_queue_info(q);

    destroy_queue(q);
}