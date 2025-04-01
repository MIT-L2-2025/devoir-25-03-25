#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<string.h>
#include <sys/stat.h> 
#include <unistd.h> 
#include <dirent.h>

void head(FILE* file); 
void currencyFile(FILE* file); // Fonction pour sélectionner un répertoire et afficher son inode
void getID(const char* repertoire, FILE* file); // Fonction pour récupérer l'inode et l'afficher
void getFileFrag(const char* fichier, FILE* file); 

int main()
{
    FILE* file = fopen("resultat.html", "w"); // Ouvrir le fichier HTML en mode écriture
    if (file == NULL) 
    {
        perror("Erreur d'ouverture du fichier");
        return 1;
    }

    srand(time(NULL)); // Initialiser le générateur de nombres aléatoires
    head(file);
    currencyFile(file);

    fprintf(file, "</body>\n</html>\n");
    fclose(file); // Fermer le fichier
    return 0;
}
void head(FILE* file)
{
    // Écrire l'en-tête HTML dans le fichier
    fprintf(file, "<!DOCTYPE html>\n");
    fprintf(file, "<html lang=\"fr\">\n");
    fprintf(file, "<head>\n");
    fprintf(file, "<meta charset=\"UTF-8\">\n");
    fprintf(file, "<title>Informations sur le répertoire</title>\n");
    fprintf(file, "<style>\n");
    fprintf(file, "table { width: 80%%; border-collapse: collapse; margin: 20px auto; }\n");
    fprintf(file, "th, td { border: 1px solid black; padding: 10px; text-align: center; }\n");
    fprintf(file, "th { background-color: #f2f2f2; }\n");
    fprintf(file, "</style>\n");
    fprintf(file, "</head>\n");
    fprintf(file, "<body>\n");
}
void currencyFile(FILE* file)
{
    char* repertoires[] = {"/etc", "/usr/local", "/var/log"};
    int index;
    index = rand() % 3; // Sélectionner un répertoire au hasard entre les trois 

    DIR* dir;
    dir = opendir(repertoires[index]);

    if (dir == NULL) 
    {
        fprintf(file, "<h1>Erreur d'ouverture du répertoire : %s</h1>\n", repertoires[index]);
        return;
    }
    
    struct dirent* entry;
    fprintf(file, "<h1>Répertoire sélectionné : %s</h1>\n", repertoires[index]);
    fprintf(file, "<table>\n<tr><th>Nom</th><th>Inode</th><th>Taille (octets)</th><th>Position</th></tr>\n");

    while ((entry = readdir(dir)) != NULL) 
    {   
        if (entry->d_name[0] == '.') // Ignorer les fichiers et répertoires qui commencent par '.'
        {
            continue;
        }
        
        char chemin[1024];
        snprintf(chemin, sizeof(chemin), "%s/%s", repertoires[index], entry->d_name);
        getID(chemin, file);
    }

    fprintf(file, "</table>\n");
    closedir(dir); // Fermer le répertoire après avoir terminé
}

void getID(const char* repertoire, FILE* file)
{
    struct stat info;  // Structure pour stocker les informations sur le fichier

    if (stat(repertoire, &info) == 0) 
    {
        fprintf(file, "<tr><td>%s</td><td>%ld</td><td>%ld</td>", repertoire, (long)info.st_ino, (long)info.st_size);
        getFileFrag(repertoire, file);
    }
}
void getFileFrag(const char* fichier, FILE* file) 
{
    char command[1024];
    FILE* fp;
    char result[1024] = "Non disponible"; 

    // Créer la commande filefrag pour obtenir la position des blocs du fichier
    snprintf(command, sizeof(command), "filefrag -v %s | tail -n 2 | head -n 1 | cut -d : -f3", fichier);

    fp = popen(command, "r");
    if (fp == NULL) 
    {
        fprintf(file, "<td>%s</td></tr>\n", result);
        return;
    }

    // Lire et stocker le résultat de filefrag
    if (fgets(result, sizeof(result), fp) == NULL) 
    {
        strcpy(result, "Non disponible"); 
    }

    result[strcspn(result, "\n")] = 0;
    fprintf(file, "<td>%s</td></tr>\n", result);

    pclose(fp);
}
