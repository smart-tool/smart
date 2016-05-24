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
 
int string2decimal(char *s) {
	int i;
	int decimal;
	int len = strlen(s);
	decimal = 0;
	for(i=0; i<len; i++) {
		decimal = (decimal*10)+(s[i]-'0');
	}
	return decimal;
}

/* returns 1 if s is an integer number. 0 otherwise */ 
int isInt(char *s) {
	int i;
	for(i=0; i<strlen(s); i++) if(s[i]<'0' || s[i]>'9') return 0;
	return 1;
}

void getExecute(int EXECUTE[]) {
	FILE *fp = fopen("source/execute.h", "r");
	char c; int i=0;
	while( (c=getc(fp)) != EOF )
		if(c=='#') EXECUTE[i++] = (getc(fp)-'0');
	fclose(fp);
	while(i<NumAlgo) EXECUTE[i++]=0;
}

void getAlgoName(char *ALGO_NAME[]) {
	FILE *fp = fopen("source/algoname.h", "r");
	char c; int i=0;
	while( (c=getc(fp)) != EOF )
		if(c=='#') { 
			ALGO_NAME[i] = (char*) malloc (sizeof(char)*20);			
			int j = 0;
			while((c=getc(fp))!=',') ALGO_NAME[i][j++]=c;
			ALGO_NAME[i][j]='\0';
			i++;
		}
	while(i<NumAlgo) ALGO_NAME[i++]=NULL;
	fclose(fp);
}

void getAlgo(char *ALGO_NAME[], int EXECUTE[]) {
	FILE *fp = fopen("source/algorithms.h", "r");
	char c; int i=0;
	while( (c=getc(fp)) != EOF )
		if(c=='#') { 
			EXECUTE[i] = (getc(fp)-'0');
			getc(fp);getc(fp);
			ALGO_NAME[i] = (char*) malloc (sizeof(char)*20);			
			int j = 0;
			while((c=getc(fp))!=' ') ALGO_NAME[i][j++]=c;
			ALGO_NAME[i][j]='\0';
			i++;
		}
	while(i<NumAlgo) ALGO_NAME[i++]=NULL;
	fclose(fp);
}

char *str2lower(char *s)  {
	int n = strlen(s)-1;
	while(n>=0) {
		if(s[n]>='A' && s[n]<='Z') s[n]=s[n]-'A'+'a';
		n--;
	}
	return s;
}

char *str2upper(char *s)  {
	int n = strlen(s)-1;
	while(n>=0) {
		if(s[n]>='a' && s[n]<='z') s[n]=s[n]-'a'+'A';
		n--;
	}
	return s;
}

void getAlgoDescription(char *ALGO_DESCRIPTION[]) {
	FILE *fp = fopen("source/algodescription.h", "r");
	char c; int i=0;
	while( (c=getc(fp)) != EOF )
		if(c=='#') { 
			ALGO_DESCRIPTION[i] = (char*) malloc (sizeof(char)*100);			
			int j = 0;
			while((c=getc(fp))!=',') ALGO_DESCRIPTION[i][j++]=c;
			ALGO_DESCRIPTION[i][j]='\0';
			i++;
		}
	while(i<NumAlgo) ALGO_DESCRIPTION[i++]=NULL;
	fclose(fp);
}

int split_filelsit(char *filename, char list_of_filenames[NumSetting][50]) {
	int i, j, k;
	i = j = k = 0;
	int m = strlen(filename);
	while(i<m) {
		while(filename[i] != '-' && i<m) {
			list_of_filenames[k][j++] = filename[i];
			i++;
		}
		list_of_filenames[k][j] = '\0';
		if(filename[i] == '-') {
			k++;
			j=0;
			i++;
		}
	}
	return (k+1);
}


