#include "../include/File.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]) {
    struct stat status;
    // off_t size;
    // char *mode;
    // char *permisson;
    // ino_t inode;

    srand(time(NULL));
    char* dirs[] = {"/etc", "/usr/local", "/var/log"};
    char* selectedDir = dirs[rand() % 3];
    
    // if(fstat(STDIN_FILENO,&status) < 0) {
    //     printf("Erreur : %s à la ligne %d.\n\n",__FILE__,__LINE__);
    //     return -1;
    // }else {
    //     size = getSize(&status);
    //     mode = getMode(&status);
    //     permisson = getPermission(&status);
    //     inode = getInode(&status);
    //     printf("Inode     : %ld\n",inode);
    //     printf("Size      : %ld\n",size);
    //     printf("Mode      : %s\n",mode);
    //     printf("Permisson : %s\n",permisson);
    // }

    // if(fstat(STDOUT_FILENO,&status) < 0) {
    //     printf("Erreur : %s à la ligne %d.\n\n",__FILE__,__LINE__);
    //     return -1;
    // }else {
    //     size = getSize(&status);
    //     mode = getMode(&status);
    //     permisson = getPermission(&status);
    //     inode = getInode(&status);
    //     printf("Inode     : %ld\n",inode);
    //     printf("Size      : %ld\n",size);
    //     printf("Mode      : %s\n",mode);
    //     printf("Permisson : %s\n",permisson);
    // }

    // if(stat("/etc/passwd",&status) < 0) {
    //     printf("Erreur : %s à la ligne %d.\n\n",__FILE__,__LINE__);
    //     return -1;
    // }else {
    //     size = getSize(&status);
    //     mode = getMode(&status);
    //     permisson = getPermission(&status);
    //     inode = getInode(&status);
    //     printf("Inode     : %ld\n",inode);
    //     printf("Size      : %ld\n",size);
    //     printf("Mode      : %s\n",mode);
    //     printf("Permisson : %s\n",permisson);
    //     printf("Bloc : %ld\n",status.st_blocks);
    // }

    display(selectedDir);

    return 0;
}