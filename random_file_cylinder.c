#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

typedef struct random_file_cylinder
{
    char id[256];
    char size[256];  
    char filename[1024];
    char block[128];
} rfc;

int main()
{
    char directory[3][128] = 
    {
        "/etc",
        "/usr/local",
        "/var/log"
    };

    char file_o[64] = "output.txt";
    char dir[256];
    int i, j;
    rfc *f;

    srand(time(NULL));
    int random = rand() % 3;
        switch(random)
        {
            case 0:
                sprintf(dir , "ls -il %s| awk '{print $1, $6, $10}' > %s ", directory[random],file_o);
                system(dir);

                break;

            case 1:
                sprintf(dir , "ls -il %s| awk '{print $1, $6, $10}' > %s ", directory[random],file_o);
                system(dir);

                break;
            
            case 2:
                sprintf(dir , "ls -il %s| awk '{print $1, $6, $10}' > %s ", directory[random],file_o);
                system(dir);

                break;
        }

        printf("%s\n", directory[random]);


        FILE *file;
        file = fopen(file_o, "r");
        if(!file)
        {
            printf("can't open file %s", file_o);
            exit(EXIT_SUCCESS);
        }
        FILE *html;
        html = fopen("cylinder.html", "w+");
        if(!html)
        {
            printf("can't open file html");
            exit(EXIT_SUCCESS);
        }

        char string[1024];
        char id[256];
        char size[256];

        int incr = 0;
        char str[1024];
        char ch_count;

        while (fgets(str, 256, file))
        {
            incr++;
        }
        rewind(file);

        f = (rfc*)malloc(sizeof(rfc)*incr);
        for (i = 1; i < incr; i++)
        {
            fscanf( file, "%s %s %s", f[i].filename, f[i].id, f[i].size );
        }


        char *block_file = "block.txt";

        FILE *blockn;
        blockn = fopen(block_file, "w+");
        if(!blockn)
        {
            printf("can't open file %s", block_file);
            exit(EXIT_SUCCESS);
        }

        char blk[2048];
            sprintf(blk, "stat %s/* | grep Blocks |awk '{print$4}' > %s",directory[random], block_file);
            system(blk);

        for (i = 1; i < incr; i++)
        {
            fscanf(blockn, "%s", f[i].block);
            printf("%s\n", f[i].block);
        }

        fprintf(html ," <html>\n");
        fprintf(html ," <head>\n");
        fprintf(html ," <title> Random file");
        fprintf(html ," </title>\n");
        fprintf(html ," </head>\n");
        fprintf(html ," <body>\n");

        fprintf(html, "<table border = 1 >\n");
            fprintf(html ," <tr>\n");
            fprintf(html ," <td>nom du fichier</td>");
            fprintf(html ," <td>id </td>");
            fprintf(html ," <td> taille</td>");
            fprintf(html ," <td> bloc</td>");
            fprintf(html ," </tr>\n");

            for(j = 1; j < incr ;j++)
            {
                fprintf(html ," <tr>\n");
                fprintf(html ," <td>%s</td>", f[j].filename);
                fprintf(html ," <td>%s</td>", f[j].id);
                fprintf(html ," <td>%s</td>", f[j].size);
                fprintf(html ," <td>%s</td>", f[j].block);
                fprintf(html ," </tr>\n");
            }

        fprintf(html, "<table>\n");
        fprintf(html ," </body>");
        fprintf(html ," </html>");
    fclose(file);
    fclose(html);
    return 0;
}