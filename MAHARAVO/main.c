#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

int main(){
    int choix;
    long int position;
    char commande[350];
    char ligne[265];
    FILE *fichier = fopen("index.html", "w");
    struct  stat st;
    FILE *tri=NULL;
    DIR *dir;
    int i;
    struct  dirent *ent;
    char *dossier[]={"/usr/local","/var/log" ,"/etc"};
    int directory=sizeof(dossier)/sizeof(dossier[0]);
    srand(time(NULL));
    
    choix=rand()%directory;
    printf ("Dossier choisi : %s\n",dossier[choix]);


    fprintf(fichier, "<!DOCTYPE html>\n");
    fprintf(fichier, "<html lang=\"fr\">\n");
    fprintf(fichier, "<head>\n");
    fprintf(fichier, "    <meta charset=\"UTF-8\">\n");
    fprintf(fichier, "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(fichier, "    <title>Liste </title>\n");
    fprintf(fichier, "    <style>\n");
    fprintf(fichier, "        body {\n");
    fprintf(fichier, "            font-family: Arial, sans-serif;\n");
    fprintf(fichier, "            margin: 20px;\n");
    fprintf(fichier, "        }\n");
    fprintf(fichier, "        table {\n");
    fprintf(fichier, "            width: 80%%;\n");
    fprintf(fichier, "            margin: 20px auto;\n");
    fprintf(fichier, "            border-collapse: collapse;\n");
    fprintf(fichier, "            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);\n");
    fprintf(fichier, "        }\n");
    fprintf(fichier, "        th, td {\n");
    fprintf(fichier, "            padding: 12px;\n");
    fprintf(fichier, "            text-align: center;\n");
    fprintf(fichier, "            border: 1px solid #ddd;\n");
    fprintf(fichier, "        }\n");
    fprintf(fichier, "        th {\n");
    fprintf(fichier, "            background-color: #4CAF50;\n");
    fprintf(fichier, "            color: white;\n");
    fprintf(fichier, "        }\n");
    fprintf(fichier, "        tr:nth-child(even) {\n");
    fprintf(fichier, "            background-color: #f2f2f2;\n");
    fprintf(fichier, "        }\n");
    fprintf(fichier, "        tr:hover {\n");
    fprintf(fichier, "            background-color: #ddd;\n");
    fprintf(fichier, "        }\n");
    fprintf(fichier, "    </style>\n");
    fprintf(fichier, "</head>\n");
    fprintf(fichier, "<body>\n");
    fprintf(fichier, "    <h1><center>Liste du dosier %s</center></h1>\n",dossier[choix]);
    fprintf(fichier, "    <table>\n");
    fprintf(fichier, "        <tr>\n");
    fprintf(fichier, "            <th>Nom</th>\n");
    fprintf(fichier, "            <th>id</th>\n");
    fprintf(fichier, "            <th>taille</th>\n");
    fprintf(fichier, "            <th>position</th>\n");
    fprintf(fichier, "        </tr>\n");
   
    dir=opendir(dossier[choix]);
    while((ent=readdir(dir))!=NULL){
        printf("%s---",ent->d_name);
        printf("taille: %ld",ent->d_off);
        printf("\tid=%lu\n",ent->d_ino);
        fprintf(fichier, "        <tr>\n");
        fprintf(fichier, "            <td>%s</td>\n",ent->d_name);
        fprintf(fichier, "            <td>%lu</td>\n",ent->d_ino);
        fprintf(fichier, "            <td>%ld</td>\n",ent->d_off);
        
        sprintf(commande, "sudo filefrag -v %s/%s",dossier[choix], ent->d_name);

        tri=popen(commande,"r");
        while (fgets(ligne, sizeof(ligne), tri)) {
            int n=sscanf(ligne,"%*d: %*s %*d: %*s %ld: %*d: %*s ",&position);
        //    printf("position:%ld\tligne :%s\n",position,ligne);
            
        }
        fprintf(fichier, "            <td>%ld</td>\n",position);
        position=0;
       
        fprintf(fichier, "        </tr>\n");

        printf("\n");
    }
    fprintf(fichier, "    </table>\n");
    fprintf(fichier, "</body>\n");
    fprintf(fichier, "</html>\n");
    
    fclose(fichier);
    
    return 0;
}