#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int main(){
    char* buffer;

    int fd = open("large_file",O_RDONLY);

    if (fd < 0){
        printf("large_file could not be opened\n");
        return -1;
    }
    else{
        printf("large_file was opened\n");
    }

    memset(buffer, 'a', 4096);
    
    for(int i = 0; i < 1000000; i++){
        read(fd, buffer, 4096);
    }

    if(close(fd) < 0){
        printf("Failed to close large_file\n");
        return -1;
    }
    else{
        printf("Closed large_file\n");
    }
}
