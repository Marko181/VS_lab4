#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main(){

	// mkfifo("fifo16", 0644);
	int fi, fo, a, d=1280, v=1024, inx;
	int SIZEI = 640*3;
	int SIZEO = 640*2;
	char *bufI, *bufO, R, G, B;

	bufI = (char *) malloc(sizeof(char) * SIZEI);
	bufO = (char *) malloc(sizeof(char) * SIZEO);
	
	fi=open("fifo_vhod", O_RDONLY);
	fo=open("fifo_izhod", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	
	double time_start = clock();

	read(fi,bufI,SIZEI);

	double time_end = clock();

	double delta_time = ((double)(time_end - time_start)) / CLOCKS_PER_SEC;
	printf("Delta time FIFO read: %f\n", delta_time);

	free(bufI);
	free(bufO);

	return 0;
}
