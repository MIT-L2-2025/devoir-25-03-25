#include "blocfichier.h"


int main() {
    srand(time(NULL));

    const char *dir = get_random_directory();
    printf("Répertoire sélectionné : %s\n", dir);

    DIR *d = opendir(dir);
    if (d == NULL) {
        perror("Erreur d'ouverture du répertoire");
        exit(1);
    }

    struct dirent *entry;
    struct stat file_stat;
    FileInfo files[1024];
    int file_count = 0;

    while ((entry = readdir(d)) != NULL) {

        if (entry->d_type == DT_DIR) {      // Tsy affichena ny dossier fa ny fichier ihany no atao
            continue;
        }
        
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)    // Tsy affichena ny "." sy ".."
            continue;

        char file_path[1024];
        snprintf(file_path, sizeof(file_path), "%s/%s", dir, entry->d_name);

        if (stat(file_path, &file_stat) == 0) {
            files[file_count].inode = file_stat.st_ino;
            files[file_count].size = file_stat.st_size;
            strncpy(files[file_count].path, file_path, sizeof(files[file_count].path) - 1);
            files[file_count].position = file_stat.st_ino % 1024;           // Pas précis
            file_count++;
        }
    }

    closedir(d);

    generate_html_table(files, file_count, dir);

    printf("Le fichier HTML 'fichiers_info.html' a été généré.\n");

    return 0;
}
