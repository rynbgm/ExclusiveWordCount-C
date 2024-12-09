#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include "bst.h"
#include "holdall.h"

#define WORD_LENGTH_MAX 150
#define STR(s)  #s
#define XSTR(s) STR(s)
#define TRACK fprintf(stderr, "*** %s:%d\n", __func__, __LINE__)
#define HELP print_help()
#define TRY fprintf(stdout,"Try 'xwc --help' for more information.\n\n")
//13,818,832

//structure pour les mots qu'on va lire depuis les fichers
typedef struct cword cword;
struct cword {
  char mot[WORD_LENGTH_MAX];
  int occurance;
  int source;
};
//structure pour avoir les info sur chaque otpion sur
//la ligne de commande


//fonction pour comparer les cell du mot renvoi 0 si c'est le meme mot
//renvoi -1 si c'est pas le meme mot mais source diffenrete
//renvoi 1 si
//int cword_compar(cword * c1, cword *c2);
int compar_mot(const void *c1, const void *mot);

//  rfree : libère la zone mémoire pointée par ptr et renvoie zéro.
//static int rfree(void *ptr);
int rfree(void *ptr);

//  scptr_display : affiche sur la sortie standard *cptr, le caractère
//    tabulation, la chaine de caractères pointée par s et la fin de ligne.
//    Renvoie zéro en cas de succès, une valeur non nulle en cas d'échec.
int display(void *ref);

//fonction qui sert a affciher l'aide sur la sorie sur la sortie satandard
void print_help();

int main(int argc, char *argv[]) {
  //faire les test des option et le reste
  //int opt=0;

  //while ((opt = getopt(argc, argv, "?w:ha")) != 1) {
    //switch (opt) {
      //case '?':
        //print_help();
        //exit(0);
      //case'w':
      //case'a':
      //case'h':
        //fprintf(stderr,"Undeveloped option \n");
        //TRY;
        //exit(0);
      //default:
        //print_help();
        //exit(0);
    //}
  //}
  //////////////////////////////////
  //TRACK;
  char word[WORD_LENGTH_MAX + 1];
  bst *xwbst = bst_empty((int (*)(const void *, const void *))compar_mot);
  holdall *xwha = holdall_empty();
  //arbre des mots en commun
  int f = 1;
  while (f < argc) {
    FILE *file = fopen(argv[f], "r");
    if (file == NULL) {
      //TRACK;
      fprintf(stderr, "erreur lecute ficher..");
      exit(1);
    }

    while (fscanf(file, "%" XSTR(WORD_LENGTH_MAX) "s", word) == 1) {
      //while (fscanf(file, "%s", word) == 1) {
      void *recherche = bst_search(xwbst, word);
      if (recherche == NULL) {
        //donc pas dans l'arbre
        cword *p = malloc(sizeof *p);
        if (p == NULL) {
          fprintf(stderr, "erreur allocation cword");
          return -2;
        }
        strcpy(p->mot, word);
        p->source = f;
        p->occurance = 1;
        void *t = bst_add_endofpath(xwbst, p);
        if (t == NULL) {
          //TRACK;
          fprintf(stderr, "erreur lors de l'ajoute du mot %s", word);
          //en vrai faut faire un goto apres free et tt free
          free(p);
          return -2;
        }
        holdall_put(xwha, p);
      } else {
        //donc est dans l'arbre rechere a l'adresse de la cell
        if (((cword *) recherche)->source != f) {
          //donc pas le meme ficher
          ((cword *) recherche)->source = -1;
        } else {
          ((cword *) recherche)->occurance += 1;
        }
      }
    }
    //int holdall_apply_context(holdall *ha,
    //void *context, void *(*fun1)(void *context, void *ptr),
    //int (*fun2)(void *ptr, void *resultfun1)) {
    //for (const choldall *p = ha->head; p != NULL; p = p->next) {
    //int r = fun2(p->ref, fun1(context, p->ref));
    //if (r != 0) {
    //return r;
    //}
    //}
    //return 0;
    //}
    //if (holdall_apply_context(xwha,
    //cword, (void *(*)(void *, void *))compar_source,
    //(int (*)(void *, void *))scptr_display) != 0) {
    //return -1;
    //}
    //fprintf(stdout,"ccount: %zu\n",holdall_count(xwha));
    fclose(file);
    f++;
  }
  bst_dispose(&xwbst);
  holdall_sort(xwha,compar_mot);
  holdall_apply(xwha, display);
  holdall_apply(xwha, rfree);
  holdall_dispose(&xwha);
  //print_help();
}

int compar_mot(const void *c1, const void *mot) {
  return strcoll(((cword*)c1)->mot, mot);
}

#define TAB_SPACE 4
//ma display a besoin de
int display(void *ref) {
  if (((cword *) ref)->source != -1) {
    return printf("%s\t%d\n",
        ((cword *) ref)->mot,
        ((cword *) ref)->occurance) < 0;
  }
  return 1;
  //return qlq chose jsp
}

int rfree(void *ptr) {
  free(ptr);
  return 0;
}

void print_help() {
  fprintf(stdout,
      "Usage: xwc [OPTION]... [FILE]...\n\n"
      "Exclusive word counting. Print the number of occurrences"
      "of each word that appears in one and only one of given text FILES.\n\n"
      "A word is, by default, a maximum length sequence of characters that do not belong"
      "to the white-space characters set.\n\n"
      "Results are displayed in columns on the standard output. Columns are separated by "
      "the tab character. Lines are terminated by the end-of-line character. A header"
      " line shows the FILE names: the name of the first FILE appears in the second "
      "column, that of the second in the third, and so on. For the following lines,"
      " a word appears in the first column, its number of occurrences in the FILE in"
      " which it appears to the exclusion of all others in the column associated with "
      "the FILE. No tab characters are written on a line after the number of"
      " occurrences.\n\n Read the standard input when no FILE is given or for any "
      "FILE which is \"-\". In such cases, \"\" is displayed in the column associated"
      " with the FILE on the header line.\n\n"
      "The locale specified by the environment affects sort order. Set "
      "'LC_ALL=C' to get the traditional sort order that uses native byte values.\n\n"
      "Mandatory arguments to long options are mandatory for short options"
      " too.\n\n"
      "Program Information\n"
      "  -?, --help    Print this help message and exit.\n\n"
      "      --usage   Print a short usage message and exit.\n\n"
      "      --version    Print version information.\n\n"
      "Processing\n"
      "  -a            Same as --words-processing=avl-binary-tree.\n\n"
      "  -h            Same as --words-processing=hash-table.\n\n"
      "  -w, --words-processing=TYPE    Process the words according to the data structure"
      " specified by TYPE. The available values for TYPE are self explanatory:"
      " 'avl-binary-tree' and 'hash-table'. Default is 'hash-table'.\n\n"
      "Input Control\n"
      "  -i, --initial=VALUE    Set the maximal number of significant initial letters"
      " for words to VALUE. 0 means without limitation. Default is 0.\n\n"
      "  -p, --punctuation-like-space    Make the punctuation characters play the same"
      " role as white-space characters in the meaning of words.\n\n"
      "  -r, --restrict=FILE    Limit the counting to the set of words that appear"
      " in FILE. FILE is displayed in the first column of the header line. If FILE"
      " is \"-\", read words from the standard input; in this case, \"\" is displayed in"
      " first column of the"
      " header line.\n\n"
      "Output Control\n"
      "  -l            Same as --sort=lexicographical.\n\n"
      "  -n            Same as --sort=numeric.\n\n"
      "  -S            Same as --sort=none.\n\n"
      "  -s, --sort=TYPE    Sort the results in ascending order, by default, according to"
      " TYPE. The available values for TYPE are: 'lexicographical', sort on words,"
      " 'numeric', sort on number of occurrences, first key, and words, second key, and"
      " 'none', don't try to sort, take it as it comes. Default is 'none'.\n\n"
      "  -R, --reverse    Sort in descending order on the single or first key instead of"
      " ascending order. This option has no effect if the -S option is enable.\n"
      );
}

//while (i < argc) {
//FILE *file = fopen(argv[i], "r");
//if (file == NULL) {
//fprintf(stderr, "erreur lecture ficher %d...\n", i);
//exit(1);
//}
//sinon on va lire les fichers mot par mot
//while (fscanf(file, "%" XSTR(WORD_LENGTH_MAX) "s", word) == 1) {
//faire le taff ici
//if (bst_search(cwbst, word) == NULL) {
//donc la ya pas le mot dans les mot en commun
//donc on l'ajout dans xwbst
//bst_add_endofpath(xwbst, word);
//}
//}
//if (fclose(file) != 0) {
//fprintf(stderr, "erreur fermeture ficher");
//}
//i++;
//}

