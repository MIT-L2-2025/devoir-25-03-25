#include <time.h>
#include <dirent.h> 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef LIKELY_POSSIBLE_DIRECTORY

enum RANDOM_CHOICE_DIRECTORY {
    ETC_DIRECTORY, 
    USR_LOCAL_DIRECTORY,
    VAR_LOG_DIRECTORY
};

#endif 

#ifdef __linux__

void headers(FILE* html, char* dirname) {
    fprintf(html, "\
        <!DOCTYPE html>\n\
        <html lang=\"en\">\n\
        <head>\n\
            <meta charset=\"UTF-8\">\n\
            <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
            <meta name=\"description\">\n\
            <title>Content</title>\n\
            <style>\n\
                table {\n\
                    width: 100%;\n\
                    border-collapse: collapse;\n\
                }\n\
                th, td {\n\
                    border: 1px solid black;\n\
                    padding: 8px;\n\
                    text-align: left;\n\
                }\n\
                th {\n\
                    background-color: #f2f2f2;\n\
                }\n\
                hr {\n\
                    border: none;\n\
                    border-top: 2px solid black;\n\
                    margin: 10px 0;\n\
                }\n\
                </style>\n\
        </head>\n\
        <body>\n\
        <h1>\n\
            %s \
        </h1>\n\
        <table>\n\
            <thead>\n\
                <th> File </th>\n\
                <th> Inode </th>\n\
                <th> Blocks allocated </th>\n\
                <th> Size </th>\n\    
            </thead>\n\ 
            <tbody>", dirname);
}

void footer(FILE* html) {
    fprintf(html,
        "       </tbody>\n\
            </table>\n\
        </body>\n\
        </html>"
    );
}

char* human_readable_size(off_t size) {
    char *units[] = {"B", "KB", "MB", "GB", "TB"};
    char *final_result;
    int unit_index = 0;

    final_result = malloc(1024 * sizeof(char));
    if (!final_result)
        exit(ENOMEM);

    while (size >= 1024 && unit_index < 4) {
        size /= 1024;
        unit_index++;
    }

    sprintf(final_result, "%ld %s", size, units[unit_index]);
    return final_result;
}

long get_directory_size(char *dirname) {
    DIR *dir = opendir(dirname);
    if (!dir) {
        exit(EBADE);
    }

    struct dirent *entry;
    struct stat entry_stat;
    long total_size = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dirname, entry->d_name);

        if (stat(full_path, &entry_stat) == 0) {
            if (S_ISDIR(entry_stat.st_mode)) {
                total_size += get_directory_size(full_path);
            } 
            else if (S_ISREG(entry_stat.st_mode)) {
                total_size += entry_stat.st_size;
            }
        }
    }

    closedir(dir);
    return total_size;
}

char* rand_directory()
{
    int status;
    short rand_dir;
    
    srand(time(NULL));
    rand_dir = rand() % 3;
    switch(rand_dir)
    {
        case ETC_DIRECTORY: 
            return "/etc"; 
        break;
        case USR_LOCAL_DIRECTORY:        
            return "/usr/local";
        break;
        case VAR_LOG_DIRECTORY: 
            return "/var/log";
        break;
    }
}

int main()
{
    FILE* html;
    char *dirname = rand_directory();
    struct dirent* entry; 
    struct stat fstat;
    
    html = fopen("index.html", "w");
    if (!html) {
        return EBADF;
    }

    headers(html, dirname);
    DIR* dir = opendir(dirname);
    if (!dir) {
        return ENOENT;
    }

#ifndef PATH_MAX 
#define PATH_MAX 0x0F00
            
#endif
    while((entry = readdir(dir)) != NULL) {
        char filepath[PATH_MAX];
        snprintf(filepath, sizeof(filepath), "%s/%s", dirname, entry->d_name);
        if (stat(filepath, &fstat) == 0) {
            if (S_ISREG(fstat.st_mode)) {
                char file_logic_block[8196];
                char get_file_logic_block[8196];
                FILE *fp;

                snprintf(get_file_logic_block, sizeof(file_logic_block), "filefrag -v %s | tail -n 2 | head -n 1| awk -F':' '{print $3}'", filepath);
                fp = popen(get_file_logic_block, "r");
                if (!fp) {
                    return EBADF;
                }

                while(fgets(file_logic_block, sizeof(file_logic_block), fp) !=NULL) {

                }

                fprintf(html, " <tr><td> (F) %s </td><td> %lu </td><td> %s </td><td> %s </td></tr>\n", entry->d_name, fstat.st_ino, file_logic_block, human_readable_size(fstat.st_size));
            }

            else if (S_ISDIR(fstat.st_mode)) {
                fprintf(html, "<tr><td style=\"color: blue;\"> (D) %s </td><td> %lu </td><td> %p </td><td> %ld </td></tr>\n", entry->d_name, fstat.st_ino, NULL, get_directory_size(filepath));
            }
        }
    }

    footer(html);
    closedir(dir);
    return EXIT_SUCCESS;
}

#endif