#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <dirent.h> // Pour opendir, readdir, closedir
#include <fcntl.h>  // Pour open
#include <linux/fs.h> // Pour ioctl et les structures associées

int main(int argc, char**argv){
    struct stat st; /* pour récupérer les informations sur un fichier */
    const char* dir_default[] = { "/etc/", "/var/log/", "/usr/local" };
    const char* _dir;
	srand(time(NULL));
    int index = rand() % 3;
    _dir = dir_default[index];

    DIR* dir = opendir(_dir); // open rep
    if (dir == NULL) {
        perror("error acces to rep\n");
        exit(1);
    }

    FILE* html_file = fopen("result.html", "w");
    if (html_file == NULL) {
        perror("error creation fic HTML\n");
        exit(1);
    }
    fprintf(html_file, "<!DOCTYPE html>\n<html>\n<head>\n<title>Résultats</title>\n</head>\n<body>\n");
    fprintf(html_file, "<h1>Contenu du répertoire %s</h1>\n", _dir);
    fprintf(html_file, "<table border=\"1\">\n<tr><th>Nom</th><th>ID</th><th>Taille (octets)</th><th>Position (bloc)</th></tr>\n");

    struct dirent* entrer;
    while ((entrer = readdir(dir)) != NULL) {
        // ignore "." et ".."
        if (strcmp(entrer->d_name, ".") == 0 || strcmp(entrer->d_name, "..") == 0) {
            continue;
        }

        // Construction path complet
        char path[1024];
        snprintf(path, sizeof(path), "%s%s", _dir, entrer->d_name);

        // Recup des info avec stat
        if (stat(path, &st) == 0) {
            int block = 0; // Position phy default
            if (S_ISREG(st.st_mode)) {
                // pos phy sur le disk
                int fd = open(path, O_RDONLY);
                if (fd != -1) {
                    if (ioctl(fd, FIBMAP, &block) != 0) {
                        block = 0; // Si echec (ioctl) , pos = 0
                    }
                    close(fd);
                }
            }
            fprintf(html_file, "<tr>");
            fprintf(html_file, "<td>%s</td>", path);
            fprintf(html_file, "<td>%ld</td>", st.st_ino);
            fprintf(html_file, "<td>%ld</td>", st.st_size);
            fprintf(html_file, "<td>%d</td>", block);
            fprintf(html_file, "</tr>\n");
        } else {
            perror("Erreur avec stat");
        }
    }
    fprintf(html_file, "</table>\n</body>\n</html>\n");
    fclose(html_file);

    closedir(dir);
    printf("resultat => result.html\n");
    return 0;
}
