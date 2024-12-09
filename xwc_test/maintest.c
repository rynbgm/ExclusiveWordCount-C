#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bst.h"
#include "holdall.h"

/////////////facilite la vie/////////////////////

#define MAX_LENGTH_OPTIONS 3
#define INDICE_DEBUT_FICHERS (1 + nbr_options)
#define INDICE_FIN_FICHERS (argc - 1)
#define HELP "-?"
#define USAGE argv[0]
//fprintf(stdout,"%s"  " [OPTION] ... [FILE]...\n",USAGE);
#define VERSION ("This is freeware: you can redistribute it."                  \
  "There is NO WARRANTY\n")
//fprintf(stdout,VERSION);
#define BINARY_TREE "-a"
//#define HASH_TABLE "-h"
//#define w ?
//#define WORD_MAX_LENGTH "-i"
//#define PUNCTUATION "-p"
//#define RESTRICT "-r"
//#define LEX_SORT "-l"
//#define NUM_SORT "-n"
//#define NON_SORT "-S"
//#define un truc - pour l'entre standard
#define OTHER(opt) ("xwc :  Unrecognized option '"opt"'."                      \
                    "Try 'xwc --help' for more information.")                  \

#define STR(s)  #s
#define XSTR(s) STR(s)
#define WORD_LENGTH_MAX 3
#define BORNE 5
//#define IMPLENTED_OPTIONS 3
//#define POSSIBLE_OPTIONS (char * opts[IMPLENTED_OPTIONS]{HELP,BINARY_TREE})


//bool is_opt (char * opt_tab ,char opt) {
  //for ()
  //}

int main(int argc, char *argv[]) {
  int i = 1;
  int j = 0;
  char word[1000];
  if(argc == 1){
    fprintf(stdout,"lecture entree standard ficher 1");
    while (fscanf(stdin,"%" XSTR(WORD_LENGTH_MAX) "s", word) == 1) {
      fprintf(stdout, "%s\n", word);
      j++;
    }
    }

  while (i <= argc) {
    FILE *file = fopen(argv[i], "r");
    if (file == NULL) {
      fprintf(stderr, "erreur lecture ficher %d...\n", i);
        fprintf(stdout, "nombre de mot est :%d \n", j);
      exit(1);
    }
    //sinon on va lire les fichers

    while (fscanf(file,"%" XSTR(WORD_LENGTH_MAX) "s", word) == 1) {
      fprintf(stdout, "%s\n", word);
      j++;
    }
    fclose(file);
    i++;
  }
  fprintf(stdout, "nombre de mot est :%d", j);
  return 0;
}

