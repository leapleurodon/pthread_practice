#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//For simplicity AR_SIZE must be divisable by THREADS
#define AR_SIZE (1000)
#define THREADS (100)

int ar[AR_SIZE];
void* foo (void* args) {
    int idx = *(int*)(args);
    int sum = 0;
    for (int i = 0; i < AR_SIZE/THREADS; i++) {
        sum += ar[idx++];
    }
    *(int*)args = sum;
    return args;
}

int main(){
    pthread_t threads[THREADS];
    int sum = 0;
    for(int i = 0; i < AR_SIZE; i++){
        ar[i] = i + 1;
    }
    if (AR_SIZE % THREADS != 0) {
        perror("bad array size and thread number provided!");
        assert(AR_SIZE % THREADS == 0);
    }
    for (int i = 0; i < THREADS; i++) {
        //argument to the thread must be malloced
        // this is because if we just used an int idx int the stack of the for loop
        // it would get overwritten each iteration, and completely freed by the end of the loop
        int *idx = (int*)malloc(sizeof(int));
        *idx = i * (AR_SIZE/THREADS);
        pthread_create(&threads[i], NULL, &foo, idx);
    }
    for (int i = 0; i < THREADS; i++) {
        int *r;
        pthread_join(threads[i], (void**)&r);
        sum += *r;
        free(r);
    }
    //verify that the sum is the sum of the series 1 to AR_SIZE
    int expected = (float)((1+AR_SIZE)/2.0) * AR_SIZE;
    assert(sum == expected);
    printf("end: %d\n", sum);
    return 0;
}