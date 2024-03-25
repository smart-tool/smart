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
 * contact the authors at: faro@dmi.unict.it and thierry.lecroq@univ-rouen.fr
 * download the tool at: http://www.dmi.unict.it/~faro/smart/
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "algos/include/define.h"
#include "sets.h"
/* OUTPUT VARIABLES */
unsigned int MINLEN = 1,
             MAXLEN = 4200; // min length and max length of pattern size
#include "output.h"
#include "timer.h"

//#define XSIZE 4200       // maximal length of the pattern
//#define SIGMA 256        // constant alphabet size
#define MG (1024 * 1024) // costant for 1 MB size
#define MAXTIME 999.00

#ifndef BINDIR
#define BINDIR "bin"
#endif

struct options {
  unsigned simple :1;
  unsigned dif :1;
  unsigned occ :1;
  unsigned pre :1;
  unsigned txt :1;
  unsigned std :1;
  unsigned _short :1;
  unsigned vshort :1;
  unsigned php :1;
  unsigned tex :1;
  unsigned tb :1;
  int limit;
} options;

//NOLINTBEGIN(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)

void printManual() {
  if (system("./logo"))
    exit(1);
  printf("\tThis is a basic help guide for using the tool\n\n");
  // printf("\t-alpha S      sets to S the size of the alphabet (default
  // 256)\n");
  printf("\t-pset N       computes running times as the mean of N runs "
         "(default 500)\n");
  printf("\t-tsize S      set the upper bound dimension (in Mb) of the text "
         "used for experimental results (default 1Mb)\n");
  printf("\t-plen L U     test only patterns with a length between L and U "
         "(included).\n");
  printf("\t-text F[:F...] performs experimental results using text buffer(s) F "
         "(mandatory unless you use the -simple parameter)\n");
  printf("\t              Use option \"all\" to performe experimental results "
         "using all text buffers.\n");
  printf("\t              Use the style A:B:C to performe experimental results "
         "using multiple text buffers.\n");
  printf("\t              Separate the list of text buffers using the symbol "
         "\":\"\n");
  printf("\t-short        computes experimental results using short length "
         "patterns (from 2 to 32)\n");
  printf("\t-vshort       computes experimental results using very short "
         "length patterns (from 1 to 16)\n");
  printf("\t-occ          prints the average number of occurrences\n");
  printf("\t-pre          computes separately preprocessing times and "
         "searching times\n");
  printf("\t-tb L         set to L the upper bound for any wort case running "
         "time (in ms). The default value is 300 ms\n");
  printf(
      "\t-dif          prints the number the best and the worst running time "
      "(they refer to searching time if the -pre option is selected)\n");
  printf("\t-std          prints the standard deviations of the running times "
         "(it refers to searching time if the -pre option is selected)\n");
  printf("\t-txt          output results in txt tabular format\n");
  printf("\t-tex          output results in latex tabular format\n");
  printf("\t-simple P T   executes a single run searching T (max 1000 chars) "
         "for occurrences of P (max 100 chars)\n");
  printf("\t-h            gives this help list\n");
  printf("\n\n");
}

void printTopEdge(int len) {
  int i;
  fprintf(stdout, "\t");
  for (i = 0; i < len; i++)
    fprintf(stdout, "%c", '_');
  fprintf(stdout, "\n");
}

int getAlpha(char *filename) {
  unsigned int i = 0;
  while (i < NumSetting && strcmp(SETTING_BUFFER[i], filename))
    i++;
  if (i == NumSetting) {
    printf("\tError in loading alphabet size\n");
    return 0;
  } else
    return SETTING_ALPHA_SIZE[i];
}

void generateCode(char *code) {
  unsigned t = (unsigned)time(NULL);
  sprintf(code, "EXP%u", t);
}

#ifndef HAVE_SHM
int execute(enum algo_id algo, unsigned char *P, int m, unsigned char *T, int n) {
  char command[100];
  snprintf(command, sizeof(command), "./%s/%s %s %d %s %d", BINDIR,
           ALGO_NAME[algo], P, m, T, n);
  return system(command);
}
#else
int execute(enum algo_id algo, int m, int n, int *count) {
  char command[100];
  snprintf(command, sizeof(command), "./%s/%s shared %d %d %d %d %d %d %d",
           BINDIR, ALGO_NAME[algo], shmids[shm_P].key, m, shmids[shm_T].key, n,
           shmids[shm_r].key, shmids[shm_e].key, shmids[shm_pre].key);
  // TODO fork/exec with timeout
  int res = system(command);
  if (!res)
    return (*count);
  else
    return -1;
}
#endif

/********************************************************/

int run_setting(char *filename, unsigned char *T, int n, int alpha,
                int *FREQ, unsigned VOLTE, struct options options,
                char *code, unsigned char *simplePattern,
                char *time_format) {
  // performs experiments on a text
  int m, occur, total_occur;
  unsigned int i, j, k, il;
  enum algo_id algo;
  double TIME[NumAlgo][NumPatt], PRE_TIME[NumAlgo][NumPatt],
      BEST[NumAlgo][NumPatt], WORST[NumAlgo][NumPatt], STD[NumAlgo][NumPatt],
      STDTIME[5000];
  unsigned char **setP =
      (unsigned char **)malloc(sizeof(unsigned char *) * VOLTE);
  for (i = 0; i < VOLTE; i++)
    setP[i] = (unsigned char *)malloc(sizeof(unsigned char) * (XSIZE + 1));
  unsigned char *P = NULL;
  FILE *stream = NULL;
  int SIMPLE = (strcmp((char *)simplePattern, "") ? 1 : 0);
  char buf[40] = {0};
  double *e_time = NULL, *pre_time = NULL;
  int *count = NULL;

  if (!SIMPLE) {
    char logfile[100];
    sprintf(logfile, "results/%s", code);
#ifndef _WIN32
    mkdir(logfile, 0775);
#else
    mkdir(logfile);
#endif
    strncat(logfile, "/errorlog.txt", SZNCAT(logfile));
    stream = freopen(logfile, "a", stderr); // redirect of stderr
  }

  // allocate space for running time in shared memory
  srand(time(NULL));
  // allocate in shared memory
  count = shmalloc(shm_r, sizeof(int)); // number of occurrences
  e_time = shmalloc(shm_e, sizeof(double)); // running time
  pre_time = shmalloc(shm_pre, sizeof(double)); // preprocessing
  
  memset(FREQ, 0, SIGMA * sizeof(int));

  // initializes the vector which will contain running times
  for (i = 0; i < NumAlgo; i++) {
    for (j = 0; j < NumPatt; j++) {
      TIME[i][j] = PRE_TIME[i][j] = WORST[i][j] = STD[i][j] = 0;
      BEST[i][j] = MAXTIME;
    }
  }

  // count how many algorithms are going to run
  int num_running = 0;
  for (i = 0; i < NumAlgo; i++)
    if (EXECUTE[i])
      num_running++;

  // i=system("./logo");
  for (il = 0; PATT_SIZE[il] > 0; il++) {
    if (PATT_SIZE[il] >= MINLEN &&
        PATT_SIZE[il] <= MAXLEN &&
        PATT_SIZE[il] <= (unsigned)n) {
      m = PATT_SIZE[il];
      if (m > XSIZE || m < 1 || m > n) {
        fprintf(stderr, "Invalid m %d\n", m);
        continue;
      }
      P = shmalloc(shm_P, m + 1); // pattern
      // ensures count >= 1 at some random offset
      setOfRandomPatterns(setP, m, T, n, VOLTE, simplePattern);
      printf("\n");
      printTopEdge(60);
      if (!options.simple)
        printf("\tExperimental results on %s: %s\n", filename, code);
      else
        printf("\tExperimental results on %s\n", T);
      printf("\tSearching for a set of %u patterns with m=%d in n=%d\n", VOLTE, m, n);
      printf("\tTesting %d algorithms\n", num_running);
      printf("\n");

      int current_running = 0;
      for (algo = 0; algo < NumAlgo; algo++) {
        if (EXECUTE[algo] && (!ALGOS[algo].minlen || m >= ALGOS[algo].minlen)) {
          char *upname = str2upper(ALGO_NAME[algo]);
          current_running++;
          if (!options.simple)
            fprintf(stream, "%s - %s - %d\n", ALGO_NAME[algo], filename, m);
          snprintf(buf, sizeof(buf), "\t - [%d/%d] %s ", current_running,
                   num_running, upname);
          printf("%s", buf);
          fflush(stdout);
          for (i = 0; i < 35 - strlen(buf); i++)
            printf(".");
          total_occur = 0;
          if ((algo == _MUSL || algo == _LIBC) &&
              (memchr(P, 0, m) || memchr(T, 0, n))) {
            printf("\b\b\b\b\b.[SKIP] \n");
            continue;
          }
          for (k = 1; k <= VOLTE; k++) {
            for (j = 0; j <= (unsigned)m; j++)
              P[j] = setP[k - 1][j];
            P[j] = '\0'; // creates the pattern
            int perc = (int)((100 * k) / VOLTE);
            if (perc < 10)
              printf("\b\b\b\b[%d%%]", perc);
            else if (perc < 100)
              printf("\b\b\b\b\b[%d%%]", perc);
            else
              printf("\b\b\b\b[%d%%]", perc);
            fflush(stdout);

            (*e_time) = (*pre_time) = 0.0;
#ifndef HAVE_SHM
            occur = execute(algo, P, m, T, n);
#else
            occur = execute(algo, m, n, count);
#endif
            if (!options.pre)
              (*e_time) += (*pre_time);
            STDTIME[k] = (*e_time);
            TIME[algo][il] += (*e_time);
            PRE_TIME[algo][il] += (*pre_time);
            if (BEST[algo][il] > (*e_time))
              BEST[algo][il] = (*e_time);
            if (WORST[algo][il] < (*e_time))
              WORST[algo][il] = (*e_time);
            total_occur += occur;
            fflush(stderr);
            if (occur <= 0 && (!SIMPLE)) {
              // timer_stop(_timer);
              TIME[algo][il] = 0;
              PRE_TIME[algo][il] = 0;
              total_occur = occur;
              break;
            }
            if ((*e_time) > options.limit) {
              // timer_stop(_timer);
              TIME[algo][il] = 0;
              PRE_TIME[algo][il] = 0;
              total_occur = -2;
              break;
            }
          }

          TIME[algo][il] /= (double)VOLTE;
          PRE_TIME[algo][il] /= (double)VOLTE;
          for (k = 1; k <= VOLTE; k++)
            STD[algo][il] += pow(STDTIME[k] - TIME[algo][il], 2.0);
          STD[algo][il] /= (double)VOLTE;
          STD[algo][il] = sqrt(STD[algo][il]);

          if (total_occur > 0 || (total_occur >= 0 && SIMPLE)) {
            /*
            int nchar = 15;
            if (dif)
              nchar += 20;
            if (std)
              nchar += 15;
            */
            unsigned i;
            printf("\b\b\b\b\b\b\b.[OK]  ");
            if (options.pre)
              snprintf(buf, sizeof(buf), "\t\%.2f + \%.2f ms", PRE_TIME[algo][il],
                      TIME[algo][il]);
            else
              snprintf(buf, sizeof(buf), "\t\%.2f ms", TIME[algo][il]);
            printf("%s", buf);
            for (i = 0; i < 20 - strlen(buf); i++)
              printf(" ");
            if (options.dif) {
              snprintf(buf, sizeof(buf), " [%.2f, %.2f]", BEST[algo][il], WORST[algo][il]);
              printf("%s", buf);
              for (i = 0; i < 20 - strlen(buf); i++)
                printf(" ");
            }
            if (options.std) {
              snprintf(buf, sizeof(buf), " std %.2f", STD[algo][il]);
              printf("%s", buf);
              for (i = 0; i < 15 - strlen(buf); i++)
                printf(" ");
            }
            if (options.occ)
              printf("\tocc \%u", total_occur / VOLTE);
            printf("\n");
          } else if (total_occur == 0)
            printf("\b\b\b\b\b\b\b\b.[ERROR] \n");
          else if (total_occur == -1)
            printf("\b\b\b\b\b.[--]  \n");
          else if (total_occur == -2)
            printf("\b\b\b\b\b\b.[OUT]  \n");
        }
      }
#ifdef HAVE_SHM
      shmdt(P);
      shmctl(shmids[shm_P].id, IPC_RMID, 0);
#else
      free(P);
#endif
    }
  }

  printf("\n");
  printTopEdge(60);
  if (!options.simple) {
    fclose(stream);
    printf("\tOUTPUT RUNNING TIMES %s\n", code);
    outputXML(TIME, alpha, filename, code);
    outputHTML2(PRE_TIME, TIME, BEST, WORST, STD, options.pre, options.dif, alpha, n, VOLTE,
                filename, code, time_format);
    if (options.txt)
      outputTXT(TIME, alpha, filename, code, time_format);
    if (options.tex)
      outputLatex(TIME, alpha, filename, code, time_format);
    if (options.php)
      outputPHP(TIME, BEST, WORST, STD, alpha, filename, code, options.dif, options.std);
  }
  // free memory allocated for patterns
  for (i = 0; i < VOLTE; i++)
    free(setP[i]);
  free(setP);
  free_shm(NULL, P, count, e_time, pre_time);
  return 0;
}

/**************************************************/
int FREQ[SIGMA]; // frequency of alphabet characters

int main(int argc, const char *argv[]) {
  // mandatory parameters:
  char filename[100] = {0};
  // optional parameters:
  PATT_SIZE = PATT_LARGE_SIZE; // the set of pattern lengths (max 4096)
  int alpha = 256;             // size of the alphabet, -alpha
  int VOLTE = 500;             // number of runs for each pattern length
  int TSIZE = 1048576;
  unsigned char simplePattern[100]; // used for the simple run of SMART
  unsigned char simpleText[1000];   // used for the simple run of SMART
  /* useful variables */
  unsigned char *T = NULL; // text
  int n;           // length of the text
  char parameter[1000];

  memset(&options, 0, sizeof(options));
  options.limit = 300; // running time bound
  srand(time(NULL));

  /* processing of input parameters */
  if (argc == 1) {
    printf("No parameter given. Use -h for help.\n\n");
    goto end;
  }
  if (!strcmp("-h", argv[1])) {
    printManual();
    goto end;
  }
  int par = 1;
  while (par < argc) {
    if (par < argc && !strcmp("-pset", argv[par])) {
      par++;
      if (par >= argc) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      strncpy(parameter, argv[par++], SZNCPY(parameter));
      if (!isInt(parameter)) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      VOLTE = string2decimal(parameter);
    }
    if (par < argc && !strcmp("-alpha", argv[par])) {
      par++;
      if (par >= argc) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      strncpy(parameter, argv[par++], SZNCPY(parameter));
      if (!isInt(parameter)) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      alpha = string2decimal(parameter);
      if (alpha > 255 || alpha <= 0) {
        printf("Error in alpha input parameter. 0 > alpha < 256.\n\n");
        goto end;
      }
    }
    if (par < argc && !strcmp("-tsize", argv[par])) {
      par++;
      if (par >= argc) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      strncpy(parameter, argv[par++], SZNCPY(parameter));
      if (!isInt(parameter)) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      TSIZE = string2decimal(parameter);
      TSIZE *= MG;
    }
    if (par < argc && !strcmp("-tb", argv[par])) {
      par++;
      if (par >= argc) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      strncpy(parameter, argv[par++], SZNCPY(parameter));
      if (!isInt(parameter)) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      options.limit = string2decimal(parameter);
    }
    if (par < argc && !strcmp("-text", argv[par])) {
      par++;
      if (par >= argc) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      strncat(filename, argv[par++], SZNCAT(filename));
    }
    if (par < argc && !strcmp("-plen", argv[par])) {
      par++;
      if (par >= argc) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      strncpy(parameter, argv[par++], SZNCPY(parameter));
      MINLEN = string2decimal(parameter);

      if (MINLEN < 1 || MINLEN > 4200) {
        printf("Error in input parameters. The minimum length is not a valid "
               "argument.\n\n");
        goto end;
      }
      if (par >= argc) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      strncpy(parameter, argv[par++], SZNCPY(parameter));
      MAXLEN = string2decimal(parameter);

      if (MAXLEN < 1 || MINLEN > MAXLEN) {
        printf("Error in input parameters. The maximum length is not a valid "
               "argument.\n\n");
        goto end;
      }
    }
    if (par < argc && !strcmp("-simple", argv[par])) {
      par++;
      if (par >= argc) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      if (strlen(argv[par]) >= 100) {
        printf("Error in input parameters. Max 100 chars for P parameter.\n\n");
        goto end;
      }
      strncpy((char *)simplePattern, argv[par++], SZNCPY(simplePattern));
      if (par >= argc) {
        printf("Error in input parameters. Use -h for help.\n\n");
        goto end;
      }
      if (strlen(argv[par]) >= 1000) {
        printf(
            "Error in input parameters. Max 1000 chars for T parameter.\n\n");
        goto end;
      }
      strncpy((char *)simpleText, argv[par++], SZNCPY(simpleText));
      options.simple = 1;
    }
    if (par < argc && !strcmp("-occ", argv[par])) {
      par++;
      options.occ = 1;
    }
    if (par < argc && !strcmp("-pre", argv[par])) {
      par++;
      options.pre = 1;
    }
    if (par < argc && !strcmp("-dif", argv[par])) {
      par++;
      options.dif = 1;
    }
    if (par < argc && !strcmp("-txt", argv[par])) {
      par++;
      options.txt = 1;
    }
    if (par < argc && !strcmp("-std", argv[par])) {
      par++;
      options.std = 1;
    }
    if (par < argc && !strcmp("-tex", argv[par])) {
      par++;
      options.tex = 1;
    }
    if (par < argc && !strcmp("-php", argv[par])) {
      par++;
      options.php = 1;
    }
    if (par < argc && !strcmp("-short", argv[par])) {
      par++;
      PATT_SIZE = PATT_SHORT_SIZE;
    }
    if (par < argc && !strcmp("-vshort", argv[par])) {
      par++;
      PATT_SIZE = PATT_VERY_SHORT;
    }
    if (par < argc && strcmp("-text", argv[par]) != 0 &&
        strcmp("-alpha", argv[par]) != 0 && strcmp("-tsize", argv[par]) != 0 &&
        strcmp("-plen", argv[par]) != 0 && strcmp("-occ", argv[par]) != 0 &&
        strcmp("-pre", argv[par]) != 0 && strcmp("-dif", argv[par]) != 0 &&
        strcmp("-txt", argv[par]) != 0 && strcmp("-tb", argv[par]) != 0 &&
        strcmp("-simple", argv[par]) != 0 && strcmp("-tex", argv[par]) != 0 &&
        strcmp("-std", argv[par]) != 0 && strcmp("-php", argv[par]) != 0 &&
        strcmp("-pset", argv[par]) != 0 && strcmp("-vshort", argv[par]) != 0 &&
        strcmp("-short", argv[par]) != 0) {
      printf("Error in input parameters. Use -h for help.\n\n");
      goto end;
    }
  }
  if (strcmp(filename, "") && options.simple) {
    printf("Error in input parameters. Both parameters -simple and -text "
           "defined.\n\n");
    goto end;
  }
  if (!strcmp(filename, "") && !options.simple) {
    printf("Error in input parameters. No filename given.\n\n");
    goto end;
  }

  // get information about the set of algorithms
  getAlgo(ALGO_NAME, EXECUTE);

  // allocate space for text in shared memory
  const size_t size = sizeof(unsigned char) * TSIZE + 10;
  T = shmalloc(shm_T, size); // text

  if (options.simple) {
    if (system("./logo"))
      perror("logo");
    // experimental results on a single pattern and a single text
    n = strlen((char *)simpleText);
    int m = strlen((char *)simplePattern);
    //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.strcpy)
    strcpy((char *)T, (char *)simpleText);
    //alpha = 250;
    PATT_CUSTOM_SIZE[0] = m;
    PATT_CUSTOM_SIZE[1] = 0;
    PATT_SIZE = PATT_CUSTOM_SIZE;
    
    textStats(T, n, FREQ);
    srand(time(NULL));
    char expcode[32];
    generateCode(expcode);

    time_t date_timer;
    char time_format[26];
    struct tm *tm_info;
    time(&date_timer);
    tm_info = localtime(&date_timer);
    strftime(time_format, 26, "%Y:%m:%d %H:%M:%S", tm_info);
    printf("\tExperimental tests started on %s\n", time_format);

    printf("\tStarting experimental tests with code %s\n", expcode);
    run_setting("", T, n, alpha, FREQ, VOLTE, options, expcode,
                simplePattern, time_format);
    // no output is given for the simple case;
  } else if (strcmp(filename, "all")) {
    // experimental results on a list of text buffers
    if (system("./logo"))
      perror("logo");
    char list_of_filenames[NumSetting][50];
    int num_buffers = split_filelist(filename, list_of_filenames);

    srand(time(NULL));
    char expcode[32];
    generateCode(expcode);
    printf("\tStarting experimental tests with code %s\n", expcode);

    for (int k = 0; k < num_buffers; k++) {
      printf("\n\tTry to process archive (%d/%d) %s\n", k + 1, num_buffers,
             list_of_filenames[k]);
      char fullpath[800];
      snprintf(fullpath, sizeof(fullpath), "data/%s", list_of_filenames[k]);
      // initialize the text and frequency vector
      if (!(n = getText(T, fullpath, FREQ, TSIZE))) {
        goto end_shm;
      }
      if (!(alpha = getAlpha(list_of_filenames[k]))) {
        goto end_shm;
      }

      time_t date_timer;
      char time_format[26];
      struct tm *tm_info;
      time(&date_timer);
      tm_info = localtime(&date_timer);
      strftime(time_format, 26, "%Y:%m:%d %H:%M:%S", tm_info);
      printf("\tExperimental tests started on %s\n", time_format);

      run_setting(list_of_filenames[k], T, n, alpha, FREQ, VOLTE, options,
                  expcode, (unsigned char *)"", time_format);
      outputINDEX(list_of_filenames, num_buffers, expcode);
    }
  } else {
    if (system("./logo"))
      perror("logo");
    // starts experimental results on all texts
    char list_of_filenames[NumSetting][50];
    int num_buffers = split_filelist(filename, list_of_filenames);
    srand(time(NULL));
    char expcode[32];
    generateCode(expcode);
    printf("\tStarting experimental tests with code %s\n", expcode);
    for (unsigned sett = 0; sett < NumSetting; sett++) {
      char fullpath[100];
      strncpy(fullpath, "data/", SZNCPY(fullpath));
      strncat(fullpath, SETTING_BUFFER[sett], SZNCAT(fullpath));
      alpha = SETTING_ALPHA_SIZE[sett];
      printf("\n\tTry to process archive %s\n", SETTING_BUFFER[sett]);
      // initialize the frequency vector
      if (!(n = getText(T, fullpath, FREQ, TSIZE))) {
        goto end_shm;
      }
      //printf("\tText buffer of dimension %d byte\n", n);
      time_t date_timer;
      char time_format[26];
      struct tm *tm_info;
      time(&date_timer);
      tm_info = localtime(&date_timer);
      strftime(time_format, 26, "%Y:%m:%d %H:%M:%S", tm_info);
      printf("\tExperimental tests started on %s\n", time_format);

      run_setting(SETTING_BUFFER[sett], T, n, alpha, FREQ, VOLTE, options,
                  expcode, (unsigned char *)"", time_format);
    }
    outputINDEX(list_of_filenames, num_buffers, expcode);
  }

  // free shared memory. only T is remaining
end_shm:
#ifdef HAVE_SHM
  //fprintf(stderr, "shmdt T %p\n", T);
  shmdt(T);
  shmctl(shmids[shm_T].id, IPC_RMID, 0);
#else
  free(T);
#endif
  // free other allocated memory
end:
  return 0;

#ifndef HAVE_SHM
  //end_1:
  //fprintf(stderr, "shmdt T %p\n", T);
  shmdt(T);
  shmctl(shmids[shm_T].id, IPC_RMID, 0);
  return 1;
#endif
}

//NOLINTEND(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
