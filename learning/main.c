#include <unistd.h>
#include <fcntl.h>
#include "string.h"
#include "stdio.h"


int main(){
    FILE *fp = NULL;
    fp = fopen("test","w");

    fwrite("sunwenli", strlen("sunwenli"),1,fp);
    fseek(fp,0,SEEK_SET);
    char a = 0;
//    fwrite(&a, 1,1,fp);
//    fputc( EOF , fp );
//    fwrite(&a, 1,1,fp);
//    fwrite(&a, 1,1,fp);
//    fwrite(&a, 1,1,fp);
//    fwrite(&a, 1,1,fp);
//    truncate("test",5);
    fclose(fp);
//    truncate("test",5);
    int fd = open("test",O_WRONLY);
    lseek(fd, 0, SEEK_SET);
    ftruncate(fd,5);
    close(fd);
    char str[100];
    fp = fopen("test","r");
    printf("%ld\n",fread(str, 3,1,fp));
    puts(str);
    fclose(fp);
}