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

char colors[100][8];
int num_colors = 100;

char dec2hex(int v) {
	if(v < 10) return '0'+v;
	if(v == 10) return 'a';
	if(v == 11) return 'b';
	if(v == 12) return 'c';
	if(v == 13) return 'd';
	if(v == 14) return 'e';
	if(v == 15) return 'f';
	return '0';
}

void computeRandomColors(char colors[100][8]) {
    srand( time(NULL) );
	for(int i=0; i<100; i++) {
		int j = 0;
		colors[i][j++] = '#';
		colors[i][j++] = dec2hex((rand()%14));
		colors[i][j++] = dec2hex((rand()%14));
		colors[i][j++] = dec2hex((rand()%14));
		colors[i][j++] = dec2hex((rand()%14));
		colors[i][j++] = dec2hex((rand()%14));
		colors[i][j++] = dec2hex((rand()%14));
		colors[i][j] = '\0';
	}
}

int outputPHP(double TIME[NumAlgo][NumPatt], double BEST[NumAlgo][NumPatt], double WORST[NumAlgo][NumPatt], double STD[NumAlgo][NumPatt], 
			  int alpha, char *filename, char *expcode, int dif, int std) {
  	int  i, il, algo;
   	FILE *fp;
	char outname[100];
   	//printing results in txt format
	strcpy(outname, "results/");
	strcat(outname, expcode);
	//mkdir(outname,S_IROTH);
	strcat(outname, "/");
	strcat(outname, filename);
	strcat(outname, ".php");
   	printf("\tSaving data on %s/%s.php\n",expcode,filename);
   	if ( !(fp = fopen(outname,"w") ) ) {
		printf("\tError in writing file %s/%s.txt\n",expcode,filename);
		return 0;
	}
	fprintf(fp,"<?\n$%s = array(\n",filename);
	fprintf(fp,"\t\"PATT\" => array(");
	for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
		fprintf(fp,"\"%d\", ", PATT_SIZE[il]);
	}
	fprintf(fp, "),\n");
   	for(algo=0; algo<NumAlgo; algo++) {
		if(EXECUTE[algo]) {
   			fprintf(fp,"\t\"%s\" => array(",str2upper(ALGO_NAME[algo]));
   			for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   				if(TIME[algo][il]==0)  fprintf(fp,"\"VOID\", ");
   	  	 		else
					fprintf(fp,"\"%.2f\", ",TIME[algo][il]);
   			}
   			fprintf(fp,"),\n");
			if(dif) {
				fprintf(fp,"\t\"%s.best\" => array(",str2upper(ALGO_NAME[algo]));
				for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
					if(TIME[algo][il]==0)  fprintf(fp,"\"0.1\", ");
					else
						fprintf(fp,"\"%.2f\", ",BEST[algo][il]);
				}
				fprintf(fp,"),\n");
				fprintf(fp,"\t\"%s.worst\" => array(",str2upper(ALGO_NAME[algo]));
				for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
					if(TIME[algo][il]==0)  fprintf(fp,"\"0.1\", ");
					else
						fprintf(fp,"\"%.2f\", ",WORST[algo][il]);
				}
				fprintf(fp,"),\n");
			}
			if(std) {
				fprintf(fp,"\t\"%s.std\" => array(",str2upper(ALGO_NAME[algo]));
				for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
					if(TIME[algo][il]==0)  fprintf(fp,"\"VOID\", ");
					else
						fprintf(fp,"\"%.2f\", ",STD[algo][il]);
				}
				fprintf(fp,"),\n");
			}
		}
   	}
	fprintf(fp,");\n?>");
   	fclose(fp);
	
	
	return 1;
}


int outputTXT(double TIME[NumAlgo][NumPatt], int alpha, char *filename, char *expcode, char *time_format) {
  	int  i, il, algo;
   	FILE *fp;
	char outname[100];
   	//printing results in txt format
	strcpy(outname, "results/");
	strcat(outname, expcode);
	//mkdir(outname,S_IROTH);
	strcat(outname, "/");
	strcat(outname, filename);
	strcat(outname, ".txt");
   	printf("\tSaving data on %s/%s.txt\n",expcode,filename);
   	if ( !(fp = fopen(outname,"w") ) ) {
		printf("\tError in writing file %s/%s.txt\n",expcode,filename);
		return 0;
	}
   	for(algo=0; algo<NumAlgo; algo++) {
		if(EXECUTE[algo]) {
			int first = 1;
   			fprintf(fp,"%s",str2upper(ALGO_NAME[algo]));
			for(i=0; i<20-strlen(ALGO_NAME[algo]); i++) fprintf(fp," ");
   			for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
				if(!first) fprintf(fp,"\t");
				first = 0;
   				if(TIME[algo][il]==0)  fprintf(fp,"-");
   	  	 		else
   	 	  		fprintf(fp,"%.2f",TIME[algo][il]);
   			}
   			fprintf(fp,"\n");
		}
   	}
   	fclose(fp);


	return 1;
}

int outputLatex(double TIME[NumAlgo][NumPatt], int alpha, char *filename, char *expcode, char* time_format) {
  	int  i, j,il, algo;
   	FILE *fp;
	char outname[100];
   	//printing results in latex format
	strcpy(outname, "results/");
	strcat(outname, expcode);
	//mkdir(outname,S_IROTH);
	strcat(outname, "/");
	strcat(outname, filename);
	strcat(outname, ".tex");
   	printf("\tSaving data on %s/%s.tex\n",expcode,filename);
   	if ( !(fp = fopen(outname,"w") ) ) {
		printf("\tError in writing file %s/%s.txt\n",expcode,filename);
		return 0;
	}
	int start=0;
	while(PATT_SIZE[start]<MINLEN) start++;
	int end=start;
	while(PATT_SIZE[end]<=MAXLEN) end++;
	fprintf(fp,"\\begin{tabular}{|l|");
	for(j=start; j<end; j++) fprintf(fp,"l");
	fprintf(fp,"|}\n\\hline\n");
	fprintf(fp,"$m$");
	for(j=start; j<end; j++) fprintf(fp," & $%d$",PATT_SIZE[j]);
	fprintf(fp,"\\\\\n");
   	for(algo=0; algo<NumAlgo; algo++) {
		if(EXECUTE[algo]) {
			int first = 1;
   			fprintf(fp,"\\textsc{%s}",str2upper(ALGO_NAME[algo]));
   			for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   				if(TIME[algo][il]==0)  fprintf(fp," & -");
   	  	 		else
   	 	  		fprintf(fp," & %.2f",TIME[algo][il]);
   			}
   			fprintf(fp,"\\\\\n");
		}
   	}
	fprintf(fp,"\\hline\n\\end{tabular}");
   	fclose(fp);
	return 1;
}

int outputXML(double TIME[NumAlgo][NumPatt], int alpha, char *filename, char *expcode) {
  	int  i, il, algo;
   	FILE *fp;
	char outname[100];
	//finds the best result for each pattern length
	double BEST[NumPatt];
	for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
		double best = 999999.0;	
		for(algo=0; algo<NumAlgo; algo++) if(EXECUTE[algo] && TIME[algo][il]<best && TIME[algo][il]>0.1) best=TIME[algo][il];
		BEST[il] = best;
	}
   	//printing results in xml format
	strcpy(outname, "results/");
	strcat(outname, expcode);
	mkdir(outname,S_IROTH);
	strcat(outname, "/");
	strcat(outname, filename);
	strcat(outname, ".xml");
   	printf("\tSaving data on %s/%s.xml\n",expcode,filename);
   	if ( !(fp = fopen(outname,"w") ) ) {
		printf("\tError in writing file %s/%s.xml\n",expcode,filename);
		return 0;
	}
	fprintf(fp,"<RESULTS>\n\t<CODE>%s</CODE>\n\t<TEXT>%s</TEXT>\n",expcode,filename);
   	for(algo=0; algo<NumAlgo; algo++) {
		if(EXECUTE[algo]) {
   			fprintf(fp,"\t<ALGO>\n\t\t<NAME>%s</NAME>\n",str2upper(ALGO_NAME[algo]));
			//for(i=0; i<20-strlen(ALGO_NAME[algo]); i++) fprintf(fp," ");
   			for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   				if(TIME[algo][il]==0)  fprintf(fp,"\t\t<DATA>-</DATA>\n");
   	 	  		fprintf(fp,"\t\t<DATA>\n");
   	 	  		fprintf(fp,"\t\t\t<SEARCH>%.2f</SEARCH>\n",TIME[algo][il]);
   	 	  		fprintf(fp,"\t\t</DATA>\n");
   			}
   			fprintf(fp,"\t</ALGO>\n");
		}
   	}
	fprintf(fp,"\t<BEST>\n");
	for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
  		fprintf(fp,"\t\t<DATA>%.2f</DATA>\n",BEST[il]);
	}
	fprintf(fp,"\t</BEST>\n");
	fprintf(fp,"</RESULTS>");
   	fclose(fp);
	return 1;
}

void appendFilesContent(char *source, FILE *target) {
	char c;
	FILE *fp = fopen(source,"r");
	while( (c=getc(fp))!=EOF ) putc(c,target);
	fclose(fp);
}

void printSTD(		double TIME[NumAlgo][NumPatt],
					double BEST[NumAlgo][NumPatt],
					double WORST[NumAlgo][NumPatt],
					double STD[NumAlgo][NumPatt], 
					int algo, char *expcode, FILE *fp) {

	int i;
	double dymax = 0.0;
	for(int il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   		if(WORST[algo][il]>dymax)  dymax = WORST[algo][il];
   		if(TIME[algo][il]+STD[algo][il]>dymax)  dymax = TIME[algo][il]+STD[algo][il];
  	}
	int ymax = dymax+1.0;
	
	fprintf(fp,"<div class=\"chart_container_small\"><div class=\"chart_title\">%s algorithm</div>\n",str2upper(ALGO_NAME[algo]));
	fprintf(fp,"<div><canvas class=\"exp_chart_small\" id=\"cvs%d\" width=\"460\" height=\"250\">[No canvas support]</canvas>",algo);
	fprintf(fp,"<div class=\"caption_small\">Detailed plot of the running times relative to the <b>%s algorithm</b>. ",str2upper(ALGO_NAME[algo]));
	fprintf(fp,"The plot reports the mean and the distribution of the running times.");
	fprintf(fp,"</div>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"</div>\n");


	fprintf(fp,"<script>function loadChart%d() { ",algo);

	fprintf(fp,"var data = [");
	for(int il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   		if(TIME[algo][il]==0)  fprintf(fp,",");
  		else fprintf(fp,"%.2f,",TIME[algo][il]);
  	}
    fprintf(fp,"];\n");

	fprintf(fp,"var std1 = [");
	for(int il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   		if(TIME[algo][il]==0)  fprintf(fp,",");
  		else fprintf(fp,"%.2f,",TIME[algo][il]-STD[algo][il]);
  	}
    fprintf(fp,"];\n");

	fprintf(fp,"var std2 = [");
	for(int il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   		if(TIME[algo][il]==0)  fprintf(fp,",");
  		else fprintf(fp,"%.2f,",TIME[algo][il]+STD[algo][il]);
  	}
    fprintf(fp,"];\n");

	fprintf(fp,"var bound1 = [");
	for(int il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   		if(TIME[algo][il]==0)  fprintf(fp,",");
  		else fprintf(fp,"%.2f,",WORST[algo][il]);
  	}
    fprintf(fp,"];\n");

	fprintf(fp,"var bound2 = [");
	for(int il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   		if(TIME[algo][il]==0)  fprintf(fp,",");
  		else fprintf(fp,"%.2f,",BEST[algo][il]);
  	}
    fprintf(fp,"];\n");

	fprintf(fp,"var line3 = new RGraph.Line({\n\
            id: 'cvs%d',\n\
            data: [bound1, bound2],\n\
            options: {\n\
                noxaxis: true,\n\
                textSize: 14,\n\
                filled: true,\n\
                filledRange: true,\n\
                fillstyle: 'rgba(255,0,0,0.1)',\n\
                colors: ['rgba(0,0,0,0)'],\n\
                linewidth: 0,\n\
                ylabels: false,\n\
                noaxes: true,\n\
                ymax: %d,\n\
                hmargin: 5,\n\
                spline: true,\n\
                gutterLeft: 40,\n\
                tickmarks: null,\n\
            }\n\
        }).draw();\n",algo,ymax);
        
	fprintf(fp,"var line2 = new RGraph.Line({\n\
            id: 'cvs%d',\n\
            data: [std1, std2],\n\
            options: {\n\
                noxaxis: true,\n\
                textSize: 14,\n\
                filled: true,\n\
                filledRange: true,\n\
                fillstyle: 'rgba(255,0,0,0.2)',\n\
                colors: ['rgba(0,0,0,0)'],\n\
                linewidth: 0,\n\
                ylabels: false,\n\
                noaxes: true,\n\
                ymax: %d,\n\
                hmargin: 5,\n\
                spline: true,\n\
                gutterLeft: 40,\n\
                tickmarks: null,\n\
            }\n\
        }).draw();\n",algo,ymax);


    fprintf(fp,"var line = new RGraph.Line({\n\
            id: 'cvs%d',\n\
            data: data,\n\
            options: {\n\
            	textFont: 'Yantramanav',\n\
            	textSize: '8',\n\
            	textColor: '#444',\n\
                BackgroundBarcolor1: 'white',\n\
                BackgroundBarcolor2: 'red',\n\
                BackgroundGridColor: 'rgba(238,238,238,1)',\n\
                linewidth: 1,\n\
                filled: false,\n\
                hmargin: 5,\n\
                shadow: false,\
                tickmarks: 'circle',\n\
                ymax: %d,\n\
                spline: true,\n\
                gutterLeft: 40,\n\
                tickmarks: null,\n\
                labels: [",algo,ymax);
    for(int il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) fprintf(fp, "'%d',",PATT_SIZE[il]);
	fprintf(fp,"],\n");
	fprintf(fp, "colors: ['#000000'],\n");
	fprintf(fp,"} }).draw();");


	fprintf(fp,"}</script>");
}

void printMulti(	double TIME[NumAlgo][NumPatt],
					FILE *fp, int w, int h, char *title, int code) {

	int i, algo, il;
	double dymax = 0.0;
   	for(algo=0; algo<NumAlgo; algo++) {
		for(int il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   			if(TIME[algo][il]>dymax)  dymax = TIME[algo][il];
  		}
  	}
	int ymax = dymax+1.0;
	
	fprintf(fp,"<div class=\"chart_container_small\" style=\"height:300px\"><div class=\"chart_title\">%s</div>\n",title);
	fprintf(fp,"<div><canvas class=\"exp_chart_small\" id=\"cvs%d\" width=\"%d\" height=\"%d\">[No canvas support]</canvas>",code,w,h);
	fprintf(fp,"</div>\n");
	fprintf(fp,"</div>\n");


	fprintf(fp,"<script>function multiChart%d() { var data = [",code);
   	for(algo=0; algo<NumAlgo; algo++) {
		if(EXECUTE[algo]) {
   			fprintf(fp,"[");
   			for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   				if(TIME[algo][il]==0)  fprintf(fp,",");
   	 	  		else fprintf(fp,"%.2f,",TIME[algo][il]);
   			}
   			fprintf(fp,"],\n");
		}
   	}
    fprintf(fp,"]; \n\
        var line = new RGraph.Line({\n\
            id: 'cvs%d',\n\
            data: data,\n\
            options: {\n\
            	textFont: 'Yantramanav',\n\
            	textSize: '8',\n\
            	textColor: '#444',\n\
                BackgroundBarcolor1: 'white',\n\
                BackgroundBarcolor2: 'red',\n\
                BackgroundGridColor: 'rgba(238,238,238,1)',\n\
                linewidth: 1,\n\
                filled: false,\n\
                fillstyle: ['red','blue','#0f0'],\n\
                hmargin: 5,\n\
                shadow: false,\
                tickmarks: 'circle',\n\
                spline: true,\n\
                gutterLeft: 40,\n\
                labels: [",code);
    for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) fprintf(fp, "'%d',",PATT_SIZE[il]);
	fprintf(fp,"],\n");
	fprintf(fp, "colors: [");
	for(i=0; i<num_colors; i++) fprintf(fp, "'%s',", colors[i]);
	fprintf(fp,"],\n");
	fprintf(fp,"} }).draw();\n");


	fprintf(fp,"}</script>");
}


int outputHTML2(	double PRE_TIME[NumAlgo][NumPatt], 
					double TIME[NumAlgo][NumPatt],
					double BEST[NumAlgo][NumPatt],
					double WORST[NumAlgo][NumPatt],
					double STD[NumAlgo][NumPatt], 
					int pre, int dif, int alpha, int n, int volte, char *filename, char *expcode, char *time_format) {
  	int  i, il, algo;
   	FILE *fp;
	char outname[100];
	
	computeRandomColors(colors);
	
	double OPTIMAL[NumPatt];
	for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
		OPTIMAL[il] = 0.0;
	    for(algo=0; algo<NumAlgo; algo++)
			if(EXECUTE[algo]) {
				if(OPTIMAL[il]==0.0 || (OPTIMAL[il]>TIME[algo][il] && TIME[algo][il] > 0.0))  OPTIMAL[il]=TIME[algo][il];
			}
	}

   	//printing results in html format
	strcpy(outname, "results/");
	strcat(outname, expcode);
	mkdir(outname,S_IROTH);
	strcat(outname, "/");
	strcat(outname, filename);
	strcat(outname, ".html");
   	printf("\tSaving data on %s/%s.html\n",expcode,filename);
   	if ( !(fp = fopen(outname,"w") ) ) {
		printf("\tError in writing file %s/%s.html\n",expcode,filename);
		return 0;
	}
	fprintf(fp,"<!DOCTYPE html><html><head>");
	fprintf(fp,"<script src=\"../js/RGraph.common.core.js\"></script>");
	fprintf(fp,"<script src=\"../js/RGraph.common.effects.js\"></script>");
	fprintf(fp,"<script src=\"../js/RGraph.line.js\"></script>");
	fprintf(fp,"<script src=\"../js/RGraph.bar.js\"></script>");
	fprintf(fp,"<script src=\"../RGraph.common.dynamic.js\"></script>");
	fprintf(fp,"<script src=\"../RGraph.common.tooltips.js\"></script>");
    fprintf(fp,"<script src=\"../js/Smart.TimeResultFormatting.js\"></script>");
	fprintf(fp,"<link href='https://fonts.googleapis.com/css?family=Dosis:300' rel='stylesheet' type='text/css'>");
	fprintf(fp,"<link href='https://fonts.googleapis.com/css?family=Yantramanav:400,100,700' rel='stylesheet' type='text/css'>");
	fprintf(fp,"<link rel=\"stylesheet\" type=\"text/css\" href=\"../style.css\">");
    fprintf(fp,"<title>SMART Experimental Results %s: %s</title>",expcode,filename);
    fprintf(fp,"</head><body>");

	fprintf(fp,"<div class=\"main_container\">");
	fprintf(fp,"<h1>SMART<span class=\"subtitle\">String Matching Algorithms Research Tool<span></h1>");
	fprintf(fp,"<h3>by Simone Faro - <span class=\"link\">www.dmi.unict.it/~faro/smart/</span> - <span class=\"link\">email: faro@dmi.unict.it</span></h3>");
	fprintf(fp,"<div class=\"name\">");
	fprintf(fp,"<h2><b>Report of Experimental Results</b></h2>");
	fprintf(fp,"<h2>Test Code %s</h2>",expcode);
	fprintf(fp,"<h2>Date %s</h2>",time_format);
	fprintf(fp,"<h2>Text %s (alphabet : %d - size : %d bytes)</h2>",filename,alpha,n);
	fprintf(fp,"</div>");
	
	fprintf(fp,"<table id=\"resultTable\" class=\"exp_table\">\n");
	fprintf(fp,"<tr><td class=\"length\"></td>");
	for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
		fprintf(fp,"<td class=\"length\">%d</td>",PATT_SIZE[il]);
	}
	fprintf(fp,"<tr>");
    const char* preVisible = pre ? "block" : "none";
    const char* difVisible = dif ? "block" : "none";
    for(algo=0; algo<NumAlgo; algo++)
		if(EXECUTE[algo]) {
   			fprintf(fp,"<tr>\n");
   			fprintf(fp,"<td class=\"algo\"><b>%s</b></td>\n",str2upper(ALGO_NAME[algo]));   			
   			for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   				int best = 0;
   				if(TIME[algo][il]==OPTIMAL[il]) best=1;
   				fprintf(fp,"<td><center>");
                fprintf(fp,"<div class=\"pre_time\" style=\"display:%s\">%.2f</div>",preVisible, PRE_TIME[algo][il]);
   				if(TIME[algo][il]==0)  fprintf(fp,"<div class=\"search_time\">-</div>");
   	 	  		else {
   	 	  			if(!best) fprintf(fp,"<div class=\"search_time\">%.2f</div>",TIME[algo][il]);
   	 	  			else fprintf(fp,"<div class=\"search_time_best\"><b>%.2f</b></div>",TIME[algo][il]);
   	 	  		}
                fprintf(fp,"<div class=\"dif\" style=\"display:%s\">%.2f - %.2f</div>",difVisible, BEST[algo][il],WORST[algo][il]);
   				fprintf(fp,"</center></td>");
   			}
   			fprintf(fp,"</tr>\n");
		}
	fprintf(fp,"</table>\n");
	fprintf(fp,"<div class=\"caption\"><b>Table 1.</b> Running times of experimental tests n.%s. Each time value is the mean of %d runs. ",expcode,volte);
	if(pre) fprintf(fp,"The table reports also the mean of the preprocessing time (above each time value). ");
	if(dif) fprintf(fp,"In addition the worst and best running times are reported (below each time value). ");
	fprintf(fp,"Running times are in milliseconds.\n");
    fprintf(fp,"<br><div class=\"controlHorizontalFloat\">\n"
               "<input type=\"radio\" id=\"best\" name=\"resultformat\" value=\"best\" checked onclick=\"setBestColors(document)\">\n"
               "<label for=\"best\">Best mean times</label></div>\n"
               "<div class=\"controlHorizontalFloat\">\n"
               "<input type=\"radio\" id=\"heatMap5\" name=\"resultformat\" value=\"hm5\" onclick=\"heatMapGray(document.getElementById('resultTable'), 95)\">\n"
               "<label for=\"heatMap5\">Heatmap top 5%%</label></div>\n"
               "<div class=\"controlHorizontalFloat\">\n"
               "<input type=\"radio\" id=\"heatMap10\" name=\"resultformat\" value=\"hm10\" onclick=\"heatMapGray(document.getElementById('resultTable'), 90)\">\n"
               "<label for=\"heatMap25\">Heatmap top 10%%</label></div>\n"
               "<div class=\"controlHorizontalFloat\">\n"
               "<input type=\"radio\" id=\"heatMap25\" name=\"resultformat\" value=\"hm25\" onclick=\"heatMapGray(document.getElementById('resultTable'), 75)\">\n"
               "<label for=\"heatMap25\">Heatmap top 25%%</label></div>\n"
               "<div class=\"controlHorizontalFloat\">\n"
               "<input type=\"radio\" id=\"heatMap50\" name=\"resultformat\" value=\"hm50\" onclick=\"heatMapGray(document.getElementById('resultTable'), 50)\">\n"
               "<label for=\"heatMap50\">Heatmap top 50%%</label></div><div class=\"clearHorizontalFloat\"></div>\n"
               "<div class=\"controlHorizontalFloat\">\n");
    fprintf(fp,"<input type=\"checkbox\" id=\"pretime\" name=\"pretime\" value=\"pretime\" %s onclick=\"showDivs(document, 'pre_time', this.checked)\">\n", (pre? "checked" : ""));
    fprintf(fp,"<label for=\"pretime\">Show pre-processing times</label></div>\n"
               "<div class=\"controlHorizontalFloat\">\n");
    fprintf(fp,"<input type=\"checkbox\" id=\"bestworst\" name=\"bestworst\" value=\"bestworst\" %s onclick=\"showDivs(document, 'dif', this.checked)\">\n", (dif? "checked" : ""));
    fprintf(fp,"<label for=\"bestworst\">Show best and worst running times</label></div><br></div><p>\n");
	fprintf(fp,"<div class=\"chart_container\"><div class=\"chart_title\">Average Running Times</div>\n");
	fprintf(fp,"<canvas class=\"exp_chart\" id=\"cvs\" width=\"780\" height=\"400\">[No canvas support]</canvas>");
    fprintf(fp,"<div style=\"padding-top:40px\">");
    int col=0;
    for(algo=0; algo<NumAlgo; algo++)
		if(EXECUTE[algo])
   			fprintf(fp,"<div class=\"didascalia\"><div class=\"line\" style=\"background-color:%s\"></div class=\"label\"> %s</div>\n",colors[col++],str2upper(ALGO_NAME[algo]));
    fprintf(fp,"</div><br/><br/>");
	fprintf(fp,"<div class=\"caption\"><b>Chart 1.</b> Plot of the running times of experimental tests n.%s. ",expcode);
	fprintf(fp,"The x axes reports the length of the pattern (in a log scale) while the y axes reports the running time in milliseconds. ");
	fprintf(fp,"</div>\n");
	fprintf(fp,"</div>\n");

	printMulti(	WORST, fp, 480, 250, "Worst Running Times", 2);
	printMulti(	BEST, fp, 480, 250, "Best Running Times", 3);

   	for(algo=0; algo<NumAlgo; algo++) {
		if(EXECUTE[algo])
			printSTD(TIME, BEST, WORST, STD, algo, expcode, fp);
	}

	double dymax = 0.0;
    for(algo=0; algo<NumAlgo; algo++)
		if(EXECUTE[algo])
			for(int il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   				if(TIME[algo][il]>dymax)  dymax = TIME[algo][il];
  			}
	int ymax = dymax+1.0;
    
	fprintf(fp,"<script>window.onload = (function () { var data = [");
   	for(algo=0; algo<NumAlgo; algo++) {
		if(EXECUTE[algo]) {
   			fprintf(fp,"[");
   			for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) {
   				if(TIME[algo][il]==0)  fprintf(fp,",");
   	 	  		else fprintf(fp,"%.2f,",TIME[algo][il]);
   			}
   			fprintf(fp,"],\n");
		}
   	}
    fprintf(fp,"]; \n\
        var line = new RGraph.Line({\n\
            id: 'cvs',\n\
            data: data,\n\
            options: {\n\
            	textFont: 'Yantramanav',\n\
            	textSize: '8',\n\
            	textColor: '#444',\n\
                BackgroundBarcolor1: 'white',\n\
                BackgroundBarcolor2: 'red',\n\
                BackgroundGridColor: 'rgba(238,238,238,1)',\n\
                linewidth: 1,\n\
                filled: false,\n\
                fillstyle: ['red','blue','#0f0'],\n\
                hmargin: 5,\n\
                shadow: false,\
                tickmarks: 'circle',\n\
                spline: true,\n\
                ymax: %d,\n\
                gutterLeft: 40,\n\
                labels: [",ymax);
    for(il=0; il<NumPatt; il++)	if(PATT_SIZE[il]>=MINLEN && PATT_SIZE[il]<=MAXLEN) fprintf(fp, "'%d',",PATT_SIZE[il]);
	fprintf(fp,"],\n");
	fprintf(fp, "colors: [");
	for(i=0; i<num_colors; i++) fprintf(fp, "'%s',", colors[i]);
	fprintf(fp,"],\n");
	fprintf(fp,"} }).draw();\n");

   	for(algo=0; algo<NumAlgo; algo++) {
		if(EXECUTE[algo])
			fprintf(fp,"loadChart%d();\n",algo);
	}
	fprintf(fp,"multiChart2(); multiChart3();\n");
	fprintf(fp,"});</script>");
    
    
	fprintf(fp,"</div>");
    fprintf(fp,"</body></html>");
   	fclose(fp);
	return 1;
}

int outputHTML(double PRE_TIME[NumAlgo][NumPatt], double TIME[NumAlgo][NumPatt], int pre, int alpha, char *filename, char *expcode, char *time_format) {
  	int  i, il, algo;
   	FILE *fp;
	char outname[100];
   	//printing results in html format
	strcpy(outname, "results/");
	strcat(outname, expcode);
	mkdir(outname,S_IROTH);
	strcat(outname, "/");
	strcat(outname, filename);
	strcat(outname, ".html");
   	printf("\tSaving data on %s/%s.html\n",expcode,filename);
   	if ( !(fp = fopen(outname,"w") ) ) {
		printf("\tError in writing file %s/%s.html\n",expcode,filename);
		return 0;
	}
	fprintf(fp,"<html>\n\
		<head>\n\
		\t<title>SMART: experimental results %s on %s</title>\n\
		\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\
		\t<link href=\"../style.css\" rel=\"stylesheet\" type=\"text/css\" />\n\
		</head>\n\
		<body>\n\
		\t<script type=\"text/javascript\">\n\
		\t\tif (window.XMLHttpRequest) {\n\
		\t\t\t// code for IE7+, Firefox, Chrome, Opera, Safari\n\
	  	\t\t\txmlhttp=new XMLHttpRequest();\n\
		\t\t}\n\
		\t\telse {\n\
		\t\t\t// code for IE6, IE5\n\
  		\t\t\txmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");\n\
		\t\t}\n\
		\t\txmlhttp.open(\"GET\",\"%s.xml\",false);\n\
		\t\txmlhttp.send();\n\
		\t\txmlDoc=xmlhttp.responseXML; \n\
		\t\tdocument.write(\"<h2>SMART: experimental results %s on %s</h2><br>Results computed on %s<br/>\");\n\
		\t\tdocument.write(\"<table align=\\\"center\\\">\");\n",expcode,filename,filename,expcode,filename, time_format);
	fprintf(fp,"document.write(\"<tr><td></td>\");\n");
	for(i=0; PATT_SIZE[i]>0; i++) if(PATT_SIZE[i]>=MINLEN && PATT_SIZE[i]<=MAXLEN)
		fprintf(fp,"document.write(\"<td><b>%d</b></td>\");\n",PATT_SIZE[i]);
	fprintf(fp,"document.write(\"</tr>\");\n");
	fprintf(fp,"\
		\t\tvar best=xmlDoc.getElementsByTagName(\"BEST\");\n\
		\t\tvar bestvalues = best[0].getElementsByTagName(\"DATA\");\n\
		\t\tvar x=xmlDoc.getElementsByTagName(\"ALGO\");\n\
		\t\tfor (i=0;i<x.length;i++) { \n\
		\t\t\tdocument.write(\"<tr><td><b>\");\n\
  		\t\t\tdocument.write(x[i].getElementsByTagName(\"NAME\")[0].childNodes[0].nodeValue);\n\
  		\t\t\tdocument.write(\"</b></td>\");\n\
		\t\t\tvar times = x[i].getElementsByTagName(\"DATA\");\n\
		\t\t\tfor (j=0;j<times.length;j++) { \n\
  		\t\t\t\tdocument.write(\"<td>\");\n\
		\t\t\tvar y = times[j].getElementsByTagName(\"SEARCH\");\n\
		\t\t\tfor (k=0;k<y.length;k++) { \n\
		\t\t\t\tif(y[k].childNodes[0].nodeValue==bestvalues[j].childNodes[0].nodeValue) document.write(\"<b><u>\");\n\
  		\t\t\t\tif(y[k].childNodes[0].nodeValue<0.05) document.write(\"--\");\n\
  		\t\t\t\telse document.write(y[k].childNodes[0].nodeValue);\n\
		\t\t\t\tif(y[k].childNodes[0].nodeValue==bestvalues[j].childNodes[0].nodeValue) document.write(\"</u></b>\");\n\
  		\t\t\t\tdocument.write(\"</td>\");\n\
		\t\t\t}\n\
		\t\t\t}\n\
  		\t\t\tdocument.write(\"</tr>\");\n\
		\t\t}\n\
		\t\tdocument.write(\"</table>\");\n\
		\t</script>\n\
		</body>\n\
		</html>");
   	fclose(fp);
	return 1;
}

int outputINDEX(char list_of_filenames[NumSetting][50], int num_buffers, char *expcode) {
  	int  i, sett,il, algo;
   	FILE *fp;
	char outname[100];
   	//printing results in html format
	strcpy(outname, "results/");
	strcat(outname, expcode);
	mkdir(outname,S_IROTH);
	strcat(outname, "/index.html");
   	printf("\tWriting %s/index.html\n",expcode);
   	if ( !(fp = fopen(outname,"w") ) ) {
		printf("\tError in writing file %s/index.html\n",expcode);
		return 0;
	}
	fprintf(fp,"<html>\n\
		\t<head>\n\
		\t\t<title>SMART: experimental results %s</title>\n\
		\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\
		\t\t<link href=\"../style.css\" rel=\"stylesheet\" type=\"text/css\" />\n\
		\t</head>\n\
		\t<body>\n\
		\t\t<h2>SMART: experimental results %s</h2><br>\n\
		\t\t<table align=\"center\">\n",expcode,expcode);
	if( strcmp(list_of_filenames[0],"all") ) {
		for(int k=0; k<num_buffers; k++)
			fprintf(fp,"\t\t\t<tr><td><a href=\"%s.html\">Experimental results on %s</a></td></tr>\n",list_of_filenames[k],list_of_filenames[k]);
	}
	else
		for(sett=0; sett<NumSetting; sett++) {
			fprintf(fp,"\t\t\t<tr><td><a href=\"%s.html\">Experimental results on %s</a></td></tr>\n",SETTING_BUFFER[sett],SETTING_BUFFER[sett]);
	}
	fprintf(fp,"\t\t</table>\n\
		\t</body>\n\
		</html>");
   	printf("\n");
   	fclose(fp);
	return 1;
}


