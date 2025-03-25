#include <stdio.h>
#include <stdlib.h>
#include "sys.h"

int main() {
    FILE *html_sorted_inode = NULL;
    FILE *html_sorted_name = NULL;
    About_file *files = NULL;
    int line_count,                                 //nombre de fichier a traiter
        file_treated;                               //nombre de fichier réellement traitée
    char *dir;

    dir = pick_dir();
    printf("dossier: %s\n", dir);

    // Premier ouverture pipe: compter les fichiers dans le dossier
    FILE *list_file = get_list_file(dir);
    if (list_file == NULL) {
        printf("Erreur lors de la première ouverture du pipe\n");
        return 1;
    }
    line_count = count_line(list_file);
    pclose(list_file);

    if (line_count <= 0) {
        printf("Aucun fichier trouvé ou erreur de comptage\n");
        return 1;
    }

   files = malloc(line_count * sizeof(About_file));
    if (files == NULL) {
        printf("Erreur d'allocation mémoire\n");
        return 1;
    }

    // Seconde ouverture pipe : collecter les fichiers
    list_file = get_list_file(dir);
    if (list_file == NULL) {
        printf("Erreur lors de la seconde ouverture du pipe\n");
        free(files);
        return 1;
    }
    file_treated = collect_from_files(list_file, files);
    pclose(list_file);

    if (file_treated != line_count) {
        printf("Attention : %d fichiers collectes au lieu de %d comptes\n", file_treated, line_count);
    }

    // Génération des fichiers HTML
    // 1. Tri par inode (facultatif)
    html_sorted_inode = fopen("sorted_by_inode.html", "w");
    if (html_sorted_inode == NULL) {
        printf("erreur ouverture du fichier html (inodes)\n");
        free(files);
        return 1;
    }
    sort_by_inode(files, file_treated);
    html_head(html_sorted_inode, dir, "Inode");
    for (int i = 0; i < file_treated; i++) {
        html_line(html_sorted_inode, &files[i]);    //insérer dans le fichier html
    }
    html_foot(html_sorted_inode);
    fclose(html_sorted_inode);

    // 2. Tri par nom
    html_sorted_name = fopen("sorted_by_name.html", "w");
    if (html_sorted_name == NULL) {
        printf("erreur ouverture du fichier html (nom)\n");
        free(files);
        return 1;
    }
    sort_by_name(files, file_treated);
    html_head(html_sorted_name, dir, "Nom");
    for (int i = 0; i < file_treated; i++) {
        html_line(html_sorted_name, &files[i]);
    }
    html_foot(html_sorted_name);
    fclose(html_sorted_name);

    // 3. Tri par taille (facultatif)
    FILE *html_size = fopen("sorted_by_size.html", "w");
    if (html_size == NULL) {
        printf("erreur ouverture du fichier html (taille)\n");
        free(files);
        return 1;
    }
    sort_by_size(files, file_treated);
    html_head(html_size, dir, "Taille");
    for (int i = 0; i < file_treated; i++) {
        html_line(html_size, &files[i]);
    }
    html_foot(html_size);
    fclose(html_size);
    free(files);
    return 0;
}
