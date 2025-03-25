#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int analyse_index(){
    FILE *fichier_index=fopen("index","r");
    int index_rep=0;
    if (fichier_index!=NULL){
        fscanf(fichier_index,"%d",&index_rep);
        fclose(fichier_index);
    }
    return index_rep;
}

void renouv_index(int index_rep){
    FILE *fichier_index=fopen("index","w");
    if (fichier_index != NULL){
        fprintf(fichier_index,"%d", index_rep);
        fclose(fichier_index);
    }
}

void creation_html(char *chemin_rep){
    struct stat info_fichier;
    DIR *dp=opendir(chemin_rep);

    FILE *htmlFile=fopen("affichage.html","w");
 
    fprintf(htmlFile,"<HTML>\n<HEAD><TITLE>informations</TITLE></HEAD>\n<BODY>\n");
    fprintf(htmlFile,"<H1>Informations sur les fichiers dans %s</H1>\n", chemin_rep);
    fprintf(htmlFile,"<TABLE BORDER='1'>\n<TR><TH>Fichier</TH><TH>ID</TH><TH>Taille en octets</TH><TH>Position (en blocs)</TH></TR>\n");

    struct dirent *entry;
    while ((entry=readdir(dp))){
        if (strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
            continue;

        char *filepath=(char *)malloc(strlen(chemin_rep)+strlen(entry->d_name)+2);
        if (filepath == NULL){
            perror("Erreur d'allocation de mémoire");
            fclose(htmlFile);
            closedir(dp);
            return;
        }

        snprintf(filepath, strlen(chemin_rep)+strlen(entry->d_name)+2,"%s/%s",chemin_rep,entry->d_name);
        if (stat(filepath, &info_fichier)==-1){
            free(filepath);
            continue;
        }

        fprintf(htmlFile,"<TR><TD>%s</TD><TD>%lu</TD><TD>%lld</TD><TD>%lld</TD></TR>\n",
        entry->d_name, info_fichier.st_ino,(long long)info_fichier.st_size,(long long)(info_fichier.st_blocks));
        free(filepath);
    }

    fprintf(htmlFile,"</TABLE>\n</BODY>\n</html>\n");
    fclose(htmlFile);
    closedir(dp);
}

int main(){
    char *repert[]={"/etc","/usr/local","/var/log"};
    int index_rep=analyse_index();

    char *chemin_rep=repert[index_rep];
    creation_html(chemin_rep);

    index_rep=(index_rep+1)%3;
    renouv_index(index_rep);

    printf("Le fichier HTML a été créé avec succès : affichage.html\n");
    return 0;
}
