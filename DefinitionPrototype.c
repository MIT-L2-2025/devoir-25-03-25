#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "Prototype.h"

void display(char chaine[30])
{
    FILE *file1,*file2,*file3,*file4;
    char *tmp1,*tmp2,*tmp3;

    //Initialization
    tmp1 = NULL;
    tmp2 = NULL;
    tmp3 = NULL;
    file1 = fopen("ID.txt","r");
    file2 = fopen("SIZE.txt","r");
    file3 = fopen("POSITION.txt","r");
    file4 = fopen("display.html","w");

    if (file1 == NULL || file2 == NULL  || file3 == NULL || file4 == NULL)
    {
        printf("Error of open file\n");
        exit(0);
    }
    else
    {
        fprintf(file4,"<HTML>\n");
		fprintf(file4," <HEAD>\n");
		fprintf(file4,"	    <TITLE>\n");
        fprintf(file4,"         Display od result of /%s\n",chaine);
		fprintf(file4,"	    </TITLE>\n");
		fprintf(file4,"	</HEAD>\n");
		fprintf(file4,"	<BODY>\n");
        fprintf(file4, "    <TABLE border=\"1\">\n");
        fprintf(file4,"	        <TR>\n");
        fprintf(file4,"	            <TD> ID </TD>\n");
        fprintf(file4,"	            <TD> SIZE </TD>\n");
        fprintf(file4,"	            <TD> POSITION </TD>\n");
		fprintf(file4,"	        </TR>\n");
        tmp1 = (char*) malloc ( 256 * sizeof(char));
        tmp2 = (char*) malloc ( 256 * sizeof(char));
        tmp3 = (char*) malloc ( 256 * sizeof(char));
        if (tmp1 == NULL || tmp2 == NULL || tmp3 == NULL)
        {
            printf("Error of allocation\n");
            exit(0);
        }
        else
        {
            while (fgets(tmp1, 255, file1) != NULL && fgets(tmp2, 255, file2) != NULL && fgets(tmp3, 255, file3) != NULL) 
            {
                tmp1[strcspn(tmp1, "\n")] = '\0';
                tmp2[strcspn(tmp2, "\n")] = '\0';
                tmp3[strcspn(tmp3, "\n")] = '\0';
        
                fprintf(file4, "        <TR>\n");
                fprintf(file4, "            <TD>%s</TD>\n", tmp1);
                fprintf(file4, "            <TD>%s</TD>\n", tmp2);
                fprintf(file4, "            <TD>%s</TD>\n", tmp3);
                fprintf(file4, "        </TR>\n");
            }
        }
		fprintf(file4,"	    </TABLE>\n");
		fprintf(file4,"	</BODY>\n");
		fprintf(file4,"</HTML>\n");

        free(tmp3);
        free(tmp2);
        free(tmp1);
    }
    fclose(file4);
    fclose(file3);
    fclose(file2);
    fclose(file1);
}

char* GetNameOfFile()
{
    char **TabData = NULL;
    char *tmp = NULL;
    FILE *file = NULL;
    int i = 0,NombreAleatoire = 0;

    system ("ls / > tmp.txt");

    file = fopen("tmp.txt","r");
    if (file == NULL)
    {
        printf("Error of oppen file\n");
        exit(0);
    }
    tmp = (char*) malloc ( 256 * sizeof(char));
    if (tmp == NULL)
    {
        printf("Error of allocation\n");
        exit(0);
    }
    TabData = (char**) malloc ( 30 * sizeof(char*));
    for (i = 0;i < 30 ; i++)
    {
        TabData[i] = (char*) malloc ( 256 * sizeof(char));
    }
    i = 0;
    while (fgets(tmp, 255, file) != NULL)
    {   
        tmp[strcspn(tmp, "\n")] = '\0';
        strcpy(TabData[i],tmp);
        i++;
    }
    fclose(file);
    srand(time(NULL));
    do
    {
        NombreAleatoire = rand() % (14 - 0 + 1) + 0;
    } 
    while (NombreAleatoire > 14 || NombreAleatoire < 0);
    char *NameOfFile = strdup(TabData[NombreAleatoire]);
    for (i = 0;i < 30 ; i++)
    {
        free(TabData[i]);
    }
    free(TabData);
    free(tmp);
    return NameOfFile;
}