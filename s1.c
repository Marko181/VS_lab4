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

#define SEM1_READ 0
#define SEM1_WRITE 1

int main(){

	int vhodna_datoteka = open("/dev/video0", O_RDONLY);
	if (vhodna_datoteka == -1){

		printf("napaka pri odpiranju /dev/video0\n");
		exit(1);
	}

	int kljuc1 = ftok("shm1", 'S');
	if(kljuc1 == -1){

		printf("napaka pri ustvarjanju kljuca1\n");
		exit(2);
	}

	int shmID1 = shmget(kljuc1, X_RES_SLIKE * Y_RES_SLIKE * RGB24_BPP, IPC_CREAT | 0600);
	if(shmID1 == -1){

		printf("napaka pri ustvarjanju deljenega pomnilnika\n");
		exit(3);
	}

	char* pomnilnik = (char*)shmat(shmID1, NULL, 0);

	int sem1 = semget(kljuc1, 2, IPC_CREAT | 0600);
	if(sem1 == -1){

		printf("napaka pri ustvarjanju semaforja1\n");
		exit(4);
	}

	semctl(sem1, SEM1_READ, SETVAL, 0);
	semctl(sem1, SEM1_WRITE, SETVAL, 1);

	while(1){

		struct sembuf write1 = {SEM1_WRITE, -1, 0};
		semop(sem1, &write1, 1);

            	int st_prebranih_bajtov = read(vhodna_datoteka, pomnilnik, X_RES_SLIKE * Y_RES_SLIKE * RGB24_BPP);
	    	if (st_prebranih_bajtov == -1){

	     		printf("napaka pri branju iz /dev/video0\n");
	     		exit(4);
	    	}

		struct sembuf read1 = {SEM1_READ, 1, 0};
		semop(sem1, &read1, 1);
	}
}
