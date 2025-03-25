#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

void fonction();
int main()
{
    fonction();
    return 0;
}
void fonction()
{
    char cmd[200];
    char line[256];
    const char* directory[] = {"/etc","/usr/local","/var/log"};
    srand(time(NULL));
    int index = rand() % 3;
    sprintf(cmd,"ls -li %s|awk '{print $10,$1,$6}' > file.txt",directory[index]);
    system(cmd);

    FILE *file,*file_html,*fp; 
    file = fopen("file.txt","r");
    file_html = fopen("file.html","w");
    if(file == NULL)
    {
        printf("Erreur de creation de fichier HTML!");
        fclose(file);
        exit(1);
    }
    if(file_html == NULL)
    {
        printf("Erreur de creation de fichier HTML!");
        fclose(file_html);
        exit(1);
    }
    //Écriture dans le fichier HTML
    fprintf(file_html, "<!DOCTYPE html>\n<html>\n<head>\n<title>Tableau des Données</title>\n</head>\n<body>\n");
	fprintf(file_html, "<h1>Tableau des Données du répertoire %s</h1>\n",directory[index]);
	fprintf(file_html, "<table border=\"1\">\n<tr><th>Name</th><th>ID</th><th>Size</th><th>Position(bloc debut-bloc fin)</tr>\n");
    char result[256];
	char name[50], id[50], size[50];
	fscanf(file, "%s %s %s", name, id, size);
	while(fscanf(file, "%s %s %s", size, name, id) != EOF) 
    {
            sprintf(cmd,"filefrag -v %s/%s |awk '/0:/ {print $4 $5}' ",directory[index],name);
            fp = popen(cmd, "r");
            if (fgets(result, sizeof(result), fp) != NULL) 
            {
                fprintf(file_html, "<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", name, id, size,result);
                fclose(fp);
            }
     }
     fprintf(file_html, "</table>\n</body>\n</html>\n");
     fclose(file);
     fclose(file_html);
}