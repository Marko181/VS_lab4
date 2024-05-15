#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

FILE *ptr;
FILE *write_ptr;

int main( ){

int fi, fo, d = 1280, v = 1024;
int SIZEI = 640*2;
int SIZE_SCREEN = d * 2 - 1280;
char *buf, *bufO, *bufB;
buf = (char *) malloc(sizeof(char) * SIZEI);

fi = open("fifo_izhod", O_RDONLY);
fo = open("/dev/fb0", O_WRONLY);
while(1){
for (int i = 0; i < 480; i++) {
int bytesRead = read(fi, buf, SIZEI);
            if (bytesRead != SIZEI) {
                fprintf(stderr, "Read error: Expected %d bytes, got %d bytes\n", SIZEI, bytesRead);
                break;
            }
            int bytesWritten = write(fo, buf, SIZEI);
            if (bytesWritten != SIZEI) {
                fprintf(stderr, "Write error: Expected %d bytes, wrote %d bytes\n", SIZEI, bytesWritten);
            }
lseek(fo,2*1280*i,SEEK_SET);
}
lseek(fo,0,SEEK_SET);
}
    
close(fi);
close(fo);

        free(buf);

        printf("Done!\n");

        return 0;
}