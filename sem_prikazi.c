#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SEM2_READ 0
#define SEM2_WRITE 1

int main(){
    int fb = open("/dev/fb0", O_WRONLY);
	if (fb == -1){

		printf("Error opening framebuffer\n");
		return 1;
	}

    int key2 = ftok("key2", 'S');
	if(key2 == -1){

		printf("Error creating key 2\n");
		return 1;
	}

    // zakaj je velikost shared memoryja 0????
    int shmID2 = shmget(key2, 0, 0);
	if(shmID2 == -1){

		printf("Error creating shared memory 2\n");
		return 1;
	}

    char* buff = (char*)shmat(shmID2, NULL, 0);

    int sem2 = semget(key2, 2, 0);
	if(sem2 == -1){

		printf("Error creating semaphor 2\n");
		return 1;
	}

    while(1){
        struct sembuf read2 = {SEM2_READ, -1, 0};
		semop(sem2, &read2, 1);

        for (int i = 0; i < 480; i++) {
            int offset = i * 1920 * 2;
	    	char* zamik = buff + (i * 640 * 2);

			if (lseek(fb, offset, SEEK_SET) == -1){

				printf("Error setting framebuffer cursor\n");
				return 1;
			}

			int st_zapisanih_bajtov = write(fb, zamik, 640 * 2);
			if (st_zapisanih_bajtov == -1){

				printf("Error writing to framebuffer\n");
				return 1;
			}
        }
        struct sembuf write2 = {SEM2_WRITE, 1, 0};
		semop(sem2, &write2, 1);
    }
    return 0;
}