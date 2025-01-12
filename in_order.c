
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    // User defined data may be declared here.
    pthread_mutex_t m2;
    pthread_mutex_t m3;
} Foo;

Foo* fooCreate() {
    Foo* obj = (Foo*) malloc(sizeof(Foo));
    pthread_mutex_init(&(obj->m2), NULL);
    pthread_mutex_init(&(obj->m3), NULL);
    pthread_mutex_lock(&(obj->m2));
    pthread_mutex_lock(&(obj->m3));
    // Initialize user defined data here.
    return obj;
}

void* first(void * args) {
    Foo * obj = (Foo*)args;

    printf("first");
    pthread_mutex_unlock(&(obj->m2));
    return NULL;
}

void* second(void * args) {
    Foo * obj = (Foo*)args;

    pthread_mutex_lock(&(obj->m2));
    printf("second");
    pthread_mutex_unlock(&(obj->m3));
    return NULL;
}

void* third(void * args) {
    Foo * obj = (Foo*)args;
    pthread_mutex_lock(&(obj->m3));
    printf("third");
    return NULL;

}

void fooFree(Foo* obj) {
    // User defined data may be cleaned up here.
    pthread_mutex_destroy(&(obj->m2));
    pthread_mutex_destroy(&(obj->m3));
    free(obj);
}

int main(){
    pthread_t t[3];
    // Leet Code Problem: https://leetcode.com/problems/print-in-order/description/
    Foo * foo = fooCreate();
    pthread_create(&t[1], NULL, &second, foo);
    pthread_create(&t[0], NULL, &third, foo);
    pthread_create(&t[2], NULL, &first, foo);
    pthread_join(t[0], NULL);
    pthread_join(t[1], NULL);
    pthread_join(t[2], NULL);

    fooFree(foo);

}