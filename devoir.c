#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

char *choice_dir();
void head_html(FILE *fp);
void body_html(FILE *fp,char *dir);
void content(FILE *fp,char *dir,struct dirent *ent);

int main()
{
    struct dirent *ent;
    srand(time(NULL));
    char *dir = choice_dir();
    
    FILE *fp = fopen("resultat.html", "w");
    if (fp == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 1;
    }
    
    head_html(fp);
    body_html(fp,dir);
    
    DIR *p_dir = opendir(dir);
    if (p_dir == NULL) {
        perror("Impossible d'ouvrir le répertoire");
        fclose(fp);
        return 1;
    }

	//parcourir le dossier 
    while ((ent = readdir(p_dir)) != NULL) 
    {
       content(fp,dir,ent);
    }
    
    closedir(p_dir);
	fprintf(fp, "</table>\n</body>\n</html>\n");
    fclose(fp);
    return 0;
}

char *choice_dir() 
{
    char *directories[] = { "/etc", "/usr/local", "/var/log" };
    char *dir = directories[rand() % 3];
    size_t len = strlen(dir);
    if (len > 0 && (dir[len - 1] == '%' || dir[len - 1] == '\n')) {
        dir[len - 1] = '\0';
    }
	return dir;
}

void head_html(FILE *fp)
{
	fprintf(fp, "<html>\n<head>\n<title>Résultat</title>\n");
    fprintf(fp, "<style>\n");
    fprintf(fp, "table {\n");
    fprintf(fp, "    width: 80%%;\n"); 
    fprintf(fp, "    border-collapse: collapse;\n");
    fprintf(fp, "    margin: 20px auto;\n");
    fprintf(fp, "}\n");
    fprintf(fp, "th, td {\n");
    fprintf(fp, "    border: 1px solid black;\n");
    fprintf(fp, "    padding: 10px;\n");
    fprintf(fp, "    text-align: center;\n");
    fprintf(fp, "}\n");
    fprintf(fp, "</style>\n");
    fprintf(fp,"</head>");
}

void body_html(FILE *fp,char *dir)
{
	fprintf(fp,"<body>");
    fprintf(fp,"<h2>Dans le répertoire %s</h2>", dir);
    fprintf(fp, "<table>");
    fprintf(fp, "<tr>\n");
    fprintf(fp, "<th>Nom</th>\n");
    fprintf(fp, "<th>Taille (octets)</th>\n");
    fprintf(fp, "<th>Inode</th>\n");
    fprintf(fp, "<th>Position</th>\n");
    fprintf(fp, "</tr>\n");
}

void content(FILE *fp,char *dir,struct dirent *ent)
{
	char full_path[512]; //chemin vers les fichier
	char resultat[200];
	char commande[600];	//variable ppur stocker la commande
	struct stat file_stat;
    snprintf(full_path, sizeof(full_path), "%s/%s", dir, ent->d_name);

	if (stat(full_path, &file_stat) == -1) {
        perror("Erreur stat");
    }
    //ecrire dans 'commande' la commande à executer 
    snprintf(commande, sizeof(commande), "filefrag -v %s | tail -n 2 | head -n 1 | cut -d : -f3", full_path);
    FILE *cmd = popen(commande, "r");
    if (cmd == NULL) {
        perror("Erreur d'exécution de la commande");
        fclose(fp);
    }
    // Résultat de la commande filefrag
    fgets(resultat, sizeof(resultat), cmd);
    fclose(cmd);
    
    //Tableau html
    fprintf(fp, "<tr>\n");
    fprintf(fp, "<td>%s</td>\n", ent->d_name);
    fprintf(fp, "<td>%ld</td>\n", (long)file_stat.st_size);
    fprintf(fp, "<td>%ld</td>\n", (long)file_stat.st_ino);
	fprintf(fp, "<td>%s</td>\n", resultat);  // Afficher la position du fichier
	fprintf(fp, "</tr>\n");
}

