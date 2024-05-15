#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>

#define X_RES_SLIKE 640
#define Y_RES_SLIKE 480
#define RGB565_BPP 2
#define X_RES_FRAMEBUFFERJA 1920

#define SEM2_READ 0
#define SEM2_WRITE 1

int main(){

	int framebuffer = open("/dev/fb0", O_WRONLY);
	if (framebuffer == -1){

		printf("napaka pri odpiranju framebufferja\n");
		exit(1);
	}

	int kljuc2 = ftok("shm2", 'S');
	if(kljuc2 == -1){

		printf("napaka pri ustvarjanju kljuca2\n");
		exit(2);
	}

	int shmID2 = shmget(kljuc2, 0, 0);
	if(shmID2 == -1){

		printf("napaka pri ustvarjanju deljenega pomnilnika2\n");
		exit(3);
	}

	char* pomnilnik = (char*)shmat(shmID2, NULL, 0);

	int sem2 = semget(kljuc2, 2, 0);
	if(sem2 == -1){

		printf("napaka pri ustvarjanju semaforja2\n");
		exit(5);
	}

	while(1){

		struct sembuf read2 = {SEM2_READ, -1, 0};
		semop(sem2, &read2, 1);

		for (int vrstica = 0; vrstica < Y_RES_SLIKE; ++vrstica){

	    		int offset = vrstica * X_RES_FRAMEBUFFERJA * RGB565_BPP;
	    		char* zamik = pomnilnik + (vrstica * X_RES_SLIKE * RGB565_BPP);

			if (lseek(framebuffer, offset, SEEK_SET) == -1){

				printf("napaka pri nastavljanju kazalca pri framebufferju\n");
				exit(5);
			}

			int st_zapisanih_bajtov = write(framebuffer, zamik, X_RES_SLIKE * RGB565_BPP);
			if (st_zapisanih_bajtov == -1){

				printf("napaka pri pisanju v framebuffer\n");
				exit(6);
			}
		}

		struct sembuf write2 = {SEM2_WRITE, 1, 0};
		semop(sem2, &write2, 1);

	}
}
