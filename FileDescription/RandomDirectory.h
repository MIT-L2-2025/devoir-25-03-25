#ifndef RANDOM_DIRECTORY
#define RANDOM_DIRECTORY

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
     
typedef struct 
{
    int cylindre;
    int tete;
    int secteur;
}Disque;

char *RandomDirectory();
int GetRandomNumber();
void PrintFromDirectory();
void PrintInfo(FILE *file,char *path, const char *loadpath);
void GetGeoDisque(Disque *);
void HeadHtml(FILE *file);
void TailHtml(FILE *file);

#endif