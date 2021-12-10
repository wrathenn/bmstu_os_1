#include <stdio.h>
#include "stdlib.h"
#include "time.h"
#include "sys/stat.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define SEM_AMOUNT 3

#define SEM_BIN 0
#define SEM_E 1
#define SEM_F 2

#define EMPTY_NUM 20

#define PRODUCE_NUM 6
#define CONSUME_NUM 6

#define PROC_AMOUNT 3

#define SEM_ERR 1
#define SEM_SET_ERR 2
#define SHM_ERR 3
#define MEM_ERR 4
#define FORK_ERR 5
#define SEMOP_ERR 6

typedef struct buf {
    int rpos;
    int wpos;
    char data[EMPTY_NUM];
} buf_t;

buf_t *shm = NULL;

char *alphabet = "abcdefghijklmnopqrstuvwxyz";

struct sembuf prod_start[] = {
        {SEM_E,   -1, 0},
        {SEM_BIN, -1, 0}
};

struct sembuf prod_end[] = {
        {SEM_BIN, 1, 0},
        {SEM_F,   1, 0}
};

struct sembuf read_start[] = {
        {SEM_F,   -1, 0},
        {SEM_BIN, -1, 0}
};

struct sembuf read_end[] = {
        {SEM_BIN, 1, 0},
        {SEM_E,   1, 0}
};

void producer(int semID, int prodID) {
    srand(time(NULL));
    sleep(rand() % 4);

    if (semop(semID, prod_start, 2) == -1) {
        perror("Producer semop begin error");
        exit(SEMOP_ERR);
    }

    int wpos = shm->wpos;
    shm->data[wpos] = alphabet[wpos];

    printf("<< Producer[ID = %d]: wrote %c\n", prodID, shm->data[wpos]);
    shm->wpos++;

    if (semop(semID, prod_end, 2) == -1) {
        perror("Producer semop end error");
        exit(SEMOP_ERR);
    }
}

void consumer(int semID, int consID) {
    srand(time(NULL));
    sleep(rand() % 4);

    if (semop(semID, read_start, 2) == -1) {
        perror("Consumer semop begin error");
        exit(SEMOP_ERR);
    }

    printf(">> Consumer[ID = %d]: read %c\n", consID, shm->data[shm->rpos]);
    shm->rpos++;

    if (semop(semID, read_end, 2) == -1) {
        perror("Consumer semop end error");
        exit(SEMOP_ERR);
    }
}

int main() {
    int perms = S_IRWXU | S_IRWXG | S_IRWXO;
    int semID = semget(IPC_PRIVATE, SEM_AMOUNT, IPC_CREAT | perms);
    if (semID == -1) {
        perror("Semaphore creation error.");
        exit(SEM_ERR);
    }

    if (semctl(semID, SEM_BIN, SETVAL, 1) == -1 ||
        semctl(semID, SEM_E, SETVAL, EMPTY_NUM) == -1 ||
        semctl(semID, SEM_F, SETVAL, 0) == -1) {
        perror("Semaphore set error.");
        exit(SEM_SET_ERR);
    }

    int shmID = shmget(IPC_PRIVATE, sizeof(buf_t), IPC_CREAT | perms);
    if (shmID == -1) {
        perror("Shared memory creation error.");
        exit(SHM_ERR);
    }

    shm = shmat(shmID, 0, 0);
    if (shm == (void *) -1) {
        perror("Memory all error.");
        exit(MEM_ERR);
    }

    pid_t childID;
    for (int i = 0; i < PROC_AMOUNT; ++i) {
        if ((childID = fork()) == -1) {
            perror("Producer fork error");
            exit(FORK_ERR);
        }
        else if (childID == 0) {
            for (int j = 0; j < PRODUCE_NUM; ++j) {
                producer(semID, i);
            }
            exit(0);
        }

        if ((childID = fork()) == -1) {
            perror("Consumer fork error");
            exit(FORK_ERR);
        }
        else if (childID == 0) {
            for (int j = 0; j < CONSUME_NUM; ++j) {
                consumer(semID, i);
            }
            exit(0);
        }
    }

    int status;
    for (int i = 0; i < PROC_AMOUNT * 2; i++) {
        wait(&status);
    }

    if (shmdt(shm) == -1) {
        perror("smhdt error");
        exit(MEM_ERR);
    }

    if (shmctl(shmID, IPC_RMID, NULL) == -1) {
        perror("shmctl error");
        exit(MEM_ERR);
    }

    exit(0);
}
