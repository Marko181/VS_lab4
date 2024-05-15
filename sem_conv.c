#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define IMG_SIZE3 640*480*3
#define IMG_SIZE2 640*480*2
#define IMG_SIZE 640*480

#define SEM1_READ 0
#define SEM1_WRITE 1

#define SEM2_READ 0
#define SEM2_WRITE 1

int main() {
    int key1 = ftok("key1", 'S');
	if(key1 == -1){

		printf("Error creating key 1\n");
		return 1;
	}

    int shmID1 = shmget(key1, IMG_SIZE3, IPC_CREAT | 0600);
	if(shmID1 == -1){

		printf("Error creating shared memory 1\n");
		return 1;
	}

    int key2 = ftok("key2", 'S');
	if(key2 == -1){

		printf("Error creating key 2\n");
		return 1;
	}

    int shmID2 = shmget(key2, IMG_SIZE2, IPC_CREAT | 0600);
	if(shmID2 == -1){

		printf("Error creating shared memory 2\n");
		return 1;
	}

    char* in_buff = (char*)shmat(shmID1, NULL, 0);
    char* out_buff = (char*)shmat(shmID2, NULL, 0);

    // sem1 smo naredili že prej zato ga samo kličemo, sem2 pa ustvarimo
    int sem1 = semget(key1, 2, 0);
	if(sem1 == -1){

		printf("Error creating semaphor 1\n");
		return 1;
	}

    int sem2 = semget(key2, 2, IPC_CREAT | 0600);
	if(sem2 == -1){

		printf("Error creating semaphor 2\n");
		return 1;
	}

    semctl(sem2, SEM2_READ, SETVAL, 0);
	semctl(sem2, SEM2_WRITE, SETVAL, 1);

    while(1){

        struct sembuf read1 = {SEM1_READ, -1, 0};
		semop(sem1, &read1, 1);

		struct sembuf write2 = {SEM2_WRITE, -1, 0};
		semop(sem2, &write2, 1);

        for(int i = 0; i < IMG_SIZE; i++){
            char R = in_buff[i*3];
            char G = in_buff[i*3+1];
            char B = in_buff[i*3+2];

            out_buff[i*2+1] = (R & 0xF8) | ((G & 0xE0)>>5);
            out_buff[i*2] = ((G & 0x1C)<<3) | ((B & 0xF8)>>3);
        }

        struct sembuf write1 = {SEM1_WRITE, 1, 0};
		semop(sem1, &write1, 1);

		struct sembuf read2 = {SEM2_READ, 1, 0};
		semop(sem2, &read2, 1);
    }
    
    return 0;
}
