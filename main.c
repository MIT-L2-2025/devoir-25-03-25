#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include "hard_drive.h"

int main() {
    char *rep;
    FILE *fichier;
    geoDisque geo;
    int res;
    srand(time(NULL));
    choisirDir(&rep);
    res = obtenirGeo(&geo);
    if(res == -1) {
        fprintf(stderr, "Erreur de géométrie\n");
        return 1;
    }
    fichier = fopen("output.html", "w");
    if(!fichier) {
        perror("Erreur");
        return 1;
    }
    ecrireEnTete(fichier, rep);
    traiterFichiers(rep, fichier, geo);
    ecrirePied(fichier);
    fclose(fichier);
    printf("Sortie dans output.html\n");
    return 0;
}

