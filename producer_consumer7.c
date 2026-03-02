#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty, full, mutex;
sem_t writerLock;  // writer preference

int waitingWriters = 0;

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        waitingWriters++;
        sem_wait(&writerLock);   // writer gets priority

        sem_wait(&empty);
        sem_wait(&mutex);

        buffer[in] = i;
        printf("Producer %d produced %d at %d\n", id, i, in);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&full);

        waitingWriters--;
        sem_post(&writerLock);

        sleep(rand() % 2);
    }
    pthread_exit(NULL);
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < (NUM_ITEMS * NUM_PRODUCERS) / NUM_CONSUMERS; i++) {
        sem_wait(&full);
        sem_wait(&mutex);

        int item = buffer[out];
        printf("Consumer %d consumed %d from %d\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&empty);

        sleep(rand() % 3);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t prod[NUM_PRODUCERS], cons[NUM_CONSUMERS];
    int prod_id[NUM_PRODUCERS] = {1, 2};
    int cons_id[NUM_CONSUMERS] = {1, 2};

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);
    sem_init(&writerLock, 0, 1);

    for(int i=0;i<NUM_PRODUCERS;i++)
        pthread_create(&prod[i], NULL, producer, &prod_id[i]);
    for(int i=0;i<NUM_CONSUMERS;i++)
        pthread_create(&cons[i], NULL, consumer, &cons_id[i]);

    for(int i=0;i<NUM_PRODUCERS;i++)
        pthread_join(prod[i], NULL);
    for(int i=0;i<NUM_CONSUMERS;i++)
        pthread_join(cons[i], NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
    sem_destroy(&writerLock);

    printf("All production and consumption completed.\n");
    return 0;
}
