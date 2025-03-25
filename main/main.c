#include "utl.h"
#include <time.h>

int main()
{
    srand(time(NULL));
    switch (rand() % 3)
    {
    case 0:
        get_fileInDoc_info("/etc/");
        break;
    case 1:
        get_fileInDoc_info("/usr/local/");
        break;
    case 2:
        get_fileInDoc_info("/var/log/");
        break;
    default:
        break;
    }
    //get_fileInDoc_info("/var/log/");

    return 0;
}