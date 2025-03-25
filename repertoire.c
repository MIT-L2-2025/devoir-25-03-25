#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> //je l'utulise pour manipuler les repertoires dans le système de fichier
#include <sys/stat.h>   //info sur les fichiers et repertoires(taille,type,..)
#include <time.h>
#include <unistd.h> //accessibilité aux fonctionnalités du système liées aux fichiers

int main() {
    // Tableau des répertoires possibles
    const char *directories[] = {"/etc", "/usr/local", "/var/log"};
    int num_reps = 3;
    
    // Sélection aléatoire d'un répertoire
    srand(time(NULL));
    int random_index = rand() % num_reps;
    const char *selected_dir = directories[random_index];
    
    // Ouverture du répertoire
    DIR *rep = opendir(selected_dir);
    if (rep == NULL) {
        printf("Erreur: Impossible d'ouvrir le répertoire %s\n", selected_dir);
        return 1;
    }

    // Création du fichier HTML
    FILE *html_file = fopen("affichage.html", "w");
    if (html_file == NULL) {
        printf("Erreur: Impossible de créer le fichier HTML\n");
        closedir(rep);
        return 1;
    }

    // Écriture de l'en-tête HTML
    fprintf(html_file, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(html_file, "<title>Résultat - %s</title>\n", selected_dir);
    fprintf(html_file, "<style>table {border-collapse: collapse;} th, td {border: 1px solid black; padding: 8px;}</style>\n");
    fprintf(html_file, "</head>\n<body>\n");
    fprintf(html_file, "<h1>Contenu de %s</h1>\n", selected_dir);
    fprintf(html_file, "<table>\n");
    fprintf(html_file, "<tr><th>ID (inode)</th><th>Taille (octets)</th><th>Position (blocs)</th><th>Nom du fichier</th></tr>\n");

    // Parcours du répertoire
    struct dirent *entry;
    struct stat file_stat;
    char filepath[1024];

    while ((entry = readdir(rep)) != NULL) {
        // Construction du chemin complet
        snprintf(filepath, sizeof(filepath), "%s/%s", selected_dir, entry->d_name);
        
        // Récupération des informations du fichier
        if (stat(filepath, &file_stat) == -1) {
            continue; // Passe au fichier suivant en cas d'erreur
        }

        // Écriture d'une ligne dans le tableau HTML
        fprintf(html_file, "<tr>");
        fprintf(html_file, "<td>%lu</td>", (unsigned long)file_stat.st_ino);  // ID (inode)
        fprintf(html_file, "<td>%ld</td>", (long)file_stat.st_size);          // Taille en octets
        fprintf(html_file, "<td>%ld</td>", (long)file_stat.st_blocks);        // Nombre de blocs
        fprintf(html_file, "<td>%s</td>", entry->d_name);                     // Nom du fichier
        fprintf(html_file, "</tr>\n");
    }

    // Fermeture du tableau et du HTML
    fprintf(html_file, "</table>\n");
    fprintf(html_file, "</body>\n</html>");

    // Fermeture des fichiers
    fclose(html_file);
    closedir(rep);

    printf("Le fichier resultat.html a été généré avec succès.\n");
    printf("Répertoire analysé : %s\n", selected_dir);
    printf("Ouvrez resultat.html dans un navigateur pour voir les résultats.\n");

    return 0;
}