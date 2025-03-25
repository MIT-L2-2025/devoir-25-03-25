#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

void scanDir(char* path, char* device) {
    DIR* dir = opendir(path);
    if(!dir) {
        return;
    }

    struct dirent* entry;
    while((entry = readdir(dir))) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        struct stat st;
        if(lstat(fullPath, &st)){
            continue;
        } 

        if(S_ISREG(st.st_mode)) {
            printf("=> \033[1mFichier : %s\033[0m\n", fullPath);
            printf("\t- \033[34mID    :\033[0m %lu\n", (unsigned long)st.st_ino);
            printf("\t- \033[34mTaille:\033[0m %ld octets\n", (long)st.st_size);
            printf("---------------------------------\n");
        }
        else if(S_ISDIR(st.st_mode)) {
            scanDir(fullPath, device);
        }
    }
    closedir(dir);
}

int main() {
    srand(time(NULL));
    char* dirs[] = {"/etc", "/usr/local", "/var/log"};
    char* selectedDir = dirs[rand() % 3];
    
    char device[256];
    FILE* fp = popen("df -P /etc | awk 'NR==2 {print $1}'", "r");
    fgets(device, sizeof(device), fp);
    pclose(fp);
    
    printf("ANALYSE DE : \033[1m%s\033[0m SUR LE PÉRIPHÉRIQUE : \033[1m%s\033[0m\n", selectedDir, device);
    scanDir(selectedDir, device);
    
    return 0;
}