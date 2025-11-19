#include "semaphore_class.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int problem_number = 0;

const int NUM_READERS_P1 = 5;
const int NUM_WRITERS_P1 = 5;

int readers_count_p1 = 0;
Semaphore roomEmpty_p1(1);
Semaphore turnstile_p1(1);
Semaphore mutex_p1(1);

void *reader_nostarve(void *threadID) {
    int id = (long)threadID;
    
    while(1) {
        turnstile_p1.wait();
        turnstile_p1.signal();
        
        mutex_p1.wait();
        readers_count_p1++;
        if (readers_count_p1 == 1) {
            roomEmpty_p1.wait();
        }
        mutex_p1.signal();
        
        printf("Reader %d: Reading.\n", id);
        fflush(stdout);
        sleep(2);
        
        mutex_p1.wait();
        readers_count_p1--;
        if (readers_count_p1 == 0) {
            roomEmpty_p1.signal();
        }
        mutex_p1.signal();
        
        sleep(1);
    }
    return NULL;
}

void *writer_nostarve(void *threadID) {
    int id = (long)threadID;
    
    while(1) {
        turnstile_p1.wait();
        roomEmpty_p1.wait();
        
        printf("Writer %d: Writing.\n", id);
        fflush(stdout);
        sleep(2);
        
        turnstile_p1.signal();
        roomEmpty_p1.signal();
        
        sleep(1);
    }
    return NULL;
}

const int NUM_READERS_P2 = 5;
const int NUM_WRITERS_P2 = 5;

int readers_count_p2 = 0;
int writers_count_p2 = 0;
Semaphore mutex_readers_p2(1);
Semaphore mutex_writers_p2(1);
Semaphore no_readers_p2(1);
Semaphore no_writers_p2(1);

void *reader_priority(void *threadID) {
    int id = (long)threadID;
    
    while(1) {
        no_readers_p2.wait();
        mutex_readers_p2.wait();
        readers_count_p2++;
        if (readers_count_p2 == 1) {
            no_writers_p2.wait();
        }
        mutex_readers_p2.signal();
        no_readers_p2.signal();
        
        printf("Reader %d: Reading.\n", id);
        fflush(stdout);
        sleep(2);
        
        mutex_readers_p2.wait();
        readers_count_p2--;
        if (readers_count_p2 == 0) {
            no_writers_p2.signal();
        }
        mutex_readers_p2.signal();
        
        sleep(1);
    }
    return NULL;
}

void *writer_priority(void *threadID) {
    int id = (long)threadID;
    
    while(1) {
        mutex_writers_p2.wait();
        writers_count_p2++;
        if (writers_count_p2 == 1) {
            no_readers_p2.wait();
        }
        mutex_writers_p2.signal();
        
        no_writers_p2.wait();
        
        printf("Writer %d: Writing.\n", id);
        fflush(stdout);
        sleep(2);
        
        no_writers_p2.signal();
        
        mutex_writers_p2.wait();
        writers_count_p2--;
        if (writers_count_p2 == 0) {
            no_readers_p2.signal();
        }
        mutex_writers_p2.signal();
        
        sleep(1);
    }
    return NULL;
}

const int NUM_PHILOSOPHERS_P3 = 5;
Semaphore forks_p3[NUM_PHILOSOPHERS_P3] = {
    Semaphore(1), Semaphore(1), Semaphore(1), Semaphore(1), Semaphore(1)
};
Semaphore mutex_p3(1);

void get_forks_p3(int i) {
    mutex_p3.wait();
    forks_p3[i].wait();
    forks_p3[(i+1) % NUM_PHILOSOPHERS_P3].wait();
    mutex_p3.signal();
}

void put_forks_p3(int i) {
    forks_p3[i].signal();
    forks_p3[(i+1) % NUM_PHILOSOPHERS_P3].signal();
}

void *philosopher_solution1(void *threadID) {
    int id = (long)threadID;
    
    while(1) {
        printf("Philosopher %d: Thinking.\n", id);
        fflush(stdout);
        sleep(2);
        
        get_forks_p3(id);
        
        printf("Philosopher %d: Eating.\n", id);
        fflush(stdout);
        sleep(3);
        
        put_forks_p3(id);
    }
    return NULL;
}

const int NUM_PHILOSOPHERS_P4 = 5;
Semaphore forks_p4[NUM_PHILOSOPHERS_P4] = {
    Semaphore(1), Semaphore(1), Semaphore(1), Semaphore(1), Semaphore(1)
};

void get_forks_right_first(int i) {
    forks_p4[(i+1) % NUM_PHILOSOPHERS_P4].wait();
    forks_p4[i].wait();
}

void get_forks_left_first(int i) {
    forks_p4[i].wait();
    forks_p4[(i+1) % NUM_PHILOSOPHERS_P4].wait();
}

void put_forks_p4(int i) {
    forks_p4[i].signal();
    forks_p4[(i+1) % NUM_PHILOSOPHERS_P4].signal();
}

void *philosopher_solution2(void *threadID) {
    int id = (long)threadID;
    
    while(1) {
        printf("Philosopher %d: Thinking.\n", id);
        fflush(stdout);
        sleep(2);
        
        if (id % 2 == 0) {
            get_forks_right_first(id);
        } else {
            get_forks_left_first(id);
        }
        
        printf("Philosopher %d: Eating.\n", id);
        fflush(stdout);
        sleep(3);
        
        put_forks_p4(id);
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <problem #>\n", argv[0]);
        printf("Problem 1: No-starve Readers-Writers\n");
        printf("Problem 2: Writer-Priority Readers-Writers\n");
        printf("Problem 3: Dining Philosophers Solution #1\n");
        printf("Problem 4: Dining Philosophers Solution #2\n");
        return -1;
    }
    
    problem_number = atoi(argv[1]);
    
    pthread_t threads[10];
    
    switch(problem_number) {
        case 1: {
            printf("=== Problem 1: No-starve Readers-Writers ===\n");
            printf("Creating %d readers and %d writers...\n\n", 
                   NUM_READERS_P1, NUM_WRITERS_P1);
            
            for (long i = 0; i < NUM_READERS_P1; i++) {
                pthread_create(&threads[i], NULL, reader_nostarve, (void*)i);
            }
            
            for (long i = 0; i < NUM_WRITERS_P1; i++) {
                pthread_create(&threads[NUM_READERS_P1 + i], NULL, 
                             writer_nostarve, (void*)i);
            }
            
            pthread_exit(NULL);
            break;
        }
        
        case 2: {
            printf("=== Problem 2: Writer-Priority Readers-Writers ===\n");
            printf("Creating %d readers and %d writers...\n\n", 
                   NUM_READERS_P2, NUM_WRITERS_P2);
            
            for (long i = 0; i < NUM_READERS_P2; i++) {
                pthread_create(&threads[i], NULL, reader_priority, (void*)i);
            }
            
            for (long i = 0; i < NUM_WRITERS_P2; i++) {
                pthread_create(&threads[NUM_READERS_P2 + i], NULL, 
                             writer_priority, (void*)i);
            }
            
            pthread_exit(NULL);
            break;
        }
        
        case 3: {
            printf("=== Problem 3: Dining Philosophers Solution #1 ===\n");
            printf("Creating %d philosophers...\n\n", NUM_PHILOSOPHERS_P3);
            
            for (long i = 0; i < NUM_PHILOSOPHERS_P3; i++) {
                pthread_create(&threads[i], NULL, philosopher_solution1, (void*)i);
            }
            
            pthread_exit(NULL);
            break;
        }
        
        case 4: {
            printf("=== Problem 4: Dining Philosophers Solution #2 ===\n");
            printf("Creating %d philosophers...\n\n", NUM_PHILOSOPHERS_P4);
            
            for (long i = 0; i < NUM_PHILOSOPHERS_P4; i++) {
                pthread_create(&threads[i], NULL, philosopher_solution2, (void*)i);
            }
            
            pthread_exit(NULL);
            break;
        }
        
        default:
            printf("Invalid problem number. Please choose 1-4.\n");
            return -1;
    }
    
    return 0;
}
