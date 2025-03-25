#ifndef FILE_DETAILS_H
    #define FILE_DETAILS_H

    /**
     * @brief
     * Sujet : 
     * Dans une programme ecrit aleatoirement 
     * On prend une fichier au hasard soit dans /etc/ ou /usr/local ou
     * dans /var/log (<Tout est fichier ...>)
     * 1. Recuperer l'ID de chaque fichier et sa taille et sa position dans le cylindre en disque
     * 2. Auquels moment les blocs se mette en colision dans une disque dure
     */

    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stddef.h>
    #include <time.h>
    #include <stdbool.h>
    
    #define DEFAULT_BLOC_SIZE 4096
    #define LINE_MAX 1024
    #define CYLINDRE_NUMBER 512
    #define MAX_EXTENTS 100
    #define TMP_FILE "/tmp/.tmp.txt"
    
    extern char *directories[3];
    typedef struct {
        int extent_number;
        int logic_start;
        int logic_end;
        int physic_start;
        int physic_end;
        int length;
    } extent_t;//bloc
    typedef struct fileStructure
    {
        char name[255];
        size_t id;
        size_t filesize;
        extent_t position[CYLINDRE_NUMBER];
        int extendcount;
    }fileStructure;
    /// 1. Recuperer l'ID de chaque fichier et sa taille et sa position dans le cylindre en disque
        fileStructure recuperIdEtTailleEtPosition(bool generateHtml);
    
    //-------fonction utile dans la recuperation de taille , id et position---------//
            int getNombreLigne();
            int parse_filefrag_output(FILE *fp, extent_t extents[]);
            int genarateurAleatoire(int start,int end);
            char *getlinefile(int indexLine);
            void generate_html(fileStructure files); 
            fileStructure getdetails(char *fileRandom);
    
    /// 2. Auquels moment les blocs se mette en colision dans une disque dure
        void detect_collisions(fileStructure *files, int file_count);

        //----detect de chevauchement------//
        bool check_collision(extent_t *extent1, extent_t *extent2);
#endif