#include "RandomDirectory.h"

int GetRandomNumber()
{
    srand(time(NULL));
    return (rand() % 3);
}
char *RandomDirectory()
{
    char *Directory[] = {"/etc/", "/usr/local/", "/var/log/"};
    int index = GetRandomNumber();
    return (Directory[index]);
}
void PrintFromDirectory()
{
    const char *path = RandomDirectory();
    DIR *dir = opendir(path);
    FILE *file = fopen("file_info.html", "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier HTML");
        exit(1);
    }
    if (dir == NULL)
    {
        perror("Erreur lors de l'ouverture du répertoire\n");
        exit(1);
    }
    struct  dirent *entry;
    HeadHtml(file);
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] != '.')
        {
            PrintInfo(file,entry->d_name ,path);
        }
    }
    TailHtml(file);
    fclose(file);
    closedir(dir);
}
void PrintInfo(FILE *file, char *path, const char *loadpath)
{
    struct stat stat_buf;
    char *Path = (char *)malloc(50);
    int __Ino, __Taille, __Blocksbegin, __Blocksend, blocFin;
    Disque disque;
    GetGeoDisque(&disque);
    strcpy(Path, loadpath);
    strcat(Path, path);
    if (stat(Path, &stat_buf) == -1) {
        perror("Erreur lors de l'obtention des informations sur le fichier");
        exit(1);
    }
    __Ino = stat_buf.st_ino; 
    __Taille = stat_buf.st_size;
    __Blocksbegin = (__Taille + 512 - 1) / 512;
    __Blocksend = __Ino % 10000;
    blocFin = __Blocksend + __Blocksbegin - 1;
    fprintf(file, "\t\t\t<tr>\n");
    fprintf(file, "\t\t\t<td>%s</td>\n", Path);
    fprintf(file, "\t\t\t<td>%ld</td>\n", (long)__Ino);
    fprintf(file, "\t\t\t<td>%ld</td>\n", (long)__Taille);
    fprintf(file, "\t\t\t<td>%d</td>\n", __Blocksend);
    fprintf(file, "\t\t\t<td>%d</td>\n", blocFin);
    fprintf(file, "\t\t\t<td>%s</td>\n", (__Blocksbegin > 1) ? "oui":"non");
    fprintf(file, "\t\t\t</tr>\n");
}
void HeadHtml(FILE *file)
{
    fprintf(file, "<html>\n");
    fprintf(file, "\t<head>\n");
    fprintf(file, "\t\t<title>FILE DESCRIPTION</title>\n");
    fprintf(file, "\t</head>\n");
    fprintf(file, "\t<body>\n");
    fprintf(file, "\t\t<table border=\"1\">\n");
    fprintf(file, "\t\t<tr>\n");
    fprintf(file, "\t\t\t<th>Chemin</th>\n");
    fprintf(file, "\t\t\t<th>__Inode</th>\n");
    fprintf(file, "\t\t\t<th>__Taille (octets)</th>\n");
    fprintf(file, "\t\t\t<th>Bloc de debut</th>\n");
    fprintf(file, "\t\t\t<th>Bloc de fin</th>\n");
    fprintf(file, "\t\t\t<th>Bloc de croisement</th>\n");
    fprintf(file, "\t\t\t</tr>\n");
}
void TailHtml(FILE *file)
{
    fprintf(file, "\t\t</table>\n");

    fprintf(file, "\t</body>\n");
    fprintf(file, "</html>\n");
}
void GetGeoDisque(Disque *disque)
{
    int __disque;
    struct hd_geometry disk_geometry;
    __disque = open("/dev/sda", O_RDONLY);
    if (__disque == -1)
        exit(1);
    if (ioctl(__disque, HDIO_GETGEO, &disk_geometry) == -1)
    {
        close(__disque);
        exit(1);
    }
    disque->cylindre = disk_geometry.cylinders;
    disque->tete = disk_geometry.heads;
    disque->secteur = disk_geometry.sectors;
    close(__disque);
}