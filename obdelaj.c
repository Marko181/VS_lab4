#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
while(1){
read(fi,bufI,SIZEI);

for(a = 0; a < 640; a++){
R = bufI[a*3];
G = bufI[a*3+1];
B = bufI[a*3+2];

bufO[a*2+1] = (R & 0xF8) | ((G & 0xE0)>>5);
bufO[a*2] = ((G & 0x1C)<<3) | ((B & 0xF8)>>3);

}
write(fo, bufO, SIZEO);
}
        printf("Done obdelaj!\n");
        free(bufI);
        free(bufO);

return 0;
}