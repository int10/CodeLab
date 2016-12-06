#include <sys/mman.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <error.h>  
  
#define BUF_SIZE 100  
  
int main(int argc, char **argv)  
{  
    int fd, nread, i;  
    struct stat sb;  
    char *mapped, buf[BUF_SIZE];  
  
    for (i = 0; i < BUF_SIZE; i++) {  
        buf[i] = '#';  
    }  
  
    /* ���ļ� */  
    if ((fd = open(argv[1], O_RDWR)) < 0) {  
        perror("open");  
    }  
  
    /* ��ȡ�ļ������� */  
    if ((fstat(fd, &sb)) == -1) {  
        perror("fstat");  
    }  
  
    /* ���ļ�ӳ�������̵ĵ�ַ�ռ� */  
    if ((mapped = (char *)mmap(NULL, sb.st_size, PROT_READ |   
                    PROT_WRITE, MAP_SHARED, fd, 0)) == (void *)-1) {  
        perror("mmap");  
    }  
  
    /* ӳ�����, �ر��ļ�Ҳ���Բ����ڴ� */  
    close(fd);  
  
    printf("%s", mapped);  
  
    /* �޸�һ���ַ�,ͬ���������ļ� */  
    mapped[20] = '9';  
    if ((msync((void *)mapped, sb.st_size, MS_SYNC)) == -1) {  
        perror("msync");  
    }  
  
    /* �ͷŴ洢ӳ���� */  
    if ((munmap((void *)mapped, sb.st_size)) == -1) {  
        perror("munmap");  
    }  
  
    return 0;  
}  