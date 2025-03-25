#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <mntent.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_FILES 100
#define MAX_PATH_LEN 512  // Longueur maximale du chemin de fichier

// Structure pour stocker les informations sur un fichier
typedef struct {
    char name[MAX_PATH_LEN];
    off_t size;
    int id;  // ID unique pour chaque fichier
} FileInfo;

// Fonction pour obtenir la taille et d'autres informations sur un fichier
void get_file_info(const char *filepath, FileInfo *file_info) {
    struct stat file_stat;
    if (stat(filepath, &file_stat) == 0) {
        file_info->size = file_stat.st_size;
    } else {
        perror("stat");
    }
}

// Fonction pour lister les fichiers dans un répertoire
int list_files(const char *dir_path, FileInfo files[]) {
    DIR *dir = opendir(dir_path);
    struct dirent *entry;
    int count = 0;

    if (dir == NULL) {
        if (errno == EACCES) {
            // Ignorer les répertoires sans permission
            return count;
        }
        perror("opendir");
        return 0;
    }

    while ((entry = readdir(dir)) != NULL && count < MAX_FILES) {
        // Ignorer les répertoires spéciaux (".", "..")
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // Construire le chemin complet du fichier
            char filepath[MAX_PATH_LEN];
            snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);

            struct stat file_stat;
            if (stat(filepath, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
                // Vérifier si c'est un fichier régulier
                if (strlen(filepath) < MAX_PATH_LEN) {  // Contrôler la taille du chemin
                    snprintf(files[count].name, sizeof(files[count].name), "%s", filepath);
                    files[count].id = count;  // ID basé sur l'ordre d'apparition
                    get_file_info(filepath, &files[count]);
                    count++;
                }
            }
        }
    }

    closedir(dir);
    return count;
}

// Fonction pour parcourir tous les systèmes de fichiers montés et lister les fichiers
int list_files_in_mounts(FileInfo files[]) {
    FILE *fp = setmntent("/etc/mtab", "r");
    if (fp == NULL) {
        perror("setmntent");
        return 0;
    }

    struct mntent *mnt;
    int file_count = 0;

    while ((mnt = getmntent(fp)) != NULL) {
        // Ignorer les systèmes de fichiers spéciaux comme "proc" ou "tmpfs"
        if (strcmp(mnt->mnt_type, "proc") != 0 && strcmp(mnt->mnt_type, "tmpfs") != 0) {
            // Lister les fichiers dans le point de montage
            int count = list_files(mnt->mnt_dir, files + file_count);
            file_count += count;
            if (file_count >= MAX_FILES) {
                break;
            }
        }
    }

    endmntent(fp);
    return file_count;
}

// Fonction pour calculer la position dans le cylindre du disque (simplifié)
int calculate_cylinder_position(int file_id) {
    return file_id % 10;  // Exemple simplifié : position sur 10 cylindres
}

int main() {
    srand(time(NULL));  // Initialiser le générateur de nombres aléatoires

    FileInfo files[MAX_FILES];
    int file_count = list_files_in_mounts(files);

    if (file_count == 0) {
        printf("Aucun fichier trouvé dans les disques montés.\n");
        return 1;
    }

    // Choisir un fichier aléatoire
    int random_index = rand() % file_count;
    FileInfo selected_file = files[random_index];

    // Afficher les informations
    printf("Fichier sélectionné: %s\n", selected_file.name);
    printf("ID: %d\n", selected_file.id);
    printf("Taille: %ld octets\n", selected_file.size);
    printf("Position dans le cylindre du disque: %d\n", calculate_cylinder_position(selected_file.id));

    return 0;
}