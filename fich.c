#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

void write_html_header(FILE *f, const char *directory)
{
    fprintf(f, "<html>\n");
    fprintf(f, "<head> <meta charset=\"utf-8\"><title>Exploration de %s</title></head>\n", directory);
    fprintf(f, "<body><h1>Contenu de %s</h1>\n", directory);
    fprintf(f, "<table style='border: 2px solid blue'>\n");
    fprintf(f, "<tr><th>Nom Fichier</th><th>ID Fichier</th><th>Taille</th><th>Zone Disque</th><th>Type</th></tr>\n");
}

void write_html_footer(FILE *f)
{
    fprintf(f, "</table>\n</body>\n</html>\n");
}

int main()
{
    const char *dirs[3] = {"/etc", "/usr/local", "/var/log"};
    srand((unsigned)time(NULL));
    const char *chosen_dir = dirs[rand() % 3]; // Choix aléatoire du repertoire

    DIR *d = opendir(chosen_dir);
    if (!d)
    {
        printf("Erreur: répertoire %s inaccessible\n", chosen_dir);
        return 1;
    }

    FILE *out = fopen("liste.html", "w");
    if (!out)
    {
        printf("Erreur: impossible de créer liste.html\n");
        closedir(d);
        return 1;
    }

    write_html_header(out, chosen_dir);

    struct dirent *entry;
    char path[512];   // Buffer pour le chemin complet du fichier
    struct stat info; // Structure pour les métadonnées du fichier

    while ((entry = readdir(d)))
    {
        // Ignorer le "." et ".." pour eviter boucles infinies ou doublons
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        sprintf(path, "%s/%s", chosen_dir, entry->d_name); // Construire chemin complet:repertoire+nom de l'entréé
        if (stat(path, &info) != 0)                        // Passer a l'entrée suivante en cas d'erreur
            continue;

        long disk_zone = info.st_blocks * info.st_blksize / 1000; // Approximation du disque En Ko
        // Determination du type
        const char *type = S_ISREG(info.st_mode) ? "Fichier" : S_ISDIR(info.st_mode) ? "Répertoire"
                                                                                     : "Autre";

        fprintf(out, "<tr>\n");
        fprintf(out, "<td>%s</td>\n", entry->d_name);
        fprintf(out, "<td>%lu</td>\n", (unsigned long)info.st_ino);
        fprintf(out, "<td>%ld octets</td>\n", (long)info.st_size);
        fprintf(out, "<td>%ld Ko</td>\n", disk_zone);
        fprintf(out, "<td>%s</td>\n", type);
        fprintf(out, "</tr>\n");
    }

    write_html_footer(out);

    fclose(out);
    closedir(d);

    printf("Fichier liste.html créé avec succès\n");
    printf("Informations sur: %s dans liste.html\n", chosen_dir);
    return 0;
}