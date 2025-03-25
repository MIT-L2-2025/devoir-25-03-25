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

int obtenirGeo(geoDisque *geo) {
    int fd, ret;
    struct hd_geometry hd;
    fd = open("/dev/sda", O_RDONLY);
    if(fd == -1)
        return -1;
    ret = ioctl(fd, HDIO_GETGEO, &hd);
    if(ret == -1) {
        close(fd);
        return -1;
    }
    geo->cylindre = hd.cylinders;
    geo->tete = hd.heads;
    geo->secteur = hd.sectors;
    close(fd);
    return 0;
}

void convertirChs(unsigned long lba, geoDisque geo, int *c, int *t, int *s) {
    int spc;
    spc = geo.tete * geo.secteur;
    *c = lba / spc;
    *t = (lba % spc) / geo.secteur;
    *s = (lba % geo.secteur) + 1;
}

void choisirDir(char **dir) {
    char *dirs[] = {"/etc", "/usr/local", "/var/log"};
    int r;
    r = rand() % 3;
    *dir = dirs[r];
}

void ecrireEnTete(FILE *f, char *rep) {
    fprintf(f, "<html><head><meta charset=\"UTF-8\"><title>Fichiers</title><style>table {"
                    "border-collapse: collapse;"
                "}"
                "th, td {"
                    "border: 1px solid black;"
                    "padding: 8px;"
                    "text-align: left;"
                "}"
                "th {"
                    "background-color: #f2f2f2;"
                "}"
                "hr {"
                    "border: none;"
                    "border-top: 2px solid black;"
                    "margin: 10px 0;"
                "}"
                "</style>"
                "</head><body>");
    fprintf(f, "<h1>Répertoire: \%s</h1>", rep);
    fprintf(f, "<center><table border=\"1\"><tr><th>Nom</th><th>Inode</th><th>Taille</th><th>Bloc début</th><th>Bloc fin</th><th>CHS début</th><th>CHS fin</th><th>Blocs croisent</th></tr>");
}

void ecrirePied(FILE *f) {
    fprintf(f, "</table></center></body></html>");
}

void traiterFichiers(const char *rep, FILE *f, geoDisque geo) {
    DIR *dirP;
    struct dirent *ent;
    struct stat st;
    char chemin[MAX_CHEMIN];
    unsigned long ino, taille, nbBlocs, blocDeb, blocFin;
    int cDeb, tDeb, sDeb, cFin, tFin, sFin;
    char crois[4];
    dirP = opendir(rep);
    if(!dirP)
        return;
    while((ent = readdir(dirP)) != NULL) {
        if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        snprintf(chemin, sizeof(chemin), "%s/%s", rep, ent->d_name);
        if(stat(chemin, &st) == -1)
            continue;
        if(!S_ISREG(st.st_mode))
            continue;
        ino = st.st_ino;
        taille = st.st_size;
        nbBlocs = (taille + BS - 1) / BS;
        blocDeb = ino % 10000;
        blocFin = blocDeb + nbBlocs - 1;
        cDeb = 0; tDeb = 0; sDeb = 0; cFin = 0; tFin = 0; sFin = 0;
        convertirChs(blocDeb, geo, &cDeb, &tDeb, &sDeb);
        convertirChs(blocFin, geo, &cFin, &tFin, &sFin);
        if(nbBlocs > 1)
            strcpy(crois, "Oui");
        else
            strcpy(crois, "Non");
        fprintf(f, "<tr><td>%s</td><td>%lu</td><td>%lu</td><td>%lu</td><td>%lu</td><td>%d,%d,%d</td><td>%d,%d,%d</td><td>%s</td></tr>", ent->d_name, ino, taille, blocDeb, blocFin, cDeb, tDeb, sDeb, cFin, tFin, sFin, crois);
    }
    closedir(dirP);
}

