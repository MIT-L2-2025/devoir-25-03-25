#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

void initialiseHTML(FILE* f, char* dirName);
void writeToHTML(FILE *f, char* filepath, long ID, size_t size, char* pos);
void endHTML(FILE* f);

int main()
{
    srand(time(NULL));

    char *directoryPath = ".";

    char filePath [1024];
    char line[2048];
    char command[2048];

    struct dirent *entry;
    struct stat file_stat;

    FILE *html;
    FILE *fp;

    int randomChoice =1 + rand() % 3;

    html = fopen("info.html", "w");
    if (!html)
        return EXIT_FAILURE;

    switch (randomChoice)
    {
        case 1:
                directoryPath = "/etc";
            break;
        case 2:
                directoryPath = "/usr/local";
            break;
        case 3:
                directoryPath = "/var/log";
            break;
        default:
            break;
    }

    DIR *dir = opendir(directoryPath);
    initialiseHTML(html, directoryPath);

    if (dir == NULL)
    {
        perror("opendir");
        return EXIT_FAILURE;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry -> d_name);
        if (stat(filePath, &file_stat) == -1)
            continue;
        if (S_ISREG(file_stat.st_mode))
        {
            // To search for the number where the file is located within the physical drive
            snprintf(command, sizeof(command), "filefrag -v %s | tail -2 | head -1 | awk -F':' '{print $3}'", filePath);

            // open a pipe and take the output of the command filefrag -v ...
            fp =  popen (command, "r");
            if (!fp)
                return EXIT_FAILURE;
            // Read the output line by line
	    char* token = strtok(line, " ");
            while (fgets(line, sizeof(line), fp) != NULL)
	    {
		token = strtok(NULL, " ");
	    }
            strcat(filePath, " ( File )");
            writeToHTML(html, filePath, file_stat.st_ino,file_stat.st_size, token);
        }

        else if (S_ISDIR(file_stat.st_mode))
        {
            strcat(filePath, " ( Dir )");
            writeToHTML(html, filePath, file_stat.st_ino, file_stat.st_size, NULL);
        }
    }

    closedir(dir);
    endHTML(html);
    fclose (html);
    return EXIT_SUCCESS;
}

void initialiseHTML(FILE *f, char* dirName)
{
    fprintf(f, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n");
    fprintf(f, "<style>"
    "table {border-collapse: collapse;"
    "th, td {border: 1px solid black;"
      "padding: 8px;"
      "text-align: left;}"
    "th {background-color: #f2f2f2;}"
  "</style> ");
    fprintf(f, "<title>Info</title>\n</head>\n");
    fprintf(f, "<body>\t<h1>Content of the directory: %s</h1><table>\n", dirName);
    fprintf(f, "<TR><TH>FILE</TH><TH>INODE</TH><TH>SIZE</TH><TH>BLOCKS</TH></TR>\n");
}

void endHTML(FILE *f)
{
    fprintf(f, "</table>\n</body>\n</html>\n");
}

void writeToHTML(FILE *f, char* filepath, long inode, size_t size, char* pos) {
    fprintf(f, "<TR><TD>%s</TD><TD>%ld</TD><TD>%ld</TD><TD>%s</TD></TR>\n", filepath, inode, size, pos);
}
