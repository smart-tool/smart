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
#include <sys/stat.h>
//#define NumAlgo 500   // numero di algoritmi da testare
#include "sets.h"

/***********************************************/
/* SELECTS ALGORITHMS FOR EXPERIMENATL RESULTS */
/***********************************************/

void printManual() {
  printf("\n\tSMART UTILITY FOR SELECTING STRING MATCHING ALGORITHMS\n\n");
  printf("\t-desc           with description\n");
  printf("\t-show           shows the list of all algorithms\n");
  printf("\t-which          shows the list of all selected algorithms\n");
  printf("\tALGO            selects/deselects the algorithm ALGO (ex. select "
         "bf)\n");
  printf("\t-all            selects all algorithms\n");
  printf("\t-none           deselects all algorithms\n");
  // printf("\t-group A-B-C:D  group a list of algorithms (separated by a -) in
  // a single algorithm named D\n");
  printf("\t-add ALGO       add the new alorithm ALGO to the set\n");
  printf("\t                the executable file of the new algorithm must be "
         "in /bin\n");
  printf("\t-h              gives this help list\n");
  printf("\n\n");
}

int main(int argc, const char *argv[]) {
  char filename[20], command[100];
  unsigned int i, j;
  int desc = 0;
  int *execute = malloc(sizeof(int) * (NumAlgo + 10));
  unsigned int numalgo = NumAlgo;

  getAlgo(ALGO_NAME, execute);

  /* processing of input parameters */
  if (argc == 1) {
    printf("\n\tNo parameter given. Use -h for help.\n\n");
    free(execute);
    return 0;
  }
  if (!strcmp("-h", argv[1])) {
    printManual();
    free(execute);
    return 0;
  }
  int par = 1;
  while (par < argc) {
    int selected = 0;
    if (par < argc && !strcmp("-desc", argv[par])) {
      desc++;
      par++;
    }
    if (par < argc && !strcmp("-show", argv[par])) {
      int ret = 0;
      par++;
      // shows all algorithms
      printf("The list of all string matching algorithms\n");
      for (i = 0; i < NumAlgo; i++)
        if (ALGO_NAME[i]) {
          char buf[100];
          struct stat st;
          printf("%s", ALGO_NAME[i]);
          if (desc)
            printf("\t\"%s\"", ALGOS[i].desc);

          //NOLINTBEGIN(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
          strncpy(buf, "source/algos/", sizeof(buf));
          strncat(buf, ALGO_NAME[i], SZNCAT(buf));
          strncat(buf, ".c", SZNCAT(buf));
          int res = stat(buf, &st);
          if (res && ALGOS[i].missing) // not there, and marked as missing
            printf("\t(missing %s)\n", buf);
          else if (res && !ALGOS[i].missing) { // not there and not marked as missing
            printf("\tFAIL missing %s\n", buf); ret++;
          }
          else if (!res && ALGOS[i].missing) { // there and marked as missing
            printf("\tFAIL exist %s, but marked as missing\n", buf);
            ret++;
          }
          else
            printf("\n");
        }
      return ret;
    }
    if (par < argc && !strcmp("-which", argv[par])) {
      int ret = 0;
      par++;
      // shows all selected algorithms
      printf("\nThe list of selected algorithms:\n");
      for (i = 0; i < NumAlgo; i++)
        if (ALGO_NAME[i] && execute[i]) {
          char buf[100];
          struct stat st;
          printf("\t-%s", ALGO_NAME[i]);
          if (desc)
            printf("\t\"%s\"", ALGOS[i].desc);

          strncpy(buf, "source/algos/", sizeof(buf));
          strncat(buf, ALGO_NAME[i], SZNCAT(buf));
          strncat(buf, ".c", SZNCAT(buf));
          int res = stat(buf, &st);
          if (res && ALGOS[i].missing) // not there, and marked as missing
            printf("\t(missing %s)\n", buf);
          else if (res && !ALGOS[i].missing) { // not there and not marked as missing
            printf("\tFAIL missing %s\n", buf);
            ret++;
          }
          else if (!res && ALGOS[i].missing) { // there and marked as missing
            printf("\tFAIL exist %s, but marked as missing\n", buf);
            ret++;
          }
          else
            printf("\n");
        }
      printf("\n");
      free(execute);
      return ret;
    }
    if (par < argc && !strcmp("-add", argv[par])) {
      par++;
      if (par >= argc) {
        printf("\n\n\tError in input parameters. Use -h for help.\n\n");
        free(execute);
        return 0;
      }
      char *algo = (char*)argv[par++];
      strncpy(filename, algo, sizeof(filename));
      char path[50] = "bin/";
      strncat(path, algo, SZNCAT(path));
      FILE *fp = fopen(path, "r");
      if (fp) {
        // the file exists
        if (search_ALGO(ALGO_NAME, algo) >= 0)
          printf("\n\n\tSMART error message\n\tError in input "
                 "parameters....algorithm %s already in the set\n\n",
                 filename);
        else {
          printf("\n\n\tAdding the algorithm %s to SMART\n", algo);
          fflush(stdout);
          printf("\tTesting the algorithm for correctness....");
          fflush(stdout);
          // testing correctness of the algorithm
          sprintf(command, "./test %s -nv", algo);
          fflush(stdout);
          if (system(command)) {
            printf("\n%s failed!\n", command);
            printf("\tThe system is unable to add the algorithm %s to "
                   "SMART.\n\tPlease, check for algorithm's correctness.\n\n",
                   algo);
          } else {
            printf("ok\n");
            i = NumAlgo;
            EXECUTE[i] = 0;
            execute[i] = 1;
            ALGO_NAME[i] = algo;
            numalgo++;
            printf("\tAlgorithm %s added succesfully.\n\n", algo);
          }
        }
      } else
        printf("\n\n\tSMART error message\n\tError in input "
               "parameters....program %s does not exist\n\n",
               path);
    }
    if (par < argc && !strcmp("-group", argv[par])) {
      par++;
      if (par >= argc) {
        printf("\n\n\tError in input parameters. Use -h for help.\n\n");
        free(execute);
        return 0;
      }
      char *algo = (char *)argv[par++];

      strncpy(filename, algo, sizeof(filename));
      char path[50] = "bin/";
      strncat(path, filename, SZNCAT(path));
      FILE *fp = fopen(path, "r");
      if (fp) {
        // the file exists
        if (search_ALGO(ALGO_NAME, algo) >= 0)
          printf("\n\n\tSMART error message\n\tError in input "
                 "parameters....algorithm %s already in the set\n\n",
                 algo);
        else {
          printf("\n\n\tAdding the algorithm %s to SMART\n", algo);
          fflush(stdout);
          printf("\tTesting the algorithm for correctness....");
          fflush(stdout);
          // testing correctness of the algorithm
          snprintf(command, sizeof(command), "./test %s -nv", algo);
          //NOLINTEND(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
          fflush(stdout);
          if (system(command)) {
            printf("\n%s failed!\n", command);
            printf("\tThe system is unable to add the algorithm %s to "
                   "SMART.\n\tPlease, check for algorithm's correctness.\n\n",
                   algo);
          } else {
            printf("ok\n");
            i = NumAlgo;
            EXECUTE[i] = 0;
            execute[i] = 1;
            ALGO_NAME[i] = algo;
            numalgo++;
            printf("\tAlgorithm %s added.\n\n", algo);
          }
        }
      } else
        printf("\n\n\tSMART error message\n\tError in input "
               "parameters....program %s does not exist\n\n",
               path);
    }
    for (i = 0; i < numalgo; i++)
      if (ALGO_NAME[i]) {
        if (par < argc && !strcmp(argv[par], ALGO_NAME[i])) {
          par++;
          if (execute[i] == 0) {
            execute[i] = 1;
            printf("\tThe %s algorithm has been selected\n", ALGO_NAME[i]);
          } else {
            execute[i] = 0;
            printf("\tThe %s algorithm has been deselected\n", ALGO_NAME[i]);
          }
          selected = 1;
        }
      }
    if (selected)
      continue;
    if (par < argc && !strcmp("-all", argv[par])) {
      par++;
      for (i = 0; i < NumAlgo; i++)
        if (ALGO_NAME[i])
          execute[i] = 1;
      continue;
    }
    if (par < argc && !strcmp("-none", argv[par])) {
      par++;
      for (i = 0; i < NumAlgo; i++)
        if (ALGO_NAME[i])
          execute[i] = 0;
      continue;
    }
    if (par < argc) {
      printf("\tError in input parameters....no parameter %s\n\n", argv[par]);
      free(execute);
      return 0;
    }
  }
  // store only the changes from the default
  FILE *fp = fopen("source/algorithms.lst", "w");
  for (j = 0; j < numalgo; j++)
    if (ALGO_NAME[j] && (j >= NumAlgo || ALGOS[j].execute != execute[j]))
      fprintf(fp, "#%d #%s \n", execute[j], ALGO_NAME[j]);
  fclose(fp);
  free(execute);
}
