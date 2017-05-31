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
 * This program generate random text buffers of size 5MB
 * It is called by makefile script, when compiling SMART
 */

int main(int argc, const char *argv[]) {
    FILE *stream;
    unsigned char c;
    
    for(int sigma=2; sigma<=256; sigma*=2) {
        printf("\tGenerating random text over %.3d chars ...........[000%%]",sigma);
        if(sigma==2) stream = fopen("data/rand2/rand2.txt", "w");
        if(sigma==4) stream = fopen("data/rand4/rand4.txt", "w");
        if(sigma==8) stream = fopen("data/rand8/rand8.txt", "w");
        if(sigma==16) stream = fopen("data/rand16/rand16.txt", "w");
        if(sigma==32) stream = fopen("data/rand32/rand32.txt", "w");
        if(sigma==64) stream = fopen("data/rand64/rand64.txt", "w");
        if(sigma==128) stream = fopen("data/rand128/rand128.txt", "w");
        if(sigma==256) {
            sigma = 250;
            stream = fopen("data/rand250/rand250.txt", "w");
        }
        for(int i=0; i<5000000; i++) {
            c = rand() % sigma;
            fputc(c, stream);
            if(i%1000==0) printf("\b\b\b\b\b\b[%.3d%%]",i*100/5000000); fflush(stdout);
        }
        fclose(stream);
        printf("\b\b\b\b\b\b..[OK]\n");
    }
    return 0;
}

