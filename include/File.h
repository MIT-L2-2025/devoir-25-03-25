#pragma once

#include <sys/stat.h>
#include <sys/types.h>

off_t getSize(struct stat *status);
char *getMode(struct stat *status);
char *getPermission(struct stat *status);
ino_t getInode(struct stat *status);
void display(char *dir);