#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <stdio.h>  
  
int main(int argc, char** argv)  
{  
    int fd,i;  
    int pagesize,offset;  
    char *p_map;  
    struct stat sb;  
  
    /* ȡ��page size */  
    pagesize = sysconf(_SC_PAGESIZE);  
    printf("pagesize is %d\n",pagesize);  
  
    /* ���ļ� */  
    fd = open(argv[1], O_RDWR, 00777);  
    fstat(fd, &sb);  
    printf("file size is %zd\n", (size_t)sb.st_size);  
  
    offset = 0;   
    p_map = (char *)mmap(NULL, pagesize * 2, PROT_READ|PROT_WRITE,   
            MAP_SHARED, fd, offset);  
    close(fd);  
      
    p_map[sb.st_size] = '9';  /* �������ߴ��� */  
    p_map[pagesize] = '9';    /* ���¶δ��� */  
  
    munmap(p_map, pagesize * 2);  
  
    return 0;  }  