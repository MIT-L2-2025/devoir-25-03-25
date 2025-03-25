#include "utl.h"


void get_fileInDoc_info(char *dirPath)
{
    FILE* file = fopen("result.html", "w");
    if (!file)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    

    DIR *dir = opendir(dirPath);
    if(!dir)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    struct stat st;
    char *path = (char *)malloc(sizeof(char) * 1024);  
    char *command = (char *)malloc(sizeof(char) * 1024);
    char *buffer = (char *)malloc(sizeof(char) * 1024);
    if(!path || !command || !buffer)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

//en tete de l'html
    fprintf(file, "<!DOCTYPE html\n");
    fprintf(file, "<html>\n");
    fprintf(file, "<head>\n");
    fprintf(file, "<title>File in Documents</title>\n");
    fprintf(file, "</head>\n");
//fin head
//css
    fprintf(file, "<style>");
    fprintf(file, "table {");
    fprintf(file, "width: 50%%;");
    fprintf(file, "border-collapse: collapse;");
    fprintf(file, "}");
    fprintf(file, "th, td {");
    fprintf(file, "border: 1px solid black;");
    fprintf(file, "padding: 8px;");
    fprintf(file, "text-align: center;");
    fprintf(file, "}");
    fprintf(file, "th {");
    fprintf(file, "background-color: lightgray;");
    fprintf(file, "}");
    fprintf(file, "</style>");
//fin css
    fprintf(file, "<body>\n");

    fprintf(file, "\n<table>");
    fprintf(file, "\n\n<tr>");
    fprintf(file, "\n\n\n<th>File Name</th>");
    fprintf(file, "\n\n\n<th>File Size</th>");
    fprintf(file, "\n\n\n<th>Block num</th>");
    fprintf(file, "\n\n\n<th>Id(inode)</th>");
    fprintf(file, "\n\n</tr>");

    while ((entry = readdir(dir)) != NULL)
    {
        //printf("%s\n", entry->d_name);
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        else
        {
            sprintf(path, "%s/%s", dirPath, entry->d_name);
            snprintf(command, 1024, "filefrag -v %s | awk 'NR==4 {print $4}' | cut -d '.' -f1", path);
            FILE *fp = popen(command, "r");
            if (fp == NULL) {
                perror("popen");
                exit(EXIT_FAILURE);
            }

            fgets(buffer, sizeof(buffer), fp);
            pclose(fp);

            //printf("%s\n", entry->d_name);
            if(stat(path, &st) == -1)
            {
                perror("stat");
                //exit(EXIT_FAILURE);
                continue;
            }
            if(S_ISDIR(st.st_mode))
            {
                continue;
            }

            fprintf(file, "\n\n<tr>");
            fprintf(file, "\n\n\n<td>%s</th>", entry->d_name);
            fprintf(file, "\n\n\n<td>%ld</th>", st.st_size);
            fprintf(file, "\n\n\n<td>%s</th>", buffer);
            fprintf(file, "\n\n\n<td>%ld</th>", st.st_ino);
            fprintf(file, "\n\n</tr>");
        }
    }

//fin de  l'html
    fprintf(file, "\n<table>");
    fprintf(file, "</body>\n");
    fprintf(file, "</html>\n");
}