#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH 512


// Fonction pour exécuter une commande shell et récupérer sa sortie
void executer_commande(const char *commande, char *resultat, int taille) {
    FILE *fp = popen(commande, "r");
    if (!fp) {
        perror("Erreur lors de l'exécution de la commande");
        return;
    }
    if (fgets(resultat, taille, fp)) {
        resultat[strcspn(resultat, "\n")] = 0; // Supprimer le saut de ligne
    }
    pclose(fp);
}

// Fonction pour récupérer l'ID, la taille et le bloc disque d'un fichier
void obtenir_infos_fichier(const char *chemin, FILE *html) {
    char commande[MAX_PATH];
    char resultat[MAX_PATH];

    // Obtenir l'ID et la taille
    snprintf(commande, sizeof(commande), "stat --format '%%i %%s' \"%s\"", chemin);
    executer_commande(commande, resultat, sizeof(resultat));
    
    unsigned long inode;
    long taille;
    sscanf(resultat, "%lu %ld", &inode, &taille);

    // Obtenir le bloc disque
    snprintf(commande, sizeof(commande), "filefrag -v \"%s\" | awk 'NR==4 {print $4}'", chemin);
    executer_commande(commande, resultat, sizeof(resultat));

    int bloc = -1;
    sscanf(resultat, "%d", &bloc);

    // Écrire dans le fichier HTML
    fprintf(html, "<tr><td>%s</td><td>%lu</td><td>%ld</td><td>%d</td></tr>\n", chemin, inode, taille, bloc);
}


// Fonction pour récupérer les fichiers d'un répertoire
void lister_fichiers(const char *dossier, FILE *html) {
    char commande[MAX_PATH];
    char ligne[MAX_PATH];
    FILE *fp;
    int taille=0;

    snprintf(commande, sizeof(commande), "ls -p \"%s\" | grep -v /", dossier);
    fp = popen(commande, "r");
    if (!fp) {
        perror("Erreur lors de la lecture du répertoire");
        return;
    }

    while (fgets(ligne, sizeof(ligne), fp)) {
        ligne[strcspn(ligne, "\n")] = 0; // Supprimer le retour à la ligne
        char chemin_fichier[1024];
        snprintf(chemin_fichier, sizeof(chemin_fichier), "%s/%s", dossier, ligne);
        obtenir_infos_fichier(chemin_fichier, html);
    }

    pclose(fp);
}



int main() {
    const char *dossier = ".";  // Répertoire actuel
    FILE *html = fopen("resultat.html", "w");

    if (!html) {
        perror("Erreur création fichier HTML");
        return 1;
    }

    // Début du HTML
    fprintf(html, "<html><body><table border='1'>");
    fprintf(html, "<tr><th>Fichier</th><th>ID</th><th>Taille (octets)</th><th>Bloc disque</th></tr>\n");

    lister_fichiers(dossier, html);

    // Fin du HTML
    fprintf(html, "</table></body></html>");
    fclose(html);

    printf("Fichier HTML généré : resultat.html\n");
    return 0;
}
