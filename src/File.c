#include "../include/File.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>

off_t getSize(struct stat *st) {
    return st->st_size;
}

char *getMode(struct stat *st) {
    if(S_ISBLK(st->st_mode)) {
        return ("bloc");
    }else if(S_ISCHR(st->st_mode)) {
        return ("caractère");
    }else if(S_ISDIR(st->st_mode)) {
        return ("dossier");
    }else if(S_ISFIFO(st->st_mode)) {
        return ("fifo");
    }else if(S_ISLNK(st->st_mode)) {
        return ("link");
    }else if(S_ISREG(st->st_mode)) {
        return ("fichier");
    }else if(S_ISSOCK(st->st_mode)) {
        return ("socket");
    }
}

char *getPermission(struct stat *st) {
    char *res;
    res = malloc(15);
    sprintf(
        res,
        "%s%s%s%s%s%s%s%s%s",
        st->st_mode & S_IRUSR ? "r" : "-",
        st->st_mode & S_IWUSR ? "w" : "-",
        st->st_mode & S_IXUSR ? "x" : "-",
        st->st_mode & S_IRGRP ? "r" : "-",
        st->st_mode & S_IWGRP ? "w" : "-",
        st->st_mode & S_IXGRP ? "x" : "-",
        st->st_mode & S_IROTH ? "r" : "-",
        st->st_mode & S_IWOTH ? "w" : "-",
        st->st_mode & S_IXOTH ? "x" : "-"
    );
    
    return res;
}

ino_t getInode(struct stat *st) {
    return st->st_ino;
}

void display(char *path) {
    DIR* dir = opendir(path);
    off_t size;
    char *mode;
    char *permisson;
    ino_t inode;
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
            if(stat(fullPath,&st) < 0) {
                printf("Erreur : %s à la ligne %d.\n\n",__FILE__,__LINE__);
                return;
            }else {
                printf("=> \033[1mFichier : %s\033[0m\n", fullPath);
                size = getSize(&st);
                mode = getMode(&st);
                permisson = getPermission(&st);
                inode = getInode(&st);
                printf("\t -> Inode     : %ld\n",inode);
                printf("\t -> Size      : %ld\n",size);
                printf("\t -> Mode      : %s\n",mode);
                printf("\t -> Permisson : %s\n",permisson);
                printf("\t -> Bloc      : %ld\n",st.st_blocks);
                free(permisson);
            }
            printf("---------------------------------\n");
        }
        else if(S_ISDIR(st.st_mode)) {
            display(fullPath);
        }
    }
    closedir(dir);
}