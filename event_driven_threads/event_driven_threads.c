/*
In a system various events may occur 
some of these events may require taking actions that will take a long time to perfom 
In that case a thread should be spawned

In this example the user can input either:
A: a string beginning with a number 0-9
B: any other variety of string.
In case A - the string will be processed in a way that sums the numeric value of the char 
i.e '0' = 0, any non numeric value will be treated as 0
In case B - the string will be processed to capitalize all lowercase letters
*/
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>

#define EVENT_Q_SZ (10)
//----- structures which don't require sharing with other files -----
typedef enum {
    SUM_PROCESSING,
    CHAR_PROCESSING,
    START_EVENT_HANDLER,
}event_t;

typedef struct{
    event_t event;
    char* data;
} event_data_t;

typedef struct{
    event_data_t q[EVENT_Q_SZ];
    uint8_t size;
    uint8_t head_idx;
}event_q_t;

//------ prototypes ------
void * process_sum(void * d);
void * process_alpha(void * d);
void * event_handler(void * args);

//------ globals -------
event_q_t g_events;
void* (*thread_functions[])(void*) = {
        process_sum,
        process_alpha,
        event_handler
};
pthread_mutex_t g_std_inout;
//combine top and pop because we'll always want to pop
event_data_t q_pop(event_q_t * q){
    //if q is empty an empty node will be returned, so will check for that outside
    event_data_t h = q->q[q->head_idx];
    event_data_t empty = {0};
    if (q->size > 0) {
        q->q[q->head_idx] = empty;
        q->head_idx = q->head_idx + 1 % EVENT_Q_SZ;
        q->size--;
    }
    return h;
}

void q_push(event_q_t * q, event_data_t d){
    if (q->size < EVENT_Q_SZ){
        q->q[q->size % EVENT_Q_SZ] = d;
        q->size++;
    }
}

void * process_sum(void * d) {
    // Example processing: print the line in uppercase
    printf("%s", __func__);
    char* buf = (char*)(d);
    uint64_t sum = 0;
    for (int i = 0; buf[i] != '\0'; i++) {
        if (buf[i] >= '0' && buf[i] < '9'){
            sum += buf[i] - '0';
        }
    }
    pthread_mutex_lock(&g_std_inout);
    printf("sum: %u\n", sum);
    pthread_mutex_unlock(&g_std_inout);
    free(d);
}

void * process_alpha(void * d) {
    // Example processing: print the line in uppercase
    char* buf = (char*)(d);
    printf("Processing sum: ");
    uint64_t sum = 0;
    for (int i = 0; buf[i] != '\0'; i++) {
        if (buf[i] >= 'a' && buf[i] < 'z'){
            buf[i] = buf[i] + 'A';
        }
    }
    pthread_mutex_lock(&g_std_inout);
    printf("buf: %s\n", buf);
    pthread_mutex_lock(&g_std_inout);
    free(d);
}

void create_thread(event_data_t e) {
    pthread_t p;
        printf("%s", __func__);

    pthread_create(&p, NULL, (thread_functions[e.event]), e.data);
    //no need to join since program runs indefinitely
    pthread_detach(p);
}

void * event_handler(void* args){
    while(1){
        if (g_events.size > 0){
            event_data_t d = q_pop(&g_events);
            create_thread(d);
        }
        sleep(5);
    }
}

int main() {
    char input[256];
    pthread_mutex_init(&g_std_inout, NULL);
    event_data_t d = {0};
    d.event = START_EVENT_HANDLER;
    create_thread(d);
    printf("Enter 'exit' to quit the program.\n");

    while (1) {
        pthread_mutex_lock(&g_std_inout);
        printf("Input: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nError or EOF encountered. Exiting.\n");
            break;
        }
        // Remove the trailing newline character
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            printf("Exiting program.\n");
            break;
        }
        pthread_mutex_unlock(&g_std_inout);
        // Process the input line
        char * buf = (char*)malloc(strlen(input));
        strcpy(buf, input);
        d.data = buf;
        d.event = (input[0] >'0' && input[0] < '9')? SUM_PROCESSING: CHAR_PROCESSING;
        q_push(&g_events, d);
    }
    return 0;
}
