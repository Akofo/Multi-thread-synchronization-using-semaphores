#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <iostream>
using namespace std;

class Semaphore {
public:
    Semaphore(int initialValue)
    {
        sem_init(&mSemaphore, 0, initialValue);
    }

    ~Semaphore()
    {
        sem_destroy(&mSemaphore);
    }
    
    void wait()
    {
        sem_wait(&mSemaphore);
    }

    void signal()
    {
        sem_post(&mSemaphore);
    }
    
private:
    sem_t mSemaphore;
};
