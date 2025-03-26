#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#define NUM_DIRS 3

typedef struct {
    char path[1024];
    ino_t inode;
    off_t size;
    unsigned long position;
} FileInfo;

const char* get_random_directory();
void generate_html_table(FileInfo *files, int num_files, const char *dir);