#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "sys.h"

char *pick_dir() {
    char *dirs[] = {"/etc", "/usr/local", "/var/log"};
    int num_dirs = 3;
    srand(time(NULL));
    return dirs[rand() % num_dirs];
}

FILE *get_list_file(char *dir) {
    char cmd[256];
    sprintf(cmd, "find %s -maxdepth 1 -type f", dir);
    FILE *list_file = popen(cmd, "r");
    if (!list_file) {
        printf("erreur lors de l'éxecution de la commande \n");
        exit(1);
    }
    return list_file;
}
int count_line(FILE* file) {
    int count = 0;
    int c;                  //j'ai utilisé le comptage par caractère car fgets n'est pas vraiment compatible avec rewind

    if (file == NULL) return -1;

    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            count++; 
        }
    }

    //rewind(file);         //rewind ne fonctionne pas sur les pipes 
    return count;
}

int collect_from_files(FILE *list_file, About_file *files) {
    char file_path[256];
    int count = 0;
    while (fgets(file_path, 256, list_file)) {
        file_path[strlen(file_path) - 1] = '\0';    //enlève le \n a la fin
        strcpy(files[count].file_path, file_path);
        stat(file_path, &files[count].info);
        count++;
    }
    return count;
}

void sort_by_inode(About_file *files, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (files[i].info.st_ino > files[j].info.st_ino) {
                About_file temp = files[i];
                files[i] = files[j];
                files[j] = temp;
            }
        }
    }
}

void sort_by_name(About_file *files, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(files[i].file_path, files[j].file_path) > 0) {
                About_file temp = files[i];
                files[i] = files[j];
                files[j] = temp;
            }
        }
    }
}

void sort_by_size(About_file *files, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (files[i].info.st_size > files[j].info.st_size) {
                About_file temp = files[i];
                files[i] = files[j];
                files[j] = temp;
            }
        }
    }
}

void html_head(FILE *html, char *dir, const char *sort_type) {
    fprintf(html, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(html, "<title>File List - %s</title>\n", sort_type);
    fprintf(html, "<link rel=\"stylesheet\" href=\"style.css\">\n");
    fprintf(html, "</head>\n<body>\n");
    fprintf(html, "<h2>dossier: %s (Triée par %s)</h2>\n", dir, sort_type);
    fprintf(html, "<table>\n");
    fprintf(html, "<tr><th>Name</th><th>Size</th><th>Inode</th><th>Blocks</th></tr>\n");
}

void html_line(FILE *html, About_file *entry) {
    char *size_str = human_readable_size(entry->info.st_size);
    fprintf(html, "<tr><td>%s</td><td>%s</td><td>%ld</td><td>%ld</td></tr>\n",
            entry->file_path, size_str, entry->info.st_ino, entry->info.st_blocks);
    free(size_str);
}

void html_foot(FILE *html) {
    fprintf(html, "</table>\n</body>\n</html>\n");
}

char *human_readable_size(long size) {  
    char *result = malloc(32);
    if (size < 1024) {
        sprintf(result, "%ld B", size);
    } else if (size < 1024 * 1024) {
        sprintf(result, "%.1f KB", size / 1024.0);
    } else if (size < 1024 * 1024 * 1024) {
        sprintf(result, "%.1f MB", size / (1024.0 * 1024.0));
    } else {
        sprintf(result, "%.1f GB", size / (1024.0 * 1024.0 * 1024.0));
    }
    return result;
}