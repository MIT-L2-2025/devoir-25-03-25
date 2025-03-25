#ifndef HARD_DRIVE
#define HARD_DRIVE

#define BS 512
#define MAX_CHEMIN 1024

typedef struct {
    int cylindre;
    int tete;
    int secteur;
} geoDisque;

int obtenirGeo(geoDisque *geo);
void convertirChs(unsigned long lba, geoDisque geo, int *c, int *t, int *s);
void choisirDir(char **dir);
void ecrireEnTete(FILE *f, char *rep);
void ecrirePied(FILE *f);
void traiterFichiers(const char *rep, FILE *f, geoDisque geo);

#endif
