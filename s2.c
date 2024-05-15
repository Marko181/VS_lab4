#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>

#define X_RES_SLIKE 640
#define Y_RES_SLIKE 480
#define RGB24_BPP 3
#define RGB565_BPP 2

#define SEM1_READ 0
#define SEM1_WRITE 1

#define SEM2_READ 0
#define SEM2_WRITE 1

int main(){

	int kljuc1 = ftok("shm1", 'S');
	if(kljuc1 == -1){

		printf("napaka pri ustvarjanju kljuca1\n");
		exit(1);
	}

	int kljuc2 = ftok("shm2", 'S');
	if(kljuc2 == -1){

		printf("napaka pri ustvarjanju kljuca2\n");
		exit(2);
	}

	int shmID1 = shmget(kljuc1, 0, 0);
	if(shmID1 == -1){

		printf("napaka pri ustvarjanju deljenega pomnilnika1\n");
		exit(3);
	}

	int shmID2 = shmget(kljuc2, X_RES_SLIKE * Y_RES_SLIKE * RGB565_BPP, IPC_CREAT | 0600);
	if(shmID2 == -1){

		printf("napaka pri ustvarjanju deljenega pomnilnika2\n");
		exit(4);
	}

	char* vh_pomnilnik = (char*)shmat(shmID1, NULL, 0);
	unsigned short* izh_pomnilnik = (unsigned short*)shmat(shmID2, NULL, 0);

	int sem1 = semget(kljuc1, 2, 0);
	if(sem1 == -1){

		printf("napaka pri ustvarjanju semaforja1\n");
		exit(5);
	}

	int sem2 = semget(kljuc2, 2, IPC_CREAT | 0600);
	if(sem2 == -1){

		printf("napaka pri ustvarjanju semaforja2\n");
		exit(6);
	}

	semctl(sem2, SEM2_READ, SETVAL, 0);
	semctl(sem2, SEM2_WRITE, SETVAL, 1);

	while(1){

		struct sembuf read1 = {SEM1_READ, -1, 0};
		semop(sem1, &read1, 1);

		struct sembuf write2 = {SEM2_WRITE, -1, 0};
		semop(sem2, &write2, 1);

		for (int i = 0; i < X_RES_SLIKE * Y_RES_SLIKE; ++i){

			unsigned char r = vh_pomnilnik[i * 3];
			unsigned char g = vh_pomnilnik[i * 3 + 1];
			unsigned char b = vh_pomnilnik[i * 3 + 2];

			unsigned short rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
			izh_pomnilnik[i] = rgb565;
		}

		struct sembuf write1 = {SEM1_WRITE, 1, 0};
		semop(sem1, &write1, 1);

		struct sembuf read2 = {SEM2_READ, 1, 0};
		semop(sem2, &read2, 1);

	}
}
