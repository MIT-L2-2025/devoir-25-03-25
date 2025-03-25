#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../fonctions/fonctions.h"

int main() {
    int file_size;
    unsigned long lba, chr;
    char random_dir[1024];

    generateRandomFile(random_dir);
    runFilefrag(random_dir);

    if (getFileSize("info.txt", &file_size) == 0) {
        printf("Taille: %d\n", file_size);
    } else {
        printf("Erreur lors de la récupération de la taille\n");
    }

    if (getFileLBAAndCHR("info.txt", &lba, &chr) == 0) {
        printf("LBA: %lu\nCHR: %lu\n", lba, chr);
    } else {
        printf("Erreur lors de la récupération du LBA et CHR\n");
    }

    return 0;
}
