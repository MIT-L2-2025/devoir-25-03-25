#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define rep1 "/etc/"
#define rep2 "/var/log/"
#define rep3 "/usr/local/"

int main() {
    DIR *dir;
    struct dirent *ent;
    struct stat file_stat;

    char *repertoires[] = {rep1, rep2, rep3};
    int total_reps = sizeof(repertoires) / sizeof(repertoires[0]);

    srand(time(NULL));
    int index = rand() % total_reps;
    char *repertoire_choisi = repertoires[index];

    dir = opendir(repertoire_choisi);
    if (dir == NULL) {
        fprintf(stderr, "Erreur d'ouverture\n");
        exit(1);
    }

    FILE *fic = fopen("resultat.html", "w");
    if (fic == NULL) {
        fprintf(stderr, "Erreur d'ouverture\n");
        closedir(dir);
        exit(1);
    }

    fprintf(fic, "<html lang=\"en\">\n");
    fprintf(fic, "<head>\n");
    fprintf(fic, "   <meta charset=\"UTF-8\">\n");
    fprintf(fic, "   <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(fic, "   <title>Liste des fichiers</title>\n");
    fprintf(fic, "   <style>\n");
    fprintf(fic, "       table { width: 100%%; border-collapse: collapse; }\n");
    fprintf(fic, "       th, td { border: 1px solid black; padding: 8px; text-align: left; }\n");
    fprintf(fic, "   </style>\n");
    fprintf(fic, "</head>\n");
    fprintf(fic, "<body>\n");
    fprintf(fic, "<h2>Liste des fichiers dans le répertoire %s</h2>\n", repertoire_choisi);
    
    fprintf(fic, "<table>\n");
    fprintf(fic, "   <tr>\n");
    fprintf(fic, "       <th>Nom</th>\n");
    fprintf(fic, "       <th>Taille</th>\n");
    fprintf(fic, "       <th>ID </th>\n");
    fprintf(fic, "       <th>Position</th>\n");
    fprintf(fic, "   </tr>\n");

    while ((ent = readdir(dir)) != NULL) {
        char chemin[1024];
        snprintf(chemin, sizeof(chemin), "%s%s", repertoire_choisi, ent->d_name);

        if (stat(chemin, &file_stat) == -1) {
            printf("%s (impossible à récupérer)\n", ent->d_name);
            continue;
        }

        char position[1024];
        snprintf(position, sizeof(position), "filefrag -v %s | tail -2 | head -1 | cut -d ':' -f 3", chemin);

        char result_position[1000] = " ";
        FILE *fp = popen(position, "r");

        if (fp == NULL) {
            printf("Erreur lors de l'exécution de filefrag pour %s\n", ent->d_name);
        }

        else {
            if (fgets(result_position, sizeof(result_position), fp) != NULL) {
                result_position[strcspn(result_position, "\n")] = 0;
            }
            pclose(fp);
        }

        fprintf(fic, "   <tr>\n");
        fprintf(fic, "       <td>%s</td>\n", ent->d_name);
        fprintf(fic, "       <td>%ld</td>\n", (long)file_stat.st_size);
        fprintf(fic, "       <td>%ld</td>\n", (long)file_stat.st_ino);
        fprintf(fic, "       <td>%s</td>\n", result_position);
        fprintf(fic, "   </tr>\n");

    }

    fprintf(fic, "</table>\n");
    fprintf(fic, "</body>\n");
    fprintf(fic, "</html>\n");

    fclose(fic);
    closedir(dir);

    return 0;
}

