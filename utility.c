#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int hit0, hit1;
int miss0, miss1;
int hit_diff, miss_diff;
float hit_ratio, miss_ratio;

int main(){


    while(1){
        char* buffer;
        hit_ratio = 0;
        miss_ratio = 0;
        int fd0 = open("/proc/sys/vm/pagecache_hits", O_RDONLY);

        if (fd0 < 0){
            printf("pagecache_hits could not be opened\n");
            return -1;
        }

        lseek(fd0, 0, SEEK_SET);
        read(fd0, buffer, 16);
        hit1 = atoi(buffer);       
        hit_diff = hit1 - hit0;

        if(close(fd0) < 0){
            printf("Failed to close pagecache_hits.\n");
            return -1;
        }
        
        memset(buffer, '\0', 16);
        int fd1 = open("/proc/sys/vm/pagecache_misses", O_RDONLY);

        if (fd1 < 0){
            printf("\npagecache_misses could not be opened\n");
            return -1;
        }
        
        lseek(fd1, 0, SEEK_SET);
        read(fd1, buffer, 16);
        miss1 = atoi(buffer);

        miss_diff = miss1 - miss0;

        if(hit_diff == 0)
            hit_ratio = 0;
        else
            hit_ratio = (float) (hit_diff / (float) (hit_diff + miss_diff));
                
        if(miss_diff == 0)
            miss_ratio = 0;
        else
            miss_ratio = (float) (miss_diff / (float) (hit_diff + miss_diff));

        printf("hit_ratio: %5.2f (%d) miss_ratio: %5.2f (%d)\n", hit_ratio, hit_diff, miss_ratio, miss_diff);

        if(close(fd1) < 0){
            printf("Failed to close pagecache_misses\n");
            return -1;
        }

        hit0 = hit1;
        miss0 = miss1;

        sleep(1);
    }   
}
