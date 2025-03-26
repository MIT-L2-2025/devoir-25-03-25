#include "fonction.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
short Index(){
    char command[20];
    short x=0;
    short y;
    FILE*file=fopen("temp","r");
    fscanf(file,"%hd",&y);
    while(x==y){
        srand(time(NULL));
        x=rand() % 3;
    }
    sprintf(command,"echo %hd > temp",x);
    system(command);
    return x;
}
void getID(char *dir[],short random){
    char command[100];
    unsigned long long int file_amount;
    sprintf(command,"ls -1 %s|wc -l > a",dir[random]);
    FILE*file=popen(command,"r");
    pclose(file);
    FILE*temp=fopen("a","r");
    fscanf(temp,"%lld",&file_amount);
    fclose(temp);
    strcpy(command,"");
    sprintf(command,"ls -1i %s > a",dir[random]);
    FILE*file1=popen(command,"r");
    pclose(file1);
    //return file_amount;
}
void getsize(char *dir[],short random){
    char command[100],filename[40];
    sprintf(command,"ls -1 %s > b",dir[random]);
    system("touch c");
    system(command);
    FILE*file=fopen("b","r");
    strcpy(command,"");
    while(!feof(file)){
        fscanf(file,"%s",filename);
        sprintf(command,"sudo du -h --max-depth=0 %s/%s >> c",dir[random],filename);
        system(command);
        strcpy(command,"");
    }
    fclose(file);
}        
void getposition(char *dir[],short random){
    //system("touch g");
    FILE*file=fopen("b","r");
    int line_number=0;
    char filename[40],command[100],bloc[15],string[100],flags[10];
    unsigned long int secteurs_par_piste,nombre_de_tete,bloc_physique,none;
    float cylindre;
    while(!feof(file)){
        fscanf(file,"%s",filename);
        sprintf(command,"sudo df -h %s/%s > d",dir[random],filename);
        system(command);
        FILE*open_d=fopen("d","r");
        while(fgets(string,100,open_d)){
            line_number++;
            if(line_number==2){
                sscanf(string,"%s",bloc);
            }
        }
        fclose(open_d);
        //printf("%s\n",bloc);
        line_number=0;
        strcpy(command,"");
        sprintf(command,"sudo hdparm -g %s > e",bloc);
        system(command);
        FILE*open_e=fopen("e","r");
        while(fgets(string,100,open_e)){
            line_number++;
            if(line_number==3){
                sscanf(string," geometry      = %ld/%ld/%ld,",&none,&secteurs_par_piste,&nombre_de_tete);
                //printf("%ld|%ld\n",secteurs_par_piste,nombre_de_tete);
            }
        }
        fclose(open_d);
        strcpy(command,"");
        line_number=0;
        sprintf(command,"sudo filefrag -v %s/%s > f",dir[random],filename);
        system(command);
        FILE*open_f=fopen("f","r");
        while(fgets(string,100,open_f)){
            line_number++;
            if(line_number=4){
                sscanf(string,"   %ld:        %ld..       %ld:    %ld..   %ld:      %ld:             %s",&none,&none,&none,&bloc_physique,&none,&none,flags);
                //printf("%ld\n",bloc_physique);
            }
        }
        fclose(open_f);
        line_number=0;
        cylindre=bloc_physique/(secteurs_par_piste*nombre_de_tete);
        //printf("%f\n",cylindre);
        strcpy(command,"");
        sprintf(command,"echo \"%g %s\" >> g",cylindre,filename);
        system(command);      
    }
    fclose(file);
} 
void get_html_page(){
    unsigned long long int id;
    float cylindre;
    char size[20],none[50];
    char filename[20];
    FILE*file=fopen("result.html","w");
    fprintf(file,"<html>\n");
    fprintf(file,"  <body>\n");
    fprintf(file,"      <table border='1'>\n");
    fprintf(file,"          <tr><td>Nom_fichier</td><td>taille</td><td>ID</td><td>cylindre</td></tr>\n");
    FILE*open_g=fopen("g","r");
    FILE*open_a=fopen("a","r");
    FILE*open_c=fopen("c","r");
    while(!feof(open_a)){
        fscanf(open_a,"%lld %s",&id,filename);
        fscanf(open_g,"%f %s",&cylindre,none);
        fscanf(open_c,"%s %s",size,none);
        fprintf(file,"          <tr><td>%s</td><td>%s</td><td>%lld</td><td>%f</td></tr>\n",filename,size,id,cylindre);
    }
    fclose(open_a);
    fclose(open_g);
    fclose(open_c);
    fprintf(file,"      </table>\n");
    fprintf(file,"  </body>\n");
    fprintf(file,"<html>\n");
    fclose(file);
    system("rm a b c d e f g");
}