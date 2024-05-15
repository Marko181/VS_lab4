#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SEM1_READ 0
#define SEM1_WRITE 1
#define IMAGE_SIZE 640*480*3

int main(){
    int in_file = open("/dev/video0", O_RDONLY);
	if (in_file == -1){

		printf("Error reading from /dev/video0\n");
		return 1;
	}

	int key1 = ftok("key1", 'S');
	if(key1 == -1){

		printf("Error creating key 1\n");
		return 1;
	}

    int shmID1 = shmget(key1, IMAGE_SIZE, IPC_CREAT | 0600);
	if(shmID1 == -1){

		printf("Error creating shared memory\n");
		return 1;
	}

    // If this parameter is set to NULL, the system will choose a suitable address.
    // 0 - deafult permissions (read and write)
	char* buff = (char*)shmat(shmID1, NULL, 0);

    // lahko bi tudi ustvarila 4 semaforje tukaj
	int sem1 = semget(key1, 2, IPC_CREAT | 0600);
	if(sem1 == -1){

		printf("Error creating semaphor 1\n");
		return 1;
	}

	semctl(sem1, SEM1_READ, SETVAL, 0);
	semctl(sem1, SEM1_WRITE, SETVAL, 1);

    while(1){
        // 0 - no special flags set
		struct sembuf write1 = {SEM1_WRITE, -1, 0};

		if (semop(sem1, &write1, 1) == -1) {
            perror("semop error");
            return 1;
        }

        int bytes_read = read(in_file, buff, IMAGE_SIZE);
	    if (bytes_read == -1){

	        printf("Error reading /dev/video0\n");
	    	return 1;
	    }

		struct sembuf read1 = {SEM1_READ, 1, 0};
		semop(sem1, &read1, 1);
	}

    return 0;
}