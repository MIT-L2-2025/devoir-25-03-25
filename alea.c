#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

void headers_html(FILE *html,char* argv)
{

    fprintf(html, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(html, "<title>Listes dans - %s</title>\n", argv);
    fprintf(html, "<style>table {border-collapse: collapse;} th, td {border: 1px solid black; padding: 8px;}</style>\n");
    fprintf(html, "</head>\n<body>\n");
    fprintf(html, "<h1>Liste des fichiers et repertoires du  %s</h1>\n", argv);
    fprintf(html, "<table>\n");
    fprintf(html, "<tr><th>Nom du fichier  </th><th>Taille (octets)</th><th>Position dans le Cylindre </th><th>ID (Inode)</th></tr>\n");
}
void execute(char *argv)
{ 
    char path_files[255];
    DIR *path = opendir(argv);
    printf("===> %s\n",argv);
    struct dirent *files;
    struct stat information_files;
    if (path == NULL)
    {
        printf("Erreur lors de l'ouverture du repertoire %s\n",argv);
        return;
    }
    int nbr_files = 0;
    while((files = readdir(path)) != NULL)
    {
        nbr_files++;
    }
    rewinddir(path);
    
    FILE *html = fopen("affichage.html","w+");
    if (html == NULL)
    {
        perror("erreur lors de l'ouverture du fichier \n");
        return;
    }
    headers_html(html,argv);

    while((files = readdir(path)) != NULL)
    {
        snprintf(path_files,sizeof(path_files),"%s/%s",argv,files->d_name);
        
        if (stat(path_files, &information_files) == -1) continue;
        char commande[1000];

        snprintf(commande, sizeof(commande), "sudo filefrag -v %s > temp.txt", path_files);
        system(commande);
        unsigned long long block = 0;
        FILE *temp = fopen("temp.txt", "r");
        if (temp) 
        {
            char ligne[256];
            while (fgets(ligne, sizeof(ligne), temp))
            {
                if (strstr(ligne, "physical_offset")) continue;
                if (sscanf(ligne, "%*d: %*d.. %*d: %llu..", (unsigned long long*)&block) == 1) 
                {
                    printf(", Block : %llu octets\n", block);
                    break;
                }
            }
            fclose(temp);
            remove("temp.txt"); 
        } 
        fprintf(html, "<tr>");
        fprintf(html, "<td>%s</td>", files->d_name);
        fprintf(html, "<td>%ld</td>", information_files.st_size);
        fprintf(html, "<td>%llu</td>", block);
        fprintf(html, "<td>%ld</td>", information_files.st_ino);
        fprintf(html, "</tr>\n");
        printf("============> %s\n",path_files);
        printf("Nom : %s | Inode : %lu | Taille : %ld octets \n",files->d_name,(unsigned long)information_files.st_ino,(long)information_files.st_size);
    }
    fprintf(html, "</table>\n");
    fprintf(html, "</body>\n</html>");
    closedir(path);
}

int main()
{
    char *lists[] = {"/etc","/usr/local","/var/log"};

    srand(time(NULL));
    int round = (rand()%3)+1;
    printf("%d\n",round);
    execute(lists[round-1]);
    return (0);
}






