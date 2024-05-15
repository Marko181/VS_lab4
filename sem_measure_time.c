#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

int main(){

	int in_file = open("/dev/video0", O_RDONLY);
	if (in_file == -1){

		printf("Error opening /dev/video0\n");
		return 1;
	}

	int key1 = ftok("key1", 'S');
	if(kljuc1 == -1){

		printf("Error creating key1\n");
		return 1;
	}

	int shmID1 = shmget(key1, 640*480*3, IPC_CREAT | 0600);
	if(shmID1 == -1){

		printf("Error creating shared memory\n");
		return 1;
	}

	char* buff = (char*)shmat(shmID1, NULL, 0);

	double time_start = clock();

        int bytes_read = read(in_file, buff, 640*480*3);
	if (bytes_read == -1){

	     	printf("Error reading from /dev/video0\n");
	     	return 1;
	}

	double time_end = clock();

	double delta_time = ((double)(time_end - time_start)) / CLOCKS_PER_SEC;
	printf("Delta time shared memory: %f\n", delta_time);

	return 0;
}