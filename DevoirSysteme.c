#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

void printTable(FILE* file, char *filename, ino_t inode, off_t size, unsigned long disk_position)
{

	fprintf(file, 
		"				<tr>\n"
		"					<td>%s</td>\n"
		"					<td>%lu</td>\n"
		"					<td>%ld</td>\n"	
		"					<td>%lu</td>\n"
		"				</tr>\n", filename, inode, size, disk_position
	);
	
}

void getInfo(FILE* file, char *selectDirectories)
{
	
	struct dirent *entry;
    struct stat file_stat;
    DIR *dir = opendir(selectDirectories);
    if (dir == NULL) {
        perror("Erreur d'ouverture du répertoire");
        return;
    }
    
    while((entry = readdir(dir)) != NULL) 
    {
        char file_path[1024];
        snprintf(file_path, 1024, "%s/%s", selectDirectories, entry->d_name);
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (stat(file_path, &file_stat) == 0) 
        {
            unsigned long disk_position = file_stat.st_ino % 1024; 
            printTable(file, file_path, file_stat.st_ino, file_stat.st_size, disk_position);
        } else {
            perror("Erreur sur le fichier");
        }
    }
    
    closedir(dir);
	
}

void printHtml(char *selectDirectories)
{
	
	FILE *file = fopen("index.html", "w");

    if (file == NULL) {
        perror("Erreur d'ouverture du fichier");
        exit(1);
    }

    fprintf(file,	
		"<!DOCTYPE html>\n"
		"<html lang='en'>\n"
		"	<head>\n"
		"		<meta charset='UTF-8'>\n"
		"		<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"	
		"		<title>Document</title>\n"
		"		<link rel='stylesheet' type='text/css' href='style.css'>\n"
		"	</head>\n"
		"	<body>\n"    
		"		<table border='1'>\n"   
		"			<caption>Répertoire : %s</caption>\n" 
		"			<tr>\n"
		"				<th>Fichier</th>\n"
		"				<th>ID du fichier</th>\n"
		"				<th>Taille (octets)</th>\n"
		"				<th>Position dans le cylindre</th>\n"
		"			</tr>\n", selectDirectories
		
	);
	
	getInfo(file, selectDirectories);
	
	fprintf(file,	
		"		</table>\n"    
		"	</body>\n"
		"</html>\n"
    );

    fclose(file);

}

int main()
{

	srand(time(NULL));
	char *directories[] = {"/etc", "/usr/local", "/var/log"};
	char *selectDirectories = directories[rand() % 3];
	
	printf("%s", selectDirectories);
	
	printHtml(selectDirectories);

	return 0;
}
