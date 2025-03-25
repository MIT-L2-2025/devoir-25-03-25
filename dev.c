#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/hdreg.h>

// Structure pour stocker la géométrie du disque
typedef struct {
    int sectors_per_track;
    int tracks_per_cylinder;
    int total_cylinders;
    int bytes_per_sector;
} DiskGeometry;

// Vérifie si le périphérique est un disque rotatif (HDD)
int is_rotational_disk(const char *disk_device) {
    char path[256];
    snprintf(path, sizeof(path), "/sys/block/%s/queue/rotational", disk_device + 5); // Extrait sda de /dev/sda
    FILE *fp = fopen(path, "r");
    if (!fp) 
        return -1; // Impossible de vérifier, assume non rotatif par défaut
    int rotational;
    fscanf(fp, "%d", &rotational);
    fclose(fp);
    return rotational;
}

// Fonction pour obtenir la géométrie du disque
int get_disk_geometry(const char *disk_path, DiskGeometry *geo) {
    int fd = open(disk_path, O_RDONLY);
    if (fd == -1) {
        perror("Erreur lors de l'ouverture du périphérique disque");
        return -1;
    }

    struct hd_geometry disk_geo;
    if (ioctl(fd, HDIO_GETGEO, &disk_geo) == -1) {
        perror("Erreur lors de la récupération de la géométrie (peut-être SSD/NVMe ?)");
        close(fd);
        return -1;
    }

    geo->sectors_per_track = disk_geo.sectors;
    geo->tracks_per_cylinder = disk_geo.heads;
    geo->total_cylinders = disk_geo.cylinders;
    geo->bytes_per_sector = 512; 

    close(fd);
    return 0;
}

// Calcule la position du cylindre à partir du numéro de bloc
long calculate_cylinder_position(long block_num, DiskGeometry *geo) {
    if (block_num < 0) 
        return -1; // Erreur ou non disponible
    
    int blocks_per_cylinder = geo->sectors_per_track * geo->tracks_per_cylinder;
    long cylinder = block_num / blocks_per_cylinder;

    if (cylinder >= geo->total_cylinders) 
        return -1; // Hors limites
    return cylinder;
}

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *ent;
    struct stat file_stat;
    char filepath[1024];
    const char *disk_device = (argc > 1) ? argv[1] : "/dev/sda"; // Permet de spécifier le disque en argument
    const char *directories[] = {"/etc", "/usr/local", "/var/log"};
    int num_dirs = 3;

    // Vérifie si l'utilisateur est root
    if (geteuid() != 0) {
        fprintf(stderr, "Ce programme doit être exécuté avec des privilèges root (sudo).\n");
        exit(1);
    }

    // Récupère la géométrie du disque
    DiskGeometry geo;
    int geo_available = 0;
    // Vérifie si disque rotatif
    if (is_rotational_disk(disk_device) == 1) { 
        if (get_disk_geometry(disk_device, &geo) == 0) 
            geo_available = 1; 
        else 
            fprintf(stderr, "Géométrie non disponible, positions des cylindres seront indéterminées.\n");
    } 
    else 
        fprintf(stderr, "Disque non rotatif (SSD/NVMe ?), géométrie non applicable.\n");
    

    // Choix aléatoire d'un répertoire
    srand(time(NULL));
    int chosen_dir = rand() % num_dirs;
    dir = opendir(directories[chosen_dir]);
    if (dir == NULL) {
        fprintf(stderr, "Erreur d'ouverture du répertoire %s\n", directories[chosen_dir]);
        exit(1);
    }

    // Crée le fichier HTML
    FILE *html_file = fopen("fichiers.html", "w");
    if (html_file == NULL) {
        fprintf(stderr, "Erreur lors de la création du fichier HTML\n");
        closedir(dir);
        exit(1);
    }

    // Écriture de l'en-tête HTML
    fprintf(html_file, "<html>\n<head><title>Liste des fichiers dans %s</title></head>\n", directories[chosen_dir]);
    fprintf(html_file, "<body>\n<h1>Liste des fichiers dans %s (Disque: %s)</h1>\n", directories[chosen_dir], disk_device);
    fprintf(html_file, "<table border=\"1\">\n<tr><th>Nom du fichier</th><th>Inode</th><th>Taille (octets)</th><th>Cylindre estimé</th></tr>\n");

    // Parcourt les fichiers du répertoire
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) 
            continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", directories[chosen_dir], ent->d_name);

        if (stat(filepath, &file_stat) == -1) {
            fprintf(stderr, "Erreur stat pour %s\n", filepath);
            continue;
        }

        // Récupère le numéro de bloc physique
        int fd = open(filepath, O_RDONLY);
        long block_num = -1;
        long cylinder_pos = -1;
        if (fd != -1) {
            int block = 0;
            if (ioctl(fd, FIBMAP, &block) == 0 && geo_available) {
                block_num = block;
                cylinder_pos = calculate_cylinder_position(block_num, &geo);
            }
            close(fd);
        }

        fprintf(html_file, "<tr><td>%s</td><td>%lu</td><td>%ld</td><td>%ld</td></tr>\n",
                ent->d_name,
                (unsigned long)file_stat.st_ino,
                (long)file_stat.st_size,
                cylinder_pos);
    }

    fprintf(html_file, "</table>\n</body>\n</html>\n");
    closedir(dir);
    fclose(html_file);

    printf("Fichier HTML généré avec succès : fichiers.html\n");
    return 0;
}