#include "blocfichier.h"

const char* get_random_directory() {
    const char *dirs[NUM_DIRS] = {"/etc", "/usr/local", "/var/log"};
    //const char *dirs[NUM_DIRS] = {"/home/ronhyraktos/DOSSIER/L2/Système"};
    return dirs[rand() % NUM_DIRS];
}

void generate_html_table(FileInfo *files, int num_files, const char *dir) {
    FILE *html = fopen("fichiers_info.html", "w");
    if (html == NULL) {
        perror("Erreur lors de la création du fichier HTML");
        exit(1);
    }

    // Début du fichier HTML
    fprintf(html, 
        "<!DOCTYPE html>\n"
        "<html lang=\"fr\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>Tableau Bloc Fichier</title>\n"
        "    <link rel=\"stylesheet\" href=\"style.css\">\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"table-container\">\n"
        "<center><h3>Repertoire : %s</h3></center>\n"
        "<br><br>\n"
        "<table>\n"
        "<thead>\n"
        "    <tr>\n"
        "        <th>Nom</th>\n"
        "        <th>Id</th>\n"
        "        <th>Taille</th>\n"
        "        <th>Position</th>\n"
        "    </tr>\n"
        "</thead>\n"
        "<tbody>\n",
        dir
    );

    // Ajouter chaque fichier au tableau HTML
    for (int i = 0; i < num_files; i++) {
        fprintf(html, 
        "<tr>\n"
        "    <td>%s</td>\n"
        "    <td>%lu</td>\n"
        "    <td>%ld</td>\n"
        "    <td>%lu</td>\n"
        "</tr>\n", 
        files[i].path, files[i].inode, files[i].size, files[i].position);
    }

    // Fin du fichier HTML
    fprintf(html,
        "       </tbody>\n"
        "   </table>\n"
        "   </div>\n"
        "</body>\n"
        "</html>\n"
    );
    fclose(html);
}
