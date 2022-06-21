#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/wait.h>

#define SHAREDMEMKEY 6641297

typedef struct {
    int a;
    int NE;
    int NR;
    int TE;
    int TR;
    int shmid;
    int elves;
    int reindeer;
    sem_t santa_sem;
    sem_t reindeer_sem;
    sem_t elf_tex;
    sem_t mutex;
    sem_t workshop;
    sem_t write;
    sem_t help_sem;
    sem_t hitched_sem;
    bool vacation;
} shared_mem;

/**
 *  Destroy created semaphores and shared memory. 
 *  @param shared memory pointer
 */
void cleanup(shared_mem *shm) {
    sem_destroy(&shm->hitched_sem);
    sem_destroy(&shm->help_sem);
    sem_destroy(&shm->write);
    sem_destroy(&shm->workshop);
    sem_destroy(&shm->mutex);
    sem_destroy(&shm->elf_tex);
    sem_destroy(&shm->reindeer_sem);
    sem_destroy(&shm->santa_sem);

    int shmid = shm->shmid;
    assert(shmdt(shm) == 0);
    shmctl(shmid, IPC_RMID, NULL);

    return;
}

void santa(shared_mem *shm) {
    sem_wait(&shm->write);
    printf("%d: Santa: going to sleep", shm->a++);
    sem_post(&shm->write);
    while (true) {
        sem_wait(&shm->santa_sem);
        sem_wait(&shm->mutex);
            if (shm->reindeer == shm->NR) {
                sem_wait(&shm->write);
                printf("\n%d: Santa: closing workshop", shm->a++);
                
                shm->vacation = true;
                for (int i = 0; i < shm->reindeer; i++) {
                    sem_post(&shm->reindeer_sem);
                }
                sem_post(&shm->write);
                sem_wait(&shm->hitched_sem);
                sem_wait(&shm->write);
                printf("\n%d: Santa: Christmas started", shm->a++);
                sem_post(&shm->write);
                while (shm->NE > 0) {
                    sem_post(&shm->workshop);
                    sem_post(&shm->mutex);
                }
                return;
            } else if (shm->elves == 3) {
                sem_wait(&shm->write);
                if (shm->reindeer == shm->NR) {
                    sem_post(&shm->santa_sem);
                    sem_post(&shm->mutex);
                    sem_post(&shm->write);
                    continue;
                } 
                printf("\n%d: Santa: helping elves", shm->a++);
                
                for (int i = 0; i < shm->elves; i++) {
                    sem_post(&shm->workshop);
                    sem_post(&shm->mutex);
                }
                sem_post(&shm->write);
                sem_wait(&shm->help_sem);
                sem_wait(&shm->write);
                printf("\n%d: Santa: going to sleep", shm->a++);
                sem_post(&shm->write);
            }
        sem_post(&shm->mutex);
    }
    exit(0);
}

void reindeer(shared_mem *shm, int id) {
    srand(time(NULL) * getpid());
    sem_wait(&shm->write);
    printf("\n%d: RD %d: rstarted", shm->a++, id);
    sem_post(&shm->write);
    while (true) {
        usleep((rand() % (shm->TR + 1 - shm->TR/2) + shm->TR/2) * 1000);
        sem_wait(&shm->mutex);
            sem_wait(&shm->write);
            printf("\n%d: RD %d: return home", shm->a++, id);
            shm->reindeer++;
            sem_post(&shm->write);
            if (shm->reindeer == shm->NR) {
                sem_post(&shm->santa_sem);
            }
        sem_post(&shm->mutex);
        
        sem_wait(&shm->reindeer_sem);
        sem_wait(&shm->write);
        printf("\n%d: RD %d: get hitched", shm->a++, id);
        sem_post(&shm->write);
        shm->reindeer--;
        if (shm->reindeer == 0) {
            sem_post(&shm->hitched_sem);
        }
        exit(0);
    }
}

void elf(shared_mem *shm, int id) {
    srand(time(NULL) * getpid());
    sem_wait(&shm->write);
    printf("\n%d: Elf %d: started", shm->a++, id);
    sem_post(&shm->write);
    while (true) {
        usleep((rand() % (shm->TR + 1)) * 1000);

        sem_wait(&shm->elf_tex);
        sem_wait(&shm->mutex);
            if (shm->vacation) {
                sem_wait(&shm->write);
                printf("\n%d: Elf %d: need help", shm->a++, id);
                sem_post(&shm->write);
                shm->elves++;
            } else {
                sem_wait(&shm->write);
                printf("\n%d: Elf %d: need help", shm->a++, id);
                sem_post(&shm->write);
                shm->elves++;
                if (shm->elves == 3) {
                    sem_post(&shm->santa_sem);
                } else {
                    sem_post(&shm->elf_tex);
                }  
            }
        sem_post(&shm->mutex);
        
        sem_wait(&shm->workshop);
        sem_wait(&shm->mutex);
            if (shm->vacation) {
                sem_wait(&shm->write);
                printf("\n%d: Elf %d: taking holidays", shm->a++, id);
                sem_post(&shm->write);
                shm->NE--;
                sem_post(&shm->elf_tex);
                sem_post(&shm->mutex);
                exit(0);
            } else {
                sem_wait(&shm->write);
                printf("\n%d: Elf %d: get help", shm->a++, id);
                sem_post(&shm->write);
                shm->elves--;
                if (shm->elves == 0){
                    sem_post(&shm->elf_tex);
                    sem_post(&shm->help_sem);
                }
            }
        sem_post(&shm->mutex);
    }
}

int main(int argc, char const *argv[])
{   
    // Arguments value check
    assert(argc == 5);
    int NE = atoi(argv[1]);
    int NR = atoi(argv[2]);
    int TE = atoi(argv[3]);
    int TR = atoi(argv[4]);
    assert(NE > 0 && NE < 1000);
    assert(NR > 0 && NR < 20);
    assert(TE >= 0 && TE <= 1000);
    assert(TR >= 0 && TR <= 1000);

    // Allocate shared memory
    int shmid = shmget(SHAREDMEMKEY, sizeof(shared_mem), IPC_CREAT | 0666);
    assert(shmid >= 0);

    shared_mem *shm = shmat(shmid, NULL, 0);
    assert(shm != (shared_mem *) -1);
    shm->shmid = shmid;

    // Initialize semaphores
    int e = 0; // sem_init returns -1 on error, if e < 0, clean and exit
    e += sem_init(&shm->santa_sem, 1, 0);
	e += sem_init(&shm->reindeer_sem, 1, 0);
	e += sem_init(&shm->elf_tex, 1, 1);
	e += sem_init(&shm->mutex, 1, 1);
	e += sem_init(&shm->workshop, 1, 0);
    e += sem_init(&shm->write, 1, 1);
    e += sem_init(&shm->help_sem, 1, 0);
    e += sem_init(&shm->hitched_sem, 1, 0);
    if (e < 0) {cleanup(shm); assert(false);}

    // Insert values
    shm->NE = NE;
    shm->NR = NR;
    shm->TE = TE;
    shm->TR = TR;
    shm->vacation = false;
    shm->a = 1;
    
    FILE *fp = freopen("proj2.out", "w", stdout);
    setvbuf (stdout, NULL, _IONBF, BUFSIZ);
    // Fork processes
    pid_t santa_PID = fork();
    if (santa_PID == 0) {
        sleep(1);
        for (int i = 0; i < shm->NR; i++) {
            pid_t  reindeer_PID = fork();
            if (reindeer_PID == 0) {
                reindeer(shm, i + 1);
            }
        }
    } else if (santa_PID > 0) {
        pid_t elves_PID = fork();
        if (elves_PID == 0) {
            sleep(1);
            for (int i = 0; i < shm->NE; i++) {
                pid_t  elf_PID = fork();
                if (elf_PID == 0) {
                    elf(shm, i + 1);
                }
            }
        } else if (elves_PID > 0) {
            santa(shm);
            while(wait(NULL) > 0);
        }
    }

    fclose(fp);
    cleanup(shm);
    return 0;
}
