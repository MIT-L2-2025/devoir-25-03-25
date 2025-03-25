#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <fcntl.h>
#include <linux/fs.h>

const char* directories[] = { "/etc/", "/var/log/", "/usr/local" };

void generate_html_report(const char* directory);
void process_file(const char* file_path, FILE* report);
void fetch_block_position(const char* file_path, int* block_pos);

int main(void) {
    srand(time(NULL));
    int dir_index = rand() % 3;
    generate_html_report(directories[dir_index]);
    return 0;
}

void generate_html_report(const char* directory) {
    DIR* dir = opendir(directory);
    if (!dir) {
        perror("Failed to access directory");
        exit(1);
    }

    FILE* report = fopen("result.html", "w");
    if (!report) {
        perror("Failed to create HTML file");
        exit(1);
    }

    fprintf(report, "<!DOCTYPE html>\n<html>\n<head>\n<title>Directory Content</title>\n</head>\n<body>\n");
    fprintf(report, "<h1>Content of %s</h1>\n", directory);
    fprintf(report, "<table border=\"1\">\n<tr><th>Name</th><th>ID</th><th>Size (bytes)</th><th>Block Position</th></tr>\n");

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char file_path[1024];
        snprintf(file_path, sizeof(file_path), "%s%s", directory, entry->d_name);
        process_file(file_path, report);
    }

    fprintf(report, "</table>\n</body>\n</html>\n");
    fclose(report);
    closedir(dir);
    printf("Report saved to result.html\n");
}

void process_file(const char* file_path, FILE* report) {
    struct stat file_stat;
    if (stat(file_path, &file_stat) == 0) {
        int block_pos = 0;
        if (S_ISREG(file_stat.st_mode)) {
            fetch_block_position(file_path, &block_pos);
        }
        fprintf(report, "<tr>");
        fprintf(report, "<td>%s</td>", file_path);
        fprintf(report, "<td>%ld</td>", file_stat.st_ino);
        fprintf(report, "<td>%ld</td>", file_stat.st_size);
        fprintf(report, "<td>%d</td>", block_pos);
        fprintf(report, "</tr>\n");
    } else {
        perror("Error with stat");
    }
}

void fetch_block_position(const char* file_path, int* block_pos) {
    int fd = open(file_path, O_RDONLY);
    if (fd != -1) {
        if (ioctl(fd, FIBMAP, block_pos) != 0) {
            *block_pos = 0;
        }
        close(fd);
    }
}
