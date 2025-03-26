#include <stdio.h>
#include "fonction.h"
int main(){
    char* rep[]={"/etc","/var/log","/usr/local"};
    short index;
    unsigned long long int size;
    index=Index();
    //printf("%s\n",rep[index]);
    getID(rep,index);
    getsize(rep,index);
    getposition(rep,index);
    get_html_page();
    return 0;
}