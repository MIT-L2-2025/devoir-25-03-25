#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_PATH 1024
#define NB_FIC_MAX 1000


int allFic(const char *dir, char data[][MAX_PATH], int max_data) {
    int nbfic = 0;
    DIR *d;
    struct dirent *fic;

    d = opendir(dir);
    if (d == NULL) return 0;

    while ((fic = readdir(d)) != NULL && nbfic < max_data) {
        if ((fic->d_type == DT_REG || fic->d_type == DT_DIR) && 
            strcmp(fic->d_name, ".") != 0 && strcmp(fic->d_name, "..") != 0) {
            snprintf(data[nbfic], MAX_PATH, "%s/%s", dir, fic->d_name);
            nbfic++;
        }
    }
    closedir(d);
    return nbfic;
}

int main() {
    // choix au hasard d un rep
    const char *rep[] = {"/etc", "/usr/local", "/var/log"};
    srand(time(NULL));
    int randRep = rand() % 3; 
    const char *repOK = rep[randRep];


    char data[NB_FIC_MAX][MAX_PATH];
    struct stat st;
    char posCmd[MAX_PATH];
    char position[100];

    // get all fic|rep list  in data and return nbfic in data
    int nbfic = allFic(repOK, data, NB_FIC_MAX);
    if (nbfic == 0) {
        printf("Aucune entrée trouvée dans %s\n", repOK);
        return 1;
    }

    // choix au hasard d un fic au rep
    int ficOK = rand() % nbfic;
    if (stat(data[ficOK], &st) == -1) {
        printf("Erreur lors de l'accès à l'entrée\n");
        return 1;
    }

    // recherch du position du fic dans le disk
    snprintf(posCmd, sizeof(posCmd), "filefrag -v %s | tail -n 2 | head -n 1 | cut -d : -f3",data[ficOK]);
    FILE *cmd = popen(posCmd, "r");
    if (cmd == NULL) {
        perror("Erreur lors du recherche du position");
        return 1;
    }
    fgets(position, sizeof(position), cmd);
    fclose(cmd);

    FILE *html = fopen("FileInfo.html", "a");
    if (html == NULL) {
        printf("Erreur lors de l'ouverture de FileInfo.html\n");
        return 1;
    }

    // si fic FileInfo.html == vide ,ecrire l'en-tête HTML
    fseek(html, 0, SEEK_END);
    if (ftell(html) == 0) {
        fprintf(html, "<!DOCTYPE html>\n<html>\n<head>\n");
        fprintf(html, "<title>File Info </title>\n");
        fprintf(html, "<style>table {border-collapse: collapse;} th, td {border: 1px solid black; padding: 8px;}</style>\n");
        fprintf(html, "</head>\n<body>\n");
        fprintf(html, "<h1>Informations sur les fichiers et dossiers </h1>\n");
        fprintf(html, "<table>\n");
        fprintf(html, "<tr><th>Chemin (nom)</th><th>ID (inode)</th><th>Taille (octets)</th><th>Nb Block</th><th>Position</th></tr>\n");
    }

    fprintf(html, "<tr><td>%s</td><td>%d</td><td>%d</td><td>%d</td><td>%s</td></tr>\n", data[ficOK], st.st_ino, st.st_size,st.st_blocks,position);

    fclose(html);
    printf("Informations enregistrées dans FileInfo.html (depuis %s)\n", repOK);

    return 0;
}