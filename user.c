#include <unistd.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <fcntl.h>
 #include <linux/fb.h>
 #include <sys/mman.h>
 #include <sys/ioctl.h> 
 
 #define PAGE_SIZE 4096
 
 char high[5]={1,0,0,0,0};
 char normal[5]={1,2,3,15,0};
 char low[5]={1,80,15,31,0};
  
 char high1[5]={1,80,15,31,1};
  char high2[5]={1,80,15,31,2};
   char high3[5]={1,80,15,31,3};
 int main(int argc , char *argv[])
 {
     int fd;
     int i;
     unsigned char *p_map;
     char* data;

     if(strcmp(argv[1],"0")==0)
     {
         data=high;
     }
     else if(strcmp(argv[1],"1")==0)
     {
         data=normal;
     }
     if(strcmp(argv[1],"2")==0)
     {
         data=low;
     }
     if(strcmp(argv[1],"3")==0)
     {
         data=high1;
     }
     if(strcmp(argv[1],"4")==0)
     {
         data=high2;
     }
     if(strcmp(argv[1],"5")==0)
     {
         data=high3;
     }
     
     fd = open("/dev/mymap",O_RDWR);
     if(fd < 0)
     {
         printf("open fail\n");
         exit(1);
     }
 
     
     p_map = (unsigned char *)mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0);
     if(p_map == MAP_FAILED)
     {
         printf("mmap fail\n");
         goto here;
     }
 
     
     for(i=0;i<5;i++)
        p_map[i]=data[i];
     
 
here:
     munmap(p_map, PAGE_SIZE);
     return 0;
 }
