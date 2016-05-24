/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012  Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 * 
 * contact the authors at: faro@dmi.unict.it, thierry.lecroq@univ-rouen.fr
 * download the tool at: http://www.dmi.unict.it/~faro/smart/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/*
 * This program compiles all c programs of string matching algorithms
 * It is called by makefile script, when compiling SMART
 */

int main(int argc, const char *argv[]) {
	char filename[100], command[100], binary[100];
	int i,j;
	char gcc[100] = "gcc source/algos/";
	char options[100] = " -O3 -msse4 -lm -o source/bin/"; 
	char destination[100] = "source/bin/";

	int doTest = 1;
	//if(argc>0)
	//	if(!strcmp(argv[1],"nt"))
	//		doTest = 0;
	
	DIR *d;
	FILE *stream;
	struct dirent *dir;

	//delete previous compiled files
	d = opendir("./source/bin");
	if(d) {
	  	while ((dir = readdir(d)) != NULL) {
			strcpy(filename, dir->d_name);
			sprintf(command,"./source/bin/%s",filename);
			remove(command);
		}
	}
	closedir(d);
	
	//compile c files
	d = opendir("./source/algos");
	if(d) {
	  	while ((dir = readdir(d)) != NULL) {
			strcpy(filename, dir->d_name);
			int len = strlen(filename);
			if(filename[len-1]=='c' && filename[len-2]=='.') {
				filename[len-2] = '\0';
				//compile
				sprintf(command,"%s%s.c%s%s",gcc,filename,options,filename);
				printf("\tCompiling file %s.c....",filename);
				for(i=0; i<10-strlen(filename); i++) printf(".");
				fflush(stdout);

				stream = freopen ("warning","w",stderr);  //redirect of stderr
				if( system(command)==1 ) printf("[ERROR]\n");
				else {
					//check if compiled file is present
					sprintf(binary,"%s%s",destination,filename);
					FILE *fp = fopen(binary,"r");
					if(fp) {
						printf("[OK]");
						fclose(fp);
						fflush(stdout);
						if(doTest) {
							//testing correctness of the algorithm
							sprintf(command,"./test %s -nv",filename);
							printf(" testing............");
							fflush(stdout);
							if(system(command)) {
								printf("\b\b\b\b\b\b\b\b[FAILED]");
								printf("\n");
							}
							else {
								printf("\b\b\b\b\b\b..[OK]");
								for(int j=0; j<63; j++) printf("\b");
							}
						}
					}
					else printf("[FAILED]\n");
					fflush(stdout);
				}
				fclose(stream);
			}
		}
	}
	closedir(d);
}

