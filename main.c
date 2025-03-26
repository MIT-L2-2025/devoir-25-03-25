#include <stdlib.h>
#include <stdio.h>
#include "Prototype.h"

int main(int argc,char *argv[])
{
    char* file = "etc";
    char *cmd = NULL;

    cmd = (char*) malloc ( 512 * sizeof(char));
    if (cmd == NULL)
    {
    printf("Error of allocation\n");
        exit(0);
    }
    else
    {
        //GetData
        file = GetNameOfFile();
        //ID
        sprintf(cmd,"ls -i -lh /%s/* 2>/dev/null | tr -s ' ' | cut -d' ' -f1 > ID.txt\n",file);
        system(cmd);
        
        //SIZE
        sprintf(cmd,"ls -i -lh /%s/* 2>/dev/null | tr -s ' ' | cut -d' ' -f6 > SIZE.txt\n",file);
        system(cmd);  
        
        //PPSITION
        sprintf(cmd,"filefrag -v /%s/* 2>/dev/null | awk '/: *[0-9]+.. *[0-9]+:/ {print $4}' > POSITION.txt\n",file);
        system(cmd);
        
        //DISLPAY
        display(file);
        system("xdg-open display.html");
        free(cmd);
    }   
    return (0);
}