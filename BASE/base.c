#include "file_details.h"

char *directories[3] = {
    "/etc/",
    "/var/log/",
    "/usr/local/"
};
void generate_html(fileStructure files) {
    FILE *html_file = fopen("files_info.html", "w");
    if (!html_file) {
        perror("Unable to open file for writing");
        return;
    }
    printf("ecriture de l'HTML...\n");
    fprintf(html_file, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(html_file, "<meta charset=\"UTF-8\">\n");
    fprintf(html_file, "<title>Informations sur le fichier</title>\n");
    fprintf(html_file, "<style>\n");
    fprintf(html_file, "body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; text-align: center; }\n");
    fprintf(html_file, "h1 { color: #003366; }\n");
    fprintf(html_file, "table { width: 80%%; margin: 20px auto; border-collapse: collapse; background: white; }\n");
    fprintf(html_file, "th, td { border: 1px solid #ccc; padding: 10px; }\n");
    fprintf(html_file, "th { background: #003366; color: white; }\n");
    fprintf(html_file, "tr:nth-child(even) { background: #f2f2f2; }\n");
    fprintf(html_file, "</style>\n</head>\n<body>\n");

    fprintf(html_file, "<h1>Informations sur le fichier</h1>\n");
    fprintf(html_file, "<table>\n<tr><th>Nom</th><th>ID</th><th>Taille</th><th>Details des bloc-contigus(extent)</th></tr>\n");
    fprintf(html_file, "<tr><td>%s</td><td>%zu</td><td>%zu octets</td>\n<td>\n", files.name, files.id, files.filesize);

    fprintf(html_file, "<table>\n<tr><th>Extent</th><th>Debut logique</th><th>Fin logique</th><th>Debut physique</th><th>Fin physique</th><th>Longueur</th></tr>\n");
    for (int i = 0; i < files.extendcount; i++) {
        fprintf(html_file, "<tr><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\n",
                files.position[i].extent_number, files.position[i].logic_start, files.position[i].logic_end,
                files.position[i].physic_start, files.position[i].physic_end, files.position[i].length);
    }
    fprintf(html_file, "</table></td></tr>\n</table>\n</body>\n</html>\n");

    fclose(html_file);
    printf("HTML genere avec succes.\n");
}
fileStructure recuperIdEtTailleEtPosition(bool generateHtml) {
    int choice_dir = genarateurAleatoire(0, 2);
    char *RandDim = directories[choice_dir];

    char findCommand[DEFAULT_BLOC_SIZE];
    snprintf(findCommand, sizeof(findCommand), "find %s > %s", RandDim, TMP_FILE);
    system(findCommand);
    
    int numberline = getNombreLigne();
    if (numberline <= 0)
        exit(EXIT_FAILURE);

    int RandFile = genarateurAleatoire(0, numberline);
    char *fileRandom = getlinefile(RandFile);
    fileStructure FileRand = getdetails(fileRandom);
    if(generateHtml)
        generate_html(FileRand);
    free(fileRandom);
    return FileRand;
}
int genarateurAleatoire(int start, int end) {
    return (rand() % (end - start + 1)) + start;
}
fileStructure getdetails(char *fileRandom) {
    char temp[LINE_MAX];
    char line[LINE_MAX];
    char command[LINE_MAX];
    char lsCommand[LINE_MAX];
    int tempdl;
    fileStructure flstr;

    snprintf(lsCommand, sizeof(lsCommand), "ls -il %s", fileRandom);

    FILE *fp = popen(lsCommand, "r");
    if (!fp) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fgets(line, sizeof(line), fp);

     //Si c'est une repertoire on regarde ses info mais pas ses sous-dossier
    if(strstr(line,"total") != NULL)
    {
        char *name = strrchr(fileRandom,'/');
        name++;

        snprintf(lsCommand, sizeof(lsCommand), "ls -il %s/.. | grep \"%s\"", fileRandom,name,name);

        fgets(line, sizeof(line), fp);

    }
    //1177354 -rw-rw-r-- 1 iavo iavo 6305 Mar 25 20:26 base.c
    sscanf(line, "%zu %s %d %s %s %zu %s %s %s %s", &flstr.id, temp, &tempdl, temp, temp, &flstr.filesize, temp, temp,temp, flstr.name);
    pclose(fp);

    //utilisation du commande filefrag pour information sur la fragmentation du fichier
    snprintf(command, sizeof(command), "filefrag -v %s", fileRandom);
    
    FILE *fpi = popen(command, "r");
    if (!fpi) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    //on recupere les extend ou les bloc contigu
    flstr.extendcount = parse_filefrag_output(fpi, flstr.position);
    pclose(fpi);
    return flstr;
}
int getNombreLigne() {
    int number = 0;
    char line[256];
    FILE *file = fopen(TMP_FILE, "r");
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        return -1;
    }
    while (fgets(line, sizeof(line), file)) number++;
    fclose(file);
    return number;
}
char *getlinefile(int indexLine) {
    int number = 0;
    char *line = malloc(LINE_MAX);
    FILE *file = fopen(TMP_FILE, "r");
    if (!file) return NULL;
    
    while (fgets(line, LINE_MAX, file)) {
        number++;
        if (number == indexLine) {
            fclose(file);
            return line;
        }
    }
    fclose(file);
    free(line);
    return NULL;
}
int parse_filefrag_output(FILE *fp, extent_t extents[])
{
    char line[256];
    int extent_count = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        int extent_number, logic_start, logic_end;
        int physic_start, physic_end, length;

        if (sscanf(line, "%d:        %d..       %d:    %d..   %d:      %d:", 
                   &extent_number, &logic_start, &logic_end,
                   &physic_start, &physic_end, &length) == 6) {
            if (extent_count < MAX_EXTENTS) {
                extents[extent_count].extent_number = extent_number;
                extents[extent_count].logic_start = logic_start;
                extents[extent_count].logic_end = logic_end;
                extents[extent_count].physic_start = physic_start;
                extents[extent_count].physic_end = physic_end;
                extents[extent_count].length = length;
                extent_count++;
            } else {
                break;  
            }
        }
    }
    return extent_count;
}

bool check_collision(extent_t *extent1, extent_t *extent2) {
    bool collision_physique = (extent1->physic_start < extent2->physic_end &&
                               extent1->physic_end > extent2->physic_start);
    bool collision_logique = (extent1->logic_start < extent2->logic_end &&
                              extent1->logic_end > extent2->logic_start);

    return collision_physique || collision_logique;
}
void detect_collisions(fileStructure *files, int file_count) {
    printf("\n\nVerification des collisions...\n");
    bool have_colisionv = false;
    for (int i = 0; i < file_count; i++) {
        fileStructure *current_file = &files[i];
        printf("Analyse du fichier %s (ID: %zu)\n", current_file->name, current_file->id);
        // Verification des collisions pour chaque paire d'etendues dans ce fichier
        for (int j = 0; j < current_file->extendcount; j++) {
            for (int k = j + 1; k < current_file->extendcount; k++) {
                if (check_collision(&current_file->position[j], &current_file->position[k])) {
                    have_colisionv = true;
                    printf("Collision detectee dans le fichier %s entre l'extension %d et l'extension %d\n",
                           current_file->name, current_file->position[j].extent_number, current_file->position[k].extent_number);
                }
            }
        }
    }
    if (!have_colisionv)
        printf("Pas de colision\n");   
}
