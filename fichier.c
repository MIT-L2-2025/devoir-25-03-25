#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

int main() {
    const char *repertoire[] = {"/etc", "/var/log", "/usr/local"}; // Repertoire à choisir aléatoirement
    srand(time(NULL)); // Permet d'e'viter d'avoir le meme repertoire pour le tirage
    int choix = rand() % 3;
    const char *dir = repertoire[choix]; // Repertoire choisi

    FILE *html = fopen("resultat.html", "w"); // Fichier HTML du resultat
    if (!html) {
        fprintf(stderr, "Erreur création fichier HTML\n");
        return 1;
    }
/* Entete du fichier HTML*/
    fprintf(html, "<!DOCTYPE html>\n");
    fprintf(html, "<html><head><meta charset='UTF-8'>\n");
    fprintf(html, "<title>Fichiers dans %s</title></head>\n", dir);
    fprintf(html, "<body>\n");
    fprintf(html, "<h2>Fichiers réguliers dans %s</h2>\n", dir);
    fprintf(html, "<table border='1'>\n");
    fprintf(html, "<tr><th>Nom</th><th>ID Inode</th><th>Taille</th><th>Blocs</th></tr>\n");

    DIR *dossier = opendir(dir); // Ouverture du repertoire choisi
    if (!dossier) {
        fprintf(html, "</table><p>Erreur d'accès au répertoire!</p></body></html>");
        fclose(html);
        return 1;
    }

    struct dirent *entree;
    char chemin[1024];
    struct stat st;

    while ((entree = readdir(dossier)) != NULL) { 
        if (entree->d_name[0] == '.'){
			 continue;
		}

        snprintf(chemin, sizeof(chemin), "%s/%s", dir, entree->d_name); /*Elaboration du chemin complet du fichier */
        if (lstat(chemin, &st) == -1){
			 continue;
		 }

        if (!S_ISREG(st.st_mode)) { // Ignorer les fichier qui ne sont pas de type fichier original
			continue;
		}
        fprintf(html, "<tr>");
        fprintf(html, "<td>%s</td>", entree->d_name);
        fprintf(html, "<td>%lu</td>", st.st_ino);
        fprintf(html, "<td>%ld octets</td>", st.st_size);
        fprintf(html, "<td>%ld</td>", st.st_blocks);
        fprintf(html, "</tr>\n");
    }

    closedir(dossier);
    fprintf(html, "</table>\n</body>\n</html>");
    fclose(html);

    printf("Fichier resultat.html généré avec succès!\n");
    return 0;
}
