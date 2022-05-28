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

int main(int argc, char **argv) {
	char filename[100], command[512], binary[100];
	int i,j;
	char gcc[100] = "gcc source/algos/";
	char options[100] = " -O3 -msse4 -lm -o source/bin/"; 
	char destination[100] = "source/bin/";

	int doTest = 0;
    if(argc>1) {
		if(!strcmp(argv[1],"dotest"))
			doTest = 1;
    }
	
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
	
    //counting c files
    d = opendir("./source/algos");
    int n_algo = 0;
    if(d) {
        while ((dir = readdir(d)) != NULL) {
            strcpy(filename, dir->d_name);
            int len = strlen(filename);
            if(filename[len-1]=='c' && filename[len-2]=='.') {
                n_algo ++;
            }
        }
    }
    closedir(d);
    
    int current = 0;
    int compiling_error = 0, testing_error= 0;
    //compile c files
	d = opendir("./source/algos");
	if(d) {
	  	while ((dir = readdir(d)) != NULL) {
			strcpy(filename, dir->d_name);
			int len = strlen(filename);
			if(filename[len-1]=='c' && filename[len-2]=='.') {
				filename[len-2] = '\0';
                current++;
				//compile
				sprintf(command,"%s%s.c%s%s",gcc,filename,options,filename);
                if(doTest) printf("\tCompiling and testing %s.c",filename);
                else       printf("\tCompiling %s.c ...........",filename);
				for(i=0; i<15-strlen(filename); i++) printf(".");
                printf("(%.3d/%.3d) [000%%]",current,n_algo);
				fflush(stdout);

                printf("\b\b\b\b\b\b[%.3d%%]",current*100/n_algo); fflush(stdout);

				stream = freopen ("warning","w",stderr);  //redirect of stderr
				if( system(command)==1 ) printf("[ERROR]\n");
				else {
					//check if compiled file is present
					sprintf(binary,"%s%s",destination,filename);
					FILE *fp = fopen(binary,"r");
					if(fp) {
						fclose(fp);
						fflush(stdout);
                        int failed = 0;
						if(doTest) {
							//testing correctness of the algorithm
							sprintf(command,"./test %s -nv",filename);
							//printf("\b\b\b\b\b[000%]");
							fflush(stdout);
							if(system(command)) {
                                testing_error++;
                                failed = 1;
								printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b....[TEST FAILED]");
								printf("\n");
							}
						}
                        if(!failed) {
                            printf("\b\b\b\b\b\b..[OK]");
                            for(int j=0; j<63; j++) printf("\b");
                        }
					}
                    else {
                        printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b.[COMPILING ERROR]\n");
                        compiling_error++;
                    }
					fflush(stdout);
				}
				fclose(stream);
			}
		}
	}
	closedir(d);
    for(i=0; i<33; i++) printf("\b");
    printf("\tAll algorithms have been compiled and tested.......\n");
    printf("\tCompiling errors .................................[%.3d]\n", compiling_error);
    if(doTest) printf("\tTesting errors ...................................[%.3d]\n\n", testing_error);

}

