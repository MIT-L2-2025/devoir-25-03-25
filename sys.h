#ifndef __SYS__H
#define __SYS__H

#include <stdio.h>
#include <sys/stat.h>

//mis en structure uniquement car la structure stat ne contient pas d'attribut nom
typedef struct {
    struct stat info;
    char file_path[256];
} About_file;

char *pick_dir();
FILE *get_list_file(char *dir);
int count_line(FILE* file);
int collect_from_files(FILE *list_file, About_file *files);
void sort_by_inode(About_file *files, int count);
void sort_by_name(About_file *files, int count);
void sort_by_size(About_file *files, int count);
void html_head(FILE *html, char *dir, const char *sort_type);
void html_line(FILE *html, About_file *entry);
void html_foot(FILE *html);
char *human_readable_size(long size);

#endif