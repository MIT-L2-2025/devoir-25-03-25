#include "../include/File.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]) {
    struct stat status;

    srand(time(NULL));
    char* dirs[] = {"/etc", "/usr/local", "/var/log"};
    char* selectedDir = dirs[rand() % 3];

    display(selectedDir);

    return 0;
}
