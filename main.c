#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define MAX_CMD 512
#define MAX_FILE 255

char *randomDir();
void fetchPrintInfo();
int main()
{
	fetchPrintInfo();
	return 0;
}
char *randomDir(){
	int choice=rand() % 3;
	switch (choice)
	{
		case 0:
			return "/etc";
			break;
		case 1:
			return "/usr/local";
			break;
		case 2:
			return "/var/log";
			break;
	}
	return "/etc";
}
void fetchPrintInfo()
{
	struct stat info;
	FILE *allfile, *html, *block;
	srand(time(NULL));
	char command[MAX_CMD], file[MAX_FILE], blockcmd[MAX_CMD], block_output[50];
	sprintf(command, "find %s -maxdepth 1 -type f\n", randomDir());	//detection des fichier
	if((allfile=popen(command ,"r"))==NULL)
	{
		perror("allfile: popen");
		exit(EXIT_FAILURE);
	}
	if((html=fopen("index.html", "w"))==NULL)
	{
		perror("html : fopen");
		pclose(allfile);
		exit(EXIT_FAILURE);
	}
	fprintf(html, "<!DOCTYPE html>\n");
	fprintf(html, "<html lang=\"en\">\n");
	fprintf(html, "<head>");
	fprintf(html, "    <meta charset=\"UTF-8\">\n");
	fprintf(html, "    <title>title</title>");
	fprintf(html, "</head>\n");
	fprintf(html, "<body>\n");
	fprintf(html, "    <table border=\"3\">");
	fprintf(html, "            <th>Filename</th>\n");
	fprintf(html, "            <th>Inode</th>\n");
	fprintf(html, "            <th>Size</th>\n");
	fprintf(html, "            <th>Index</th>\n");
	while(fgets(file, MAX_FILE, allfile))
	{
		file[strcspn(file, "\n")]='\0';
		if(stat(file, &info)!=0)
		{
			perror("stat");
			continue;
		}
        snprintf(blockcmd,MAX_CMD,  "filefrag -v \"%s\" | awk '{print $4}' | tail -n 2 | head -n 1", file);
        if((block=popen(blockcmd, "r"))!=NULL)
        {
        	if (fgets(block_output, 50, block)==NULL)
        	{
        		snprintf(block_output, 50, "N/A");
        	}
        }

		fprintf(html, "        <tr>");
		fprintf(html, "            <td>%s</td>\n", file);
		fprintf(html, "            <td>%ld</td>\n", (long)info.st_ino);
		fprintf(html, "            <td>%ld</td>\n",(long)info.st_size);
		fprintf(html, "            <td>%s</td>\n", block_output);
		fprintf(html, "       </tr>\n");
		pclose(block);
	}
	fprintf(html, "    </table>\n");
	fprintf(html, "</body>\n");
	fprintf(html, "</html>\n");
	fclose(html);
}
