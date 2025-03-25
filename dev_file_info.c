#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

// Structure pour stocker les informations
typedef struct {
    char *path;
    long inode;
    long size;
    long blocks;
    long block_size;
    char *timestamp;
} FileInfo;

// Obtenir la date/heure actuelle
char* get_current_time() {
    time_t raw_time;
    struct tm *time_info;
    char *buffer = malloc(80);
    if (!buffer) return NULL;

    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", time_info);
    return buffer;
}

// Choisir un fichier aléatoirement dans un répertoire
char* choose_random_file_in_dir(const char *dirs[], int num_dirs) //retourne un pointeur qui pointe sur le fichier source
{
    srand(time(NULL));
    const char *selected_dir = dirs[rand() % num_dirs];
    DIR *dir = opendir(selected_dir);
    if (!dir) {
        perror("Erreur ouverture répertoire");
        return NULL;
    }

    struct dirent *entry;
    int file_count = 0;
    char *files[1024];
    while ((entry = readdir(dir)) != NULL && file_count < 1024) {
        if (entry->d_type == DT_REG) {
            files[file_count] = strdup(entry->d_name);
            file_count++;
        }
    }
    closedir(dir);

    if (file_count == 0) {
        fprintf(stderr, "Aucun fichier trouvé dans %s\n", selected_dir);
        return NULL;
    }

    int choice = rand() % file_count;
    char *full_path = malloc(strlen(selected_dir) + strlen(files[choice]) + 2);
    sprintf(full_path, "%s/%s", selected_dir, files[choice]);

    for (int i = 0; i < file_count; i++) free(files[i]);
    return full_path;
}

// Récupérer les métadonnées du fichier
int get_file_info(const char *path, FileInfo *info) {
    struct stat file_stat;
    if (stat(path, &file_stat) == -1) {
        perror("Erreur stat");
        return -1;
    }

    info->path = strdup(path);
    info->inode = (long)file_stat.st_ino;
    info->size = (long)file_stat.st_size;
    info->blocks = (long)file_stat.st_blocks;
    info->block_size = (long)file_stat.st_blksize;
    info->timestamp = get_current_time();
    return info->timestamp ? 0 : -1;
}

// Écrire dans un tableau HTML (simplifié)
void write_to_html(const FileInfo *info) {
    FILE *html_file = fopen("disp.html", "a");
    if (!html_file) {
        perror("Erreur ouverture disp.html");
        return;
    }

    if (ftell(html_file) == 0) {
        fprintf(html_file, "<!DOCTYPE html>\n<html>\n<head>\n"
                "<meta charset=\"UTF-8\">\n<title>Fichiers</title>\n"
                "<style>table { border-collapse: collapse; } td, th { border: 1px solid #333; padding: 5px; }</style>\n"
                "</head>\n<body>\n<h1>Fichiers</h1>\n"
                "<table>\n<tr><th>Date/Heure</th><th>Taille (octets)</th><th>Inode</th><th>Fichier</th></tr>\n");
    }

    fprintf(html_file, "<tr><td>%s</td><td>%ld</td><td>%ld</td><td>%s</td></tr>\n",
            info->timestamp, info->size, info->inode, info->path);

    fclose(html_file);
    printf("Résultats ajoutés à disp.html\n");
}

// Afficher à l'écran (complet)
void print_file_info(const FileInfo *info) {
    printf("Date et heure : %s\nFichier : %s\nId : %ld\nTaille : %ld octets\n"
           "Position cylindre non disponible sur un disque SSD\n\n",
           info->timestamp, info->path, info->inode, info->size);
}

// Libérer la mémoire
void free_file_info(FileInfo *info) {
    free(info->path);
    free(info->timestamp);
}

int main() {
    const char *dirs[] = {"/etc", "/usr/local", "/var/log"};//tableau contenant les nom des fichier source
    int num_dirs = 3;//nombre

    FileInfo info = {0};//struct 
    char *selected_file = choose_random_file_in_dir(dirs, num_dirs);
    if (!selected_file) return 1;

    if (get_file_info(selected_file, &info) == -1) {
        free(selected_file);
        free_file_info(&info);
        return 1;
    }

    print_file_info(&info);
    write_to_html(&info);

    free(selected_file);
    free_file_info(&info);
    return 0;
}