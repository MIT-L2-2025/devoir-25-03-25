#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/fiemap.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>

const char* PARENTS_DIR[] = {"/etc", "/var/log", "/usr/local"};
const int INDEX_PARENTS_DIR = 3;
//En tête html
void header(FILE *fileInfo){
    fprintf(fileInfo, "<!DOCTYPE html>\n\
                        <html>\n\
                        <head>\n\
                        <title>Information sur les fichiers d'un repertoire</title>\n\
                        </head>\n\
                        <body>\n");
}
//Footer
void footer(FILE *fileInfo){
    fprintf(fileInfo, " </body>\n\
                        </html>\n");
}
// Stocke les noms des sous-répertoires
char **list_subdirs(const char *path, int *count) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return NULL;
    }

    struct dirent *entry;
    struct stat st;
    char **subdirs = NULL;
    int capacity = 10;
    *count = 0;
    subdirs = malloc(capacity * sizeof(char *));
    if (!subdirs) {
        perror("malloc");
        closedir(dir);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (lstat(fullpath, &st) == -1) {
            perror("lstat");
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            if (*count >= capacity) {
                capacity *= 2;
                subdirs = realloc(subdirs, capacity * sizeof(char *));
                if (!subdirs) {
                    perror("realloc");
                    break;
                }
            }
            subdirs[*count] = strdup(entry->d_name);
            (*count)++;
        }
    }    
    closedir(dir);
    return subdirs;
}
// Affiche les infos d'un fichier (inode, taille, bloc physique)
void print_file_info(const char *filepath, FILE* fileInfo) {
    struct stat st;
    if (lstat(filepath, &st) == -1) {
        perror("lstat");
        return;
    }
    fprintf(fileInfo, "<tr>\n");
    fprintf(fileInfo, "<td>%s\n</td>\n", filepath);
    fprintf(fileInfo, "<td>%lu</td>\n", st.st_ino);
    fprintf(fileInfo, "<td>%ld octets</td>\n", st.st_size);
    // Récupération du bloc physique (pour les fichiers réguliers)
    if (S_ISREG(st.st_mode)) {
        int fd = open(filepath, O_RDONLY);
        if (fd == -1) {
            perror("open");
            return;
        }

        struct {
            struct fiemap fiemap;
            struct fiemap_extent extent;
        } fiemap_buf = {0};

        struct fiemap *fiemap = &fiemap_buf.fiemap;
        fiemap->fm_start = 0;
        fiemap->fm_length = ~0ULL;
        fiemap->fm_flags = 0;
        fiemap->fm_extent_count = 1;

        if (ioctl(fd, FS_IOC_FIEMAP, fiemap) == -1) {
            perror("ioctl");
        } else {
            struct fiemap_extent *extent = &fiemap_buf.extent;
            fprintf(fileInfo,"<td>%llu (secteur %llu)</td>\n",
                   extent->fe_physical,
                   extent->fe_physical / 512);
            fprintf(fileInfo, "</tr>\n");
        }
        close(fd);
    } else {
        //Si le fichier n'est pas regulier
        fprintf(fileInfo,"<td>(Pas un fichier régulier)</td>\n");
        fprintf(fileInfo, "</tr>\n");
    }
}
int main() {
    srand(time(NULL));
    //Choisir aleatoirement entre /etc/, /var/log, /usr/local
    int index_parent = rand() % INDEX_PARENTS_DIR;
    const char* parent_dir = PARENTS_DIR[index_parent];
    //Fichier HTML contenant les informations
    FILE* fileInfo = fopen("fileInfo.html", "w");
    header(fileInfo);
    // Lister les sous-répertoires
    int subdir_count = 0;
    char **subdirs = list_subdirs(parent_dir, &subdir_count);
    
    if (!subdirs || subdir_count == 0) {
        printf("Aucun sous-répertoire trouvé.\n");
        return 1;
    }
    // Choisir un sous-répertoire aléatoire
    int random_index = rand() % subdir_count;
    char selected_subdir[1024];
    snprintf(selected_subdir, sizeof(selected_subdir), "%s/%s", parent_dir, subdirs[random_index]);

    fprintf(fileInfo, "<h1>Répertoire choisi aléatoirement: %s</h1>\n", selected_subdir);

    //  Lister ses fichiers avec infos détaillées
    DIR *dir = opendir(selected_subdir);
    if (!dir) {
        perror("opendir");
        goto cleanup;
    }
    struct dirent *entry;
    fprintf(fileInfo, "<h2>Fichiers dans ce répertoire:</h2>\n");
    fprintf(fileInfo, "<table border=1>\n");
    fprintf(fileInfo, "<tr>\n \
        <th>Path</th>\n\
        <th>Inode</th>\n\
        <th>Taille</th>\n\
        <th>Bloc physique</th>\n\
        </tr>");
        int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char filepath[2048];
        snprintf(filepath, sizeof(filepath), "%s/%s", selected_subdir, entry->d_name);
        print_file_info(filepath, fileInfo);
        i++;
    }
    fprintf(fileInfo, "</table>\n <h3>Nombre de fichier: %d<h3>",i);
    footer(fileInfo);
    closedir(dir);
    fclose(fileInfo);
cleanup:
    // Libérer la mémoire
    for (int i = 0; i < subdir_count; i++) {
        free(subdirs[i]);
    }
    free(subdirs);

    return 0;
}