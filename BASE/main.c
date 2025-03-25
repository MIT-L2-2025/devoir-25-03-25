#include "file_details.h"

void test_colision();
int main()
{
    srand(time(NULL));

    //test de recuperation details fichier aux hasard
    recuperIdEtTailleEtPosition(true);

    //test colision
    test_colision();
}
void test_colision()
{
    fileStructure listefichier[10];
    for (size_t i = 0; i < 10; i++)
    {
        listefichier[i] = recuperIdEtTailleEtPosition(false);
    }
    detect_collisions(listefichier,10);
}