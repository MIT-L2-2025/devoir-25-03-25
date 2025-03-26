#ifndef __SCRIPT__
#define __SCRIPT__
short Index();         //choisi le repertoire 
void getID(char *dir[],short random);          //prend l'ID des fichiers dans le repertoire
void getsize(char *dir[],short random);        //prend la taille des fichiers dans le repertoire
void getposition(char *dir[],short random);    //prend la position des fichiers dans le cylindre 
void get_html_page();                          //retourne une page html
#endif