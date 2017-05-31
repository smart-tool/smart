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
 *
 * file test.c
 * this program is used for testing correctness of algorithms
 * test the algorithm on different conditions and
 * check the number of occurrences reported
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SIGMA 256
#define XSIZE 100
#define YSIZE 100
#define ATTEMPT 40 //numer of attempt to allocate shared memory
#define VERBOSE !strcmp(parameter,"-nv")

int tshmid, eshmid, preshmid, pshmid, rshmid;
unsigned char *T,*P;
double *e_time, *pre_time;
int *count;

/* the brute force algorithm used for comparing occurrences */
int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, count, j;

   /* Searching */
   count = 0;
   for (j = 0; j <= n - m; ++j) {
      for (i = 0; i < m && x[i] == y[i + j]; ++i);
      if (i >= m)
         count++;
   }
   return count;
}

void printManual() {
	printf("\n\tSMART UTILITY FOR TESTING STRING MATCHING ALGORITHMS\n\n");
	printf("\tusage: ./test ALGONAME\n");
	printf("\tTest the program named \"algoname\" for correctness.\n");
	printf("\tThe program \"algoname\" must be located in source/bin/\n");
	printf("\tOnly programs in smart format can be tested.\n");
	printf("\n\n");
}

int execute(char *algoname, key_t pkey, int m, key_t tkey, int n, key_t rkey, key_t ekey, key_t prekey, int *count, int alpha) {
	char command[100];
	sprintf(command, "./source/bin/%s shared %d %d %d %d %d %d %d",algoname,pkey,m,tkey,n,rkey,ekey,prekey);
	//printf("%s\n",command);
    int res = system(command);
	if(!res) return (*count);
	else return -1;
}

int FREQ[SIGMA];

void free_shm() {
    shmdt(T);
    shmdt(P);
    shmdt(count);
    shmdt(e_time);
    shmdt(pre_time);
   	shmctl(tshmid, IPC_RMID,0);
   	shmctl(pshmid, IPC_RMID,0);
   	shmctl(rshmid, IPC_RMID,0);
    shmctl(eshmid, IPC_RMID,0);
    shmctl(preshmid, IPC_RMID,0);
}

int attempt(int *rip, int *count, unsigned char *P, int m, unsigned char*T, int n,
			char *algoname, key_t pkey, key_t tkey, key_t rkey, key_t ekey, key_t prekey,
			int alpha, char *parameter, int ncase) {
	(*rip)++;
	//printf("\b\b\b\b\b\b[%.3d%%]",(*rip)*100/18); fflush(stdout);
	(*count) = 0;
	int occur1 = search(P,m,T,n);
	int occur2 = execute(algoname,pkey,m,tkey,n,rkey,ekey,prekey,count,alpha);
	if(occur2>=0 && occur1 != occur2) {
		if(!VERBOSE) printf("\n\tERROR: test failed on case n.%d (\"%s\" in \"%s\")\n\
							found %d occ instead of %d\n\n", ncase,P,T,occur2,occur1);
		free_shm();
		return 0;
	}
	return 1;
}

int main(int argc, char *argv[]) {
	int i, j;
	
	/* processing of input parameters */
	if (argc==1) {printManual(); return 0;}
	char algoname[100];
	strcpy(algoname,argv[1]);
	char parameter[100];
	if(argc>2) strcpy(parameter,argv[2]);
	char filename[100] = "source/bin/";
	strcat(filename,algoname);
	FILE *fp = fopen(filename, "r");
	if( !fp ) {
		if(!VERBOSE) printf("\n\tERROR: unable to execute program %s\n\n",filename);
		exit(1);
	}
	fclose(fp);
	
   	//allocate space for text in shered memory
   	srand( time(NULL) );
   	key_t tkey = rand()%1000;
    int try = 0;
    do  {
		tkey = rand()%1000;
		tshmid = shmget(tkey, YSIZE*2, IPC_CREAT | 0666); 
 	} while(++try<ATTEMPT && tshmid<0);
    if (tshmid < 0) {
        perror("shmget"); 
		exit(1);
    }
   	if ((T = shmat(tshmid, NULL, 0)) == (unsigned char *) -1) {
   	    perror("shmat"); 
		free_shm();
		exit(1);
   	}

	//allocate space for running time in shered memory
	srand( time(NULL) );
	key_t ekey = rand()%1000;
	try = 0;
	do {
		ekey = rand()%1000; 
		eshmid = shmget(ekey, 8, IPC_CREAT | 0666); 
	} while((++try<ATTEMPT && eshmid<0) || ekey==tkey);
	if (eshmid < 0) {
		perror("shmget"); 
		free_shm();
		exit(1);
	}
	if ((e_time = shmat(eshmid, NULL, 0)) == (double *) -1) {
		perror("shmat"); 
		free_shm();
		exit(1);
	}


   //allocate space for preprocessing running time in shered memory
   key_t prekey = rand()%1000;
   try = 0;
   do {
		prekey = rand()%1000; 
		preshmid = shmget(prekey, 8, IPC_CREAT | 0666); 
   } while((++try<ATTEMPT && preshmid<0) || prekey==tkey || prekey==ekey);
   if (preshmid < 0) {
       perror("shmget"); 
	   free_shm();
	   exit(1);
   }
   if ((pre_time = shmat(preshmid, NULL, 0)) == (double *) -1) {
       perror("shmat"); 
  	   free_shm();
	   exit(1);
   }
   for(i=0; i<SIGMA; i++) FREQ[i] = 0;


	//allocate space for pattern in shered memory
   key_t pkey = rand()%1000;
   try = 0;
   do {
		pkey = rand()%1000; 
		pshmid = shmget(pkey, XSIZE+1, IPC_CREAT | 0666); 
   } while((++try<ATTEMPT && pshmid<0) || pkey==tkey || pkey==ekey || pkey==prekey);
   if (pshmid < 0) {
       perror("shmget"); 
	   free_shm();
	   exit(1);
   }
   if ((P = shmat(pshmid, NULL, 0)) == (unsigned char *) -1) {
       perror("shmat"); 
   	   free_shm();
	   exit(1);
   }
   for(i=0; i<SIGMA; i++) FREQ[i] = 0;

   //allocate space for the result number of occurrences in shared memory
   int *count;
   key_t rkey = rand()%1000;
   try = 0;
   do  {
		rkey = rand()%1000; 
		rshmid = shmget(rkey, 4, IPC_CREAT | 0666); 
   } while((++try<ATTEMPT && rshmid<0) || rkey==tkey || rkey==pkey || pkey==ekey || pkey==prekey);
   if (rshmid < 0) {
       perror("shmget"); 
       free_shm();
	   exit(1);
   }
   if ((count = shmat(rshmid, NULL, 0)) == (int *) -1) {
       perror("shmat"); 
	   free_shm();
	   exit(1);
   }

	if(!VERBOSE) printf("\n\tPlease, wait a moment..............");
	fflush(stdout);

	//begin testing
	int rip = 0;
	int alpha, k, h, m, occur1, occur2, test=1;
	/*for(alpha = 2; alpha<=128; alpha*=2) {
		for(i=0; i<YSIZE; i++) T[i] = rand()%alpha;
		// compute the frequency of characters
		//for(j=0; j<SIGMA; j++) FREQ[j]=0;
		//for(j=0; j<YSIZE; j++) FREQ[T[j]]++;
		for(m = 2; m<=16; m*=2) {
			for(j=0; j<10; j++) {
				rip++;
				printf("\b\b\b\b\b\b[%.3d%%]",rip*10/28); fflush(stdout);
				(*count) = 0;
				k = j*2;
				for(h=0; h<m; h++) P[h] = T[k+h];
				P[m]='\0';
				occur1 = search(P,m,T,YSIZE);
	        	occur2 = execute(algoname,pkey,m,tkey,YSIZE,rkey,ekey,prekey,count,alpha);
				if(occur2>=0 && occur1 != occur2) {
					if(!VERBOSE) printf("\n\tERROR: test failed\n\n");
					free_shm();
					exit(1);
				}
			}
		}
	}*/
	
	// 1) search for "a" in "aaaaaaaaaa"
	strcpy((char*)P,"a");
	strcpy((char*)T,"aaaaaaaaaa");
	if(!attempt(&rip,count,P,1,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,1))
		exit(1);

	// 2) search for "aa" in "aaaaaaaaaa"
	strcpy((char*)P,"aa");
	strcpy((char*)T,"aaaaaaaaaa");
	if(!attempt(&rip,count,P,2,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,2))
		exit(1);

	// 3) search for "aaaaaaaaaa" in "aaaaaaaaaa"
	strcpy((char*)P,"aaaaaaaaaa");
	strcpy((char*)T,"aaaaaaaaaa");
	if(!attempt(&rip,count,P,10,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,3))
		exit(1);

	// 4) search for "b" in "aaaaaaaaaa"
	strcpy((char*)P,"b");
	strcpy((char*)T,"aaaaaaaaaa");
	if(!attempt(&rip,count,P,1,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,4))
		exit(1);

	// 5) search for "abab" in "ababababab"
	strcpy((char*)P,"ab");
	strcpy((char*)T,"ababababab");
	if(!attempt(&rip,count,P,2,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,5))
		exit(1);

	// 6) search for "a" in "ababababab"
	strcpy((char*)P,"a");
	strcpy((char*)T,"ababababab");
	if(!attempt(&rip,count,P,1,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,6))
		exit(1);

	// 7) search for "aba" in "ababababab"
	strcpy((char*)P,"aba");
	strcpy((char*)T,"ababababab");
	if(!attempt(&rip,count,P,3,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,7))
		exit(1);

	// 8) search for "abc" in "ababababab"
	strcpy((char*)P,"abc");
	strcpy((char*)T,"ababababab");
	if(!attempt(&rip,count,P,3,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,8))
		exit(1);

	// 9) search for "ba" in "ababababab"
	strcpy((char*)P,"ba");
	strcpy((char*)T,"ababababab");
	if(!attempt(&rip,count,P,2,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,8))
		exit(1);

	// 10) search for "babbbbb" in "ababababab"
	strcpy((char*)P,"babbbbb");
	strcpy((char*)T,"ababababab");
	if(!attempt(&rip,count,P,7,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,10))
		exit(1);

	// 11) search for "bcdefg" in "bcdefghilm"
	strcpy((char*)P,"bcdefg");
	strcpy((char*)T,"bcdefghilm");
	if(!attempt(&rip,count,P,6,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,11))
		exit(1);

	// 12) search for rand in rand
	for(h=0; h<10; h++) T[h] = rand()%128;
	for(h=0; h<4; h++) P[h] = T[h];
	T[YSIZE] = P[4] = '\0';
	if(!attempt(&rip,count,P,4,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,12))
		exit(1);

	// 13) search for rand in rand
	for(h=0; h<10; h++) T[h] = rand()%128;
	for(h=0; h<4; h++) P[h] = T[h];
	T[10] = P[4] = '\0';
	if(!attempt(&rip,count,P,4,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,13))
		exit(1);

	// 14) search for rand in rand
	for(h=0; h<64; h++) T[h] = rand()%128;
	for(h=0; h<40; h++) P[h] = T[h];
	T[64] = P[40] = '\0';
	if(!attempt(&rip,count,P,40,T,64,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,14))
		exit(1);

	// 15) search for rand in rand
	for(h=0; h<64; h++) T[h] = rand()%128;
	for(h=0; h<40; h++) P[h] = T[h];
	T[64] = P[40] = '\0';
	if(!attempt(&rip,count,P,40,T,64,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,15))
		exit(1);

	// 16) search for rand in rand
	for(h=0; h<64; h++) T[h] = 'a';
	for(h=0; h<40; h++) P[h] = 'a';
	T[64] = P[40] = '\0';
	if(!attempt(&rip,count,P,40,T,64,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,16))
		exit(1);

	// 17) search for rand in rand
	for(h=0; h<64; h+=2) T[h] = 'a';
	for(h=1; h<64; h+=2) T[h] = 'b';
	for(h=0; h<40; h+=2) P[h] = 'a';
	for(h=1; h<40; h+=2) P[h] = 'b';
	T[64] = P[40] = '\0';
	if(!attempt(&rip,count,P,40,T,64,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,17))
		exit(1);

	// 18) search for rand in rand
	for(h=0; h<64; h+=2) T[h] = 'a';
	for(h=1; h<64; h+=2) T[h] = 'b';
	for(h=0; h<40; h+=2) P[h] = 'a';
	for(h=1; h<40; h+=2) P[h] = 'b';
	P[39] = 'c';
	T[64] = P[40] = '\0';
	if(!attempt(&rip,count,P,40,T,64,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,18))
		exit(1);

    // 19) search for "babbbbb" in "abababbbbb"
    strcpy((char*)P,"babbbbb");
    strcpy((char*)T,"abababbbbb");
    if(!attempt(&rip,count,P,7,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,19))
        exit(1);

    // 20) search for "bababb" in "abababbbbb"
    strcpy((char*)P,"bababb");
    strcpy((char*)T,"abababbbbb");
    if(!attempt(&rip,count,P,6,T,10,algoname,pkey,tkey,rkey,ekey,prekey,alpha,parameter,20))
        exit(1);
    	
	if(!VERBOSE) printf("\n\tWell done! Test passed successfully\n\n");
		

   	//free shared memory
   	free_shm();
	return 0;
}

