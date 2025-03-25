#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h> // Pour opendir, readdir, closedir
#include <string.h> // Pour strcat
#include <fcntl.h>  // Pour open
#include <linux/fs.h> // Pour ioctl et les structures associées

int main(int argc, char**argv){
    struct stat st; /* pour récupérer les informations sur un fichier */
    const char* directories[] = { "/etc/", "/var/log/", "/usr/" };
    srand(time(NULL));
    int index = rand() % 3;
    const char* selected_dir = directories[index];

    DIR* dir = opendir(selected_dir); // Ouvrir le répertoire
    if (dir == NULL) {
        perror("Erreur d'accès au répertoire\n");
        exit(1);
    }

    // Ouvrir un fichier HTML pour écrire les résultats
    FILE* html_file = fopen("result.html", "w");
    if (html_file == NULL) {
        perror("Erreur lors de la création du fichier HTML\n");
        exit(1);
    }

    // Écrire l'en-tête HTML
    fprintf(html_file, "<!DOCTYPE html>\n<html>\n<head>\n<title>Résultats</title>\n</head>\n<body>\n");
    fprintf(html_file, "<h1>Contenu du répertoire %s</h1>\n", selected_dir);
    fprintf(html_file, "<table border=\"1\">\n<tr><th>Nom</th><th>ID</th><th>Taille (octets)</th><th>Position (bloc)</th></tr>\n");

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Ignorer les entrées spéciales "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construire le chemin complet
        char path[1024];
        snprintf(path, sizeof(path), "%s%s", selected_dir, entry->d_name);

        // Récupérer les informations avec stat
        if (stat(path, &st) == 0) {
            unsigned long block = 0; // Position physique par défaut
            if (S_ISREG(st.st_mode)) {
                // Obtenir la position physique sur le disque pour les fichiers réguliers
                int fd = open(path, O_RDONLY);
                if (fd != -1) {
                    if (ioctl(fd, FIBMAP, &block) != 0) {
                        block = 0; // Si ioctl échoue, laisser la position à 0
                    }
                    close(fd);
                }
            }

            // Écrire les informations dans le tableau HTML
            fprintf(html_file, "<tr>");
            fprintf(html_file, "<td>%s</td>", path);
            fprintf(html_file, "<td>%ld</td>", st.st_ino);
            fprintf(html_file, "<td>%ld</td>", st.st_size);
            fprintf(html_file, "<td>%lu</td>", block);
            fprintf(html_file, "</tr>\n");
        } else {
            perror("Erreur avec stat");
        }
    }

    // Fermer le tableau et le fichier HTML
    fprintf(html_file, "</table>\n</body>\n</html>\n");
    fclose(html_file);

    closedir(dir); // Fermer le répertoire
    printf("Les résultats ont été écrits dans le fichier result.html\n");
    return 0;
}
