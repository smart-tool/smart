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
#define NumAlgo     500		 		//numero di algoritmi da testare
#define NumPatt     17				//maximal number of pattern lengths
#define NumSetting	12				//number of text buffers
#include "function.h"

/***********************************************/
/* SELECTS ALGORITHMS FOR EXPERIMENATL RESULTS */
/***********************************************/


void printManual() {
	printf("\n\tSMART UTILITY FOR SELECTING STRING MATCHING ALGORITHMS\n\n");
	printf("\t-show           shows the list of all algorithms\n");
	printf("\t-which          shows the list of all selected algorithms\n");
	printf("\tALGO            selects/deselects the algorithm ALGO (ex. select bf)\n");
	printf("\t-all            selects all algorithms\n");
	printf("\t-none           deselects all algorithms\n");
	//printf("\t-group A-B-C:D  group a list of algorithms (separated by a -) in a single algorithm named D\n");
	printf("\t-add ALGO       add the new alorithm ALGO to the set\n");
	printf("\t                the executable file of the new algorithm must be in /source/bin\n");
	printf("\t-h              gives this help list\n");
	printf("\n\n");
}

int search_ALGO(char *ALGO_NAME[], char* algo)  {
	int i;
	for(i=0; i<NumAlgo; i++)
		if(ALGO_NAME[i] && !strcmp(str2lower(ALGO_NAME[i]),str2lower(algo))) return i;
	return -1;
}



int main(int argc, const char *argv[]) {
	int EXECUTE[NumAlgo];
	char *ALGO_NAME[NumAlgo];
	char *ALGO_DESCRIPTION[NumAlgo];
	char *PATH[NumAlgo];
   	char filename[20], command[100]; 
	int i,j; 

	getAlgo(ALGO_NAME,EXECUTE);

	/* processing of input parameters */
	if (argc==1) {printf("\n\tNo parameter given. Use -h for help.\n\n"); return 0;}
	if (!strcmp("-h", argv[1])) {printManual(); return 0;}
	int par = 1;
	while(par<argc) {
		int selected = 0;
		if (par<argc && !strcmp("-show", argv[par])) {
			par++;
			//shows all algorithms
			printf("The list of all string matching algorithms\n");
			for(i=0; i<NumAlgo; i++) if(ALGO_NAME[i]) {
				printf("%s\n",ALGO_NAME[i]);
			}
			return 1;
		}
		if (par<argc && !strcmp("-which", argv[par])) {
			par++;
			//shows all selected algorithms
			printf("\n\tThe list of selected algorithms:\n");
			for(i=0; i<NumAlgo; i++) if(ALGO_NAME[i] && EXECUTE[i]) {
				printf("\t-%s\n",ALGO_NAME[i]);
			}
			printf("\n");
			return 1;
		}
		if (par<argc && !strcmp("-add", argv[par])) {
			par++;
			if(par>=argc) {printf("\n\n\tError in input parameters. Use -h for help.\n\n"); return 0;}
			strcpy(filename, argv[par++]);
			char path[50] = "source/bin/";
			strcat(path,filename);
			FILE *fp = fopen(path, "r");
			if( fp ) { 
				//the file exists
				if(search_ALGO(ALGO_NAME,filename)>=0) 
					printf("\n\n\tSMART error message\n\tError in input parameters....algorithm %s already in the set\n\n",filename);
				else {
					printf("\n\n\tAdding the algorithm %s to SMART\n",filename); fflush(stdout);
					printf("\tTesting the algorithm for correctness...."); fflush(stdout);
					//testing correctness of the algorithm
					sprintf(command,"./test ../bin/%s -nv",filename);
					fflush(stdout);
					if(system(command)) { 
						printf("failed!\n");
						printf("\tThe system is unable to add the algorithm %s to SMART.\n\tPlease, check for algorithm's correctness.\n\n",filename);
					}
					else {
						printf("ok\n");
						for(i=0; i<NumAlgo && ALGO_NAME[i]; i++);
						EXECUTE[i] = 0;
						ALGO_NAME[i] = filename;
						printf("\tAlgorithm %s added succesfully.\n\n",filename);
					}
				}
			}
			else printf("\n\n\tSMART error message\n\tError in input parameters....program %s does not exist\n\n",path);
		}
		if (par<argc && !strcmp("-group", argv[par])) {
			par++;
			if(par>=argc) {printf("\n\n\tError in input parameters. Use -h for help.\n\n"); return 0;}
			
			strcpy(filename, argv[par++]);
			char path[50] = "source/bin/";
			strcat(path,filename);
			FILE *fp = fopen(path, "r");
			if( fp ) { 
				//the file exists
				if(search_ALGO(ALGO_NAME,filename)>=0) 
					printf("\n\n\tSMART error message\n\tError in input parameters....algorithm %s already in the set\n\n",filename);
				else {
					printf("\n\n\tAdding the algorithm %s to SMART\n",filename); fflush(stdout);
					printf("\tTesting the algorithm for correctness...."); fflush(stdout);
					//testing correctness of the algorithm
					sprintf(command,"./test ../bin/%s -nv",filename);
					fflush(stdout);
					if(system(command)) { 
						printf("failed!\n");
						printf("\tThe system is unable to add the algorithm %s to SMART.\n\tPlease, check for algorithm's correctness.\n\n",filename);
					}
					else {
						printf("ok\n");
						for(i=0; i<NumAlgo && ALGO_NAME[i]; i++);
						EXECUTE[i] = 0;
						ALGO_NAME[i] = filename;
						printf("\tAlgorithm %s added succesfully.\n\n",filename);
					}
				}
			}
			else printf("\n\n\tSMART error message\n\tError in input parameters....program %s does not exist\n\n",path);
		}
		for(i=0; i<NumAlgo; i++) if(ALGO_NAME[i]) {
			if (par<argc && !strcmp(argv[par],ALGO_NAME[i])) {
				par++;
				if(EXECUTE[i]==0) {
					EXECUTE[i]=1;
					printf("\tThe %s algorithm has been selected\n",ALGO_NAME[i]);
				}
				else {
					EXECUTE[i]=0;
					printf("\tThe %s algorithm has been deselected\n",ALGO_NAME[i]);
				}
				selected = 1;
			}
		}
		if(selected) continue;
		if (par<argc && !strcmp("-all", argv[par])) {
			par++;
			for(i=0; i<NumAlgo; i++) if(ALGO_NAME[i]) EXECUTE[i]=1;
			continue;
		}
		if (par<argc && !strcmp("-none", argv[par])) {
			par++;
			for(i=0; i<NumAlgo; i++) if(ALGO_NAME[i]) EXECUTE[i]=0;
			continue;
		}
		if (par<argc) {printf("\tError in input parameters....no parameter %s\n\n",argv[par]); return 0;}
	}
	int order[NumAlgo];
	for(i=0; i<NumAlgo; i++) order[i] = i;
	for(i=0; i<NumAlgo; i++)
		for(j=0; j<NumAlgo; j++)
			if(ALGO_NAME[order[j]] && ALGO_NAME[order[j+1]] && strcmp(ALGO_NAME[order[j]],ALGO_NAME[order[j+1]])>0)  {
				int tmp = order[j];
				order[j] = order[j+1];
				order[j+1] = tmp;
			}
	FILE *fp = fopen("source/algorithms.h", "w");
	for(j=0; j<NumAlgo; j++) if(ALGO_NAME[j]) fprintf(fp,"#%d #%s \n", EXECUTE[order[j]], ALGO_NAME[order[j]] );
	fclose(fp);
}
