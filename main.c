#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>  
#include <string.h>

/*void get_physical_blocks(const char *filepath, ino_t inode) {
    char command[512];
    char device[128] = {0};

    snprintf(command, sizeof(command), "df \"%s\" | tail -1 | awk '{print $1}'", filepath);
    FILE *mounts = popen(command, "r");
    if (mounts == NULL) {
        perror("Erreur df");
        return;
    }
    if (fgets(device, sizeof(device), mounts) == NULL) {
        fprintf(stderr, "Erreur lecture périphérique\n");
        pclose(mounts);
        return;
    }
    pclose(mounts);
    device[strcspn(device, "\n")] = 0; // Supprime le \n

    if (strlen(device) == 0) {
        fprintf(stderr, "Périphérique non trouvé\n");
        return;
    }

   
    snprintf(command, sizeof(command), "echo 'stat <%ld>' | debugfs -R %s 2>/dev/null", (long)inode, device);
    FILE *debugfs = popen(command, "r");
    if (debugfs == NULL) {
        perror("Erreur debugfs");
        return;
    }

   
    char line[1024];
    while (fgets(line, sizeof(line), debugfs)) {
        if (strstr(line, "BLOCKS:")) {
            printf("  Blocs physiques : %s", line);
            break;
        }
    }
    pclose(debugfs);
}*/
void get_physical_blocks(const char *filepath, ino_t inode, FILE *html_file) {
    char command[512];
    char device[128] = {0};

    // Exécution de la commande df pour obtenir le périphérique de montage
    snprintf(command, sizeof(command), "df \"%s\" | tail -1 | awk '{print $1}'", filepath);
    FILE *mounts = popen(command, "r");
    if (mounts == NULL) {
        perror("Erreur df");
        return;
    }

    if (fgets(device, sizeof(device), mounts) == NULL) {
        fprintf(stderr, "Erreur lecture périphérique\n");
        pclose(mounts);
        return;
    }
    pclose(mounts);
    device[strcspn(device, "\n")] = 0; // Supprime le \n

    if (strlen(device) == 0) {
        fprintf(stderr, "Périphérique non trouvé\n");
        return;
    }

    // Exécution de la commande debugfs pour obtenir des informations sur l'inode
    snprintf(command, sizeof(command), "echo 'stat <%ld>' | debugfs -R %s 2>/dev/null", (long)inode, device);
    FILE *debugfs = popen(command, "r");
    if (debugfs == NULL) {
        perror("Erreur debugfs");
        return;
    }

    char line[1024];
    int blocks_found = 0;
    while (fgets(line, sizeof(line), debugfs)) {
        if (strstr(line, "BLOCKS:")) {
            // Ajouter les blocs physiques dans le fichier HTML
            fprintf(html_file, "<td>%s</td></tr>\n", line);
            blocks_found = 1;
            break;
        }
    }

    if (!blocks_found) {
        //fprintf(html_file, "<td>Aucun bloc trouvé</td></tr>\n");
    }

    pclose(debugfs);
}
int main(int argc, char**argv)
{
	//données
	int index;
	char *rep[] = {"/etc", "/usr/local", "/var/log"};      // Tableau des dossiers
	struct stat st;
    struct tm *temps;
    DIR *dir;
    struct dirent *entry;
	//choix aléatoire des dossier à parcourir
	
    srand(time(NULL)); 						 			// Initialisation du générateur de nombres aléatoires
	index = rand() % 3;									// Choisir un indice aléatoire entre 0 et 2
	printf("Dossier choisi : %s\n", rep[index]);		// Afficher le répertoire  choisi
    //fichier HTML
    FILE *html_file = fopen("affichage.html", "w");
    if (html_file == NULL) {
        perror("Erreur d'ouverture du fichier HTML");
        exit(1);
    }
     // Début du fichier HTML
    fprintf(html_file, "<html>\n<head>\n<title>Informations des fichiers</title>\n</head>\n<body>\n");
    fprintf(html_file, "<h1>Liste des fichiers et répertoires</h1>\n");

    // Début du tableau HTML
    fprintf(html_file, "<table border='1'>\n");
    fprintf(html_file, "<tr><th>Nom</th><th>Taille(octets)</th><th>ID</th><th>Position</th></tr>\n");
	// Ouvrir le répertoire
    dir = opendir(rep[index]);
    if (dir == NULL) {
        perror("Erreur d'accès au répertoire");
        exit(1);
    }

		// Lire le contenu du répertoire
		while ((entry = readdir(dir)) != NULL) {            
        
        if (entry->d_name[0] == '.') {					// Ignorer les répertoires spéciaux "." et ".."
            continue;
        }

		// Construire le chemin absolu du fichier/dossier
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", rep[index], entry->d_name);

        // Récupérer les informations sur le fichier/dossier
        if (stat(path, &st) != 0) {
            perror("Erreur d'accès au fichier");
            continue;
        }

        printf("\nNom : %s\n", entry->d_name);
		if (S_ISDIR(st.st_mode)) {
            fprintf(html_file, "<td>%s</td>",entry->d_name);
            fprintf(html_file, "<td>N/A</td>");
        } 
        else if (S_ISREG(st.st_mode)) {
            fprintf(html_file, "<td>Fichier ordinaire</td>");
            fprintf(html_file, "<td>%ld</td>", st.st_size);
        } 
        else {
            fprintf(html_file, "<td>Autre</td>");
            fprintf(html_file, "<td>%ld</td>",st.st_ino);
        }

			// Afficher l'ID d'inode
        fprintf(html_file, "<td>%ld</td></tr>\n", st.st_ino);
        get_physical_blocks(path, st.st_ino,html_file);
    }

       

		
    closedir(dir);                               // Fermer le répertoire
	
	
	
	
	
	
	
	return(0);
	}
