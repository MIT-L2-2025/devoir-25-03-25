/*
    Ceci est un programme qui permet de choisir aleatoirement entre: /etc, /var/log,/usr/local
    puis permet d'obtenir l'id,la taille,la position dans le cylindre
     de chaque fichier (meme dans les sous-répertoires) dans le repetoire choisie
     aléatoirement, le resultat est stocké dans resultat.html et pour lancer le programme
     on besoin du superutilisateur


     Par HERIMANANTSOA Manitriniaina Christian
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#define DISK_PATH "/dev/sda2"  // Définit le chemin vers la partition du disque contenant le système

// Déclaration des fonctions utilisées
void displayHeader(FILE *html);
void displayFooter(FILE *html);
char* generateRandomDirectory();
void traverseDirectory(const char *dirName, int disk, FILE *html);
void computeAndDisplayFileStat(const char *file, int disk, FILE *html);

int main() 
{
    // Récupère un répertoire aléatoire parmi /etc, /usr/local, /var/log
    char* dirName = generateRandomDirectory();

    // Ouvre le disque 
    int disk = open(DISK_PATH, O_RDONLY);
    if (disk < 0) {
        perror("Erreur lors de l'ouverture du disque");
        exit(EXIT_FAILURE);
    }

    // Ouvre le fichier HTML en écriture pour enregistrer le résultat
    FILE *html = fopen("resultat.html", "w");
    if (html == NULL) {
        perror("Erreur ouverture fichier HTML");
        close(disk);
        exit(EXIT_FAILURE);
    }

    // Affiche l'entête HTML avec les styles et le début du tableau
    displayHeader(html);

    // Parcours récursivement le répertoire et écrit les infos de chaque fichier dans le tableau HTML
    traverseDirectory(dirName, disk, html);

    // Affiche le pied de page HTML et ferme la structure HTML
    displayFooter(html);

    // Ferme le fichier HTML et le disque ouvert
    fclose(html);
    close(disk);

    printf("Résultats enregistrés dans resultat.html\n");

    return 0;
}

// Fonction qui retourne un répertoire aléatoire parmi trois propositions
char* generateRandomDirectory()
{
    char* directs[] = {"/etc", "/usr/local", "/var/log"};
    srand(time(NULL)); // Initialisation de la graine pour le hasard
    return directs[rand() % 3];
}

// Fonction qui parcourt récursivement un répertoire et ses sous-répertoires
void traverseDirectory(const char *dirName, int disk, FILE *html) 
{
    DIR *dir = opendir(dirName);
    if (dir == NULL) {
        perror("Erreur ouverture répertoire");
        return;
    }
    
    struct dirent *entry;
    // Boucle sur chaque entrée du répertoire
    while ((entry = readdir(dir)) != NULL) 
    {
        // Ignorer les répertoires spéciaux "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Construction du chemin complet du fichier ou du sous-répertoire
        char filePath[1000];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirName, entry->d_name);

        struct stat info;
        // Récupération des informations du fichier/dossier
        if (stat(filePath, &info) == -1) {
            fprintf(stderr, "Erreur stat %s\n", filePath);
            continue;
        }

        // Si l'entrée est un répertoire, appel récursif de la fonction
        if (S_ISDIR(info.st_mode)) 
        {
            traverseDirectory(filePath, disk, html);
        } 
        else 
        {
            // Sinon, c'est un fichier, on calcule et affiche ses statistiques
            computeAndDisplayFileStat(filePath, disk, html);
        }
    }

    closedir(dir); // Fermeture du répertoire
}

// Fonction qui récupère et affiche les informations d'un fichier dans le tableau HTML
void computeAndDisplayFileStat(const char *file, int disk, FILE *html) 
{
    // Ouvre le fichier en lecture
    int fdis = open(file, O_RDONLY);
    if (fdis < 0) {
        perror("Erreur ouverture fichier");
        return;
    }

    struct stat info;
    // Récupère les informations du fichier (taille, inode, etc.)
    if (stat(file, &info) == -1) {
        perror("Erreur stat");
        close(fdis);
        return;
    }

    // Vérification que le fichier est bien sur le disque spécifié
    struct stat disk_stat;
    if (fstat(disk, &disk_stat) == -1) 
    {
        perror("Erreur stat disque");
        close(fdis);
        return;
    }
    if (info.st_dev != disk_stat.st_rdev) 
    {
        // Si le fichier n'est pas sur le disque /dev/sda2, on ne l'affiche pas
        close(fdis);
        return;
    }

    // Récupération du numéro de bloc logique du fichier à l'aide de FIBMAP
    int block;
    if (ioctl(fdis, FIBMAP, &block) < 0) {
        perror("Erreur FIBMAP");
        close(fdis);
        return;
    }

    // Récupération de la géométrie du disque (nombre de têtes, secteurs, etc.)
    struct hd_geometry geo_disk;
    if (ioctl(disk, HDIO_GETGEO, &geo_disk) < 0) 
    {
        perror("Erreur HDIO_GETGEO");
        close(fdis);
        return;
    }

    // Calcul du cylindre à partir du numéro de bloc et de la géométrie du disque
    long long position_cylindre = block / (geo_disk.heads * geo_disk.sectors);

    // Écriture d'une ligne du tableau HTML avec les informations du fichier
    fprintf(html,
            "<tr>"
            "<td>%s</td>"
            "<td>%lu</td>"
            "<td>%lld octets</td>"
            "<td>%lld</td>"
            "</tr>\n",
            file, info.st_ino, (long long)info.st_size, position_cylindre);

    close(fdis); // Fermeture du fichier
}

// Fonction qui affiche l'entête du fichier HTML
void displayHeader(FILE *html) 
{
    fprintf(html,
            "<!DOCTYPE html>\n"
            "<html lang='fr'>\n"
            "<head>\n"
            "<meta charset='UTF-8'>\n"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
            "<title>Statistiques des fichiers</title>\n"
            "<style>\n"
            "body { font-family: 'Times New Roman', serif; background: linear-gradient(to right, #1e3c72, #2a5298); color: white; text-align: center; }\n"
            "h1 { margin-top: 20px; }\n"
            "table { width: 80%%; margin: 20px auto; border-collapse: collapse; background: white; color: black; border-radius: 10px; overflow: hidden; }\n"
            "th, td { padding: 12px; border: 1px solid black; }\n"
            "th { background: #2a5298; color: white; }\n"
            "tr:nth-child(even) { background: #f2f2f2; }\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
            "<h1>Statistiques des fichiers</h1>\n"
            "<table>\n"
            "<tr>\n"
            "<th>Fichier</th>\n"
            "<th>Inode</th>\n"
            "<th>Taille</th>\n"
            "<th>Cylindre</th>\n"
            "</tr>\n");
}

// Fonction qui affiche le pied de page du fichier HTML 
void displayFooter(FILE *html) 
{
    fprintf(html,
            "</table>\n"
            "<p>Analyse terminée - %s</p>\n"
            "</body>\n"
            "</html>\n", __DATE__);
}
