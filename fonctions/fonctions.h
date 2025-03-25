#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int getFileSize(const char *filepath, int *file_size) {
    FILE *fp = NULL;
    char buffer[1024];

    fp = fopen(filepath, "r");
    if (fp == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strstr(buffer, "File size of") != NULL) {
            if (sscanf(buffer, "File size of %*s is %d", file_size) == 1) {
                fclose(fp);
                return 0;
            }
        }
    }

    fclose(fp);
    return -1;
}


int getFileLBAAndCHR(const char *filepath, unsigned long *lba, unsigned long *chr) {
    FILE *fp = NULL;
    char buffer[1024];

    fp = fopen(filepath, "r");
    if (fp == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strstr(buffer, "ext:") != NULL) {
            if (sscanf(buffer, "   0:        %*ld..       %lu:    %lu..   %*ld:      %*d: %*s", lba, chr) == 2) {
                fclose(fp);
                return 0;
            }
        }
    }

    fclose(fp);
    return -1;
}

void generateRandomFile(char *random_dir) {
    const char *dirs[] = {"/etc", "/usr/local", "/var/log"};
    srand(time(NULL));

    int random_index = rand() % 3;
    snprintf(random_dir, 1024, "%s/main.c", dirs[random_index]);

    FILE *f = fopen(random_dir, "w");
    if (f == NULL) {
        perror("Erreur lors de la création du fichier");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "Fichier généré dans %s pour test.\n", dirs[random_index]);
    fclose(f);
}

void runFilefrag(const char *filepath) {
    char command[1024];
    snprintf(command, sizeof(command), "filefrag -v %s | tail -n +2 | head -n -1 > info.txt", filepath);
    int ret = system(command);
    if (ret == -1) {
        perror("Erreur lors de l'exécution de la commande");
        exit(EXIT_FAILURE);
    }
}

#endif