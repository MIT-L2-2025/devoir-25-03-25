/*#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/fs.h>

int main() {
    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));
    const char *repertoires[] = {"/etc","/usr/local", "/var/log"};
    const char *repertoire = repertoires[rand() % 2]; // Choisir aléatoirement

    // Ouvre le répertoire
    DIR *dir = opendir(repertoire);
    if (!dir) 
    {
        perror("Erreur lors de l'ouverture du répertoire");
        return 1;
    }

    // Ouvre le fichier HTML pour écrire le tableau
    FILE *html = fopen("resultat.html", "w");
    if (!html) 
    {
        perror("Erreur lors de la création du fichier HTML");
        closedir(dir);
        return 1;
    }

    // Écriture de l'en-tête HTML
    fprintf(html, "<!DOCTYPE html>\n<html>\n<head>\n<title>Résultats des fichiers</title>\n");
    fprintf(html, "<style>table {border-collapse: collapse; width: 80%%; margin: 20px auto;}\n");
    fprintf(html, "th, td {border: 1px solid black; padding: 8px; text-align: left;}\n");
    fprintf(html, "th {background-color: #f2f2f2;}</style>\n</head>\n<body>\n");
    fprintf(html, "<h2>Fichiers dans %s</h2>\n", repertoire);
    fprintf(html, "<table>\n<tr><th>Nom du fichier</th><th>ID (inode)</th><th>Taille (octets)</th><th>Bloc disque</th></tr>\n");

    struct dirent *entree;
    struct stat info;
    char chemin[1024];

    // Parcourir le répertoire
    while ((entree = readdir(dir)) != NULL) 
    {
        // Ignorer . et ..
        if (strcmp(entree->d_name, ".") == 0 || strcmp(entree->d_name, "..") == 0) {
            continue;
        }

        // Construire le chemin complet
        snprintf(chemin, sizeof(chemin), "%s/%s", repertoire, entree->d_name);

        // Récupérer les informations du fichier
        if (stat(chemin, &info) == -1) 
        {
            perror("Erreur stat");
            continue;
        }

        // Récupérer le numéro de bloc physique (approximation de la position)
        int fd = open(chemin, O_RDONLY);
        long bloc = -1;
        if (fd != -1) {
            if (ioctl(fd, FIBMAP, &bloc) == -1) {
                bloc = -1; // Erreur ou non supporté
            }
            close(fd);
        }

        // Écrire une ligne dans le tableau HTML
        fprintf(html, "<tr><td>%s</td><td>%ld</td><td>%ld</td><td>%ld</td></tr>\n",
                entree->d_name, (long)info.st_ino, (long)info.st_size, bloc);
    }

    // Fin du tableau et du HTML
    fprintf(html, "</table>\n</body>\n</html>\n");

    // Fermer les ressources
    closedir(dir);
    fclose(html);

    printf("Résultats écrits dans resultat.html\n");
    return 0;
}*/


#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#define MAX_PATH 1024

// Liste des répertoires possibles
const char *dirs[] = {"/etc", "/usr/local", "/var/log"};

int main() 
{
    srand(time(NULL)); // Initialiser l'aléatoire
    const char *rep = dirs[rand() % (sizeof(dirs) / sizeof(dirs[0]))]; // Choisir un répertoire au hasard

    printf("Exploration du répertoire : %s\n", rep);

    DIR *dir = opendir(rep);
    if (!dir) {
        perror("Erreur ouverture répertoire");
        return 1;
    }

    FILE *output = fopen("table.html", "w");
    if (!output) {
        perror("Erreur ouverture fichier HTML");
        closedir(dir);
        return 1;
    }

    // Écrire l'entête du fichier HTML
    fprintf(output, "<html><body><table border='1'>\n");
    fprintf(output, "<tr><th>Nom du fichier</th><th>ID</th><th>Taille (octets)</th><th>Position sur le disque</th></tr>\n");

    struct dirent *entry;
    char filepath[MAX_PATH];

    /*while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) continue; // Ignorer les dossiers et liens symboliques*/

        snprintf(filepath, MAX_PATH, "%s/%s", rep, entry->d_name);
        struct stat file_stat;

        if (stat(filepath, &file_stat) == 0) {
            fprintf(output, "<tr><td>%s</td><td>%lu</td><td>%ld</td><td>%ld</td></tr>\n",
                    entry->d_name, file_stat.st_ino, file_stat.st_size, file_stat.st_blocks);
        }
    

    // Fermer les fichiers et libérer la mémoire
    fprintf(output, "</table></body></html>\n");
    fclose(output);
    closedir(dir);

    printf("Le fichier HTML a été généré : table.html\n");

    return 0;
}