#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <locale.h>
#include "bst.h"
#include "holdall.h"

//probleme avec les allocation
//realloc ne marche pas

//-------------------------------GENERALE----------------------------------------
#define WORD_LENGTH_INITIALE 200
#define STR(s)  #s
#define XSTR(s) STR(s)
#define TRACK fprintf(stderr, "*** %s:%d\n", __func__, __LINE__)
#define ANSI_BG_RED "\033[43m"  // Définit la couleur de fond en rouge
#define ANSI_BG_RESET "\033[49m"  // Réinitialise la couleur de fond au défaut
                                  // du terminal
#define HIGHLIGHT(text, f) fprintf(stdout, \
    ANSI_BG_RED "%s %d" ANSI_BG_RESET "\n", text, f)
#define TESTER_ARG(arg, func, ret) \
  if (strcmp(argv[i], (arg)) == 0) { \
    func(); \
    return ret; \
  }

//-------------------------------LES OPTIONS------------------------------------
//#define WITH_ARGS(o) ((char*)o ":")

#define SHORT_HELP "-?"
#define LONG_HELP "--help"
#define VERSION "--version"
#define USAGE "--usage"
#define BINARY_TREE "-a"
#define HASH_TABLE "-h"
#define WORD_PROCES "-w"
#define LEX_SORT "-l"
#define NUM_SORT "-n"
#define REVERSE_SORT "-R"
#define CUT "-i"
#define NO_FILE "-"
#define TRY fprintf(stderr, "Try 'xwc %s' for more information.\n", LONG_HELP)

//-------------------------STRUCTURE CWORD--------------------------------

#define CAST(p) ((cword *) (p))
#define OCC(p) (CAST(p)->occurance)
#define SOURCE(p) (CAST(p)->source)
#define MOT(p) ((CAST(p)->mot))

//structure pour les mots qu'on va lire depuis les fichers
typedef struct cword cword;
struct cword {
  char *mot;
  int occurance;
  int source;
};

//cword * new_cword(const char * word, int source);

//-----------------------Fonctions auxiliaires---------------------------------

//int compar_f(const void *c1, const void * c2){
//return strcoll(MOT(c1),MOT(c2));
//}

//compar_mot permet de compar les deux argument renvoi une valeur inferieure
//a 0 si le premier argument est inferieure au second , une valeur positive dans
//le cas contraire ,ps:le premier argument correspond a la cellule qui stocke le
// mot et
int compar_mot(const void *c1, const void *c2);

//deux fonction de meme nature que compar_mot a l'exeption du fait que
//compar_occ (rep:compar_mot_) compare les occurance (les mots) des mots des
// deux cellules
//passees en parametre au sens de la fonction de comparaison strcoll
int compar_occ(const void *c1, const void *c2);
int compar_mot_(const void *c1, const void *c2);
int reverse_compar_occ(const void *c2, const void *c1);

//fonction qui renvoit une valeur inferieure a 0 si x < y,une valeur superieure
// a 0 dans
//le cas contraire ou une valeure nulle si les deux sont egaux
int compar(const void *x, const void *y);

//    display_words : affiche sur la sortie standard *cptr, le caractère
//    tabulation, la chaine de caractères pointée par s et la fin de ligne.
//    Renvoie zéro en cas de succès, une valeur non nulle en cas d'échec.
int display_words(void *ref);

int display_files(int length, char *files[]);

//affiche l'aide/l'usage/la version sur la sortie standard
void help();
void usage();
void version();

//  rfree : libère la zone mémoire pointée par ptr et renvoie zéro.
int rfree(void *ptr);

// Définitions supplémentaires et déclarations de fonctions, le cas échéant
bool processOptionWithArgument(char **argv, int *index, char **value) {
  if (argv[(*index) + 1] != NULL) {  // S'assurer qu'il y a un argument après
                                     // l'option
    *value = argv[*index + 1];   // Affecter l'argument à la variable pointée
                                 // par value
    (*index)++;                  // Incrémenter l'index pour sauter l'argument
                                 // déjà traité
    return true;
  }
  return false;
}

//------------------------------------MAIN-----------------------------------------------

int main(int argc, char *argv[]) {
  int r = EXIT_SUCCESS;
  bool no_file = false; //true si ./projet [-opt] [ - - ...]
  bool l_flag = false;
  bool n_flag = false;
  bool reverse_flag = false;
  //int i_value = 0;
  char *i_value = NULL; //argument pour l'option de decoupage
  char *w_value = NULL; //argument pour l'option de traitement
  char *optionArgument = NULL; //recuper avec les arguments des options
  int opt_count = 0; //compte le nombre d'options
  bool options_ended = false; // pour lire le reste comme non option des ya une
                              // non option
  //-----------------------GESTION DES
  // ARGUMENTS------------------------------------
  for (int i = 1; i < argc; i++) {
    char *arg = argv[i]; //argument en cours de traitement de la ligne de
                         // commande
    if (arg[0] != '-' || options_ended) {
      break; // voir la ligne de declaration de options_ended
    }
    if (strcmp(arg, SHORT_HELP) == 0 || strcmp(arg, LONG_HELP) == 0) {
      help(); // affichage de menu d'aide si --help ou -?
      return r;
    } else if (strcmp(arg, USAGE) == 0) {
      usage();
      return r;
    } else if (strcmp(arg, VERSION) == 0) {
      version();
      return r;
    } else if (strcmp(arg, HASH_TABLE) == 0) {
      fprintf(stderr, "This option %s is not yet implemented\n", arg);
      return r;
    } else if (strcmp(arg, BINARY_TREE) == 0) {
      opt_count++;
    } else if (strcmp(arg, LEX_SORT) == 0) {
      l_flag = true;
      opt_count++;
    } else if (strcmp(arg, NUM_SORT) == 0) {
      n_flag = true;
      opt_count++;
    } else if (strcmp(arg, REVERSE_SORT) == 0) {
      reverse_flag = true;
      opt_count++;
      //fprintf(stdout,"%d",reverse_flag);
    } else if (strcmp(arg, CUT) == 0) {
      if (i + 1 < argc //eviter une erreur d segmentaion
          && processOptionWithArgument(argv, &i, &optionArgument)) {
        i_value = optionArgument;
        opt_count += 2;
      } else {
        fprintf(stderr, "xwc: Missing argument '-i'.\n");
        TRY;
        fprintf(stdout, "val de i : %s\n", i_value); //retier apres
        return EXIT_FAILURE;
      }
    } else if (strcmp(arg, WORD_PROCES) == 0) {
      if (processOptionWithArgument(argv, &i, &optionArgument)) {
        if (strcmp(optionArgument, "avl-binary-tree") != 0
            && strcmp(optionArgument, "hash-table") != 0) {
          fprintf(stderr,
              "Error: Invalid type for -w. Use 'avl-binary-tree' or 'hash-table'.\n");
          return EXIT_FAILURE;
        }
        w_value = optionArgument;
        if (strcmp(w_value, "hash-table") == 0) {
          fprintf(stderr, "This option %s : %s is not yet implemented\n", arg,
              w_value);
          return EXIT_FAILURE;
        } else if (strcmp(w_value, "avl-binary-tree") == 0) {
          opt_count += 2;
        }
      } else {
        fprintf(stderr, "xwc: Missing argument '-w'.\n");
        TRY;
        return 1;
      }
    } else if (strcmp(arg, NO_FILE) == 0) {
      no_file = true;
      options_ended = true;
    } else {
      options_ended = true;
      fprintf(stderr, "xwc: Unrecognized option '%s'.\n", arg);
      TRY;
      return EXIT_FAILURE;
    }
  }
  bst *xwbst = bst_empty(compar_mot);
  holdall *xwha = holdall_empty();
  holdall *wha = holdall_empty();
  cword *cell = NULL;
  if (xwbst == NULL || xwha == NULL) {
    goto error_capacity;
  }
  argv += opt_count + 1;
  int f = 0;
  int total_files = argc - opt_count - 1;
  if ((argc - 1 - opt_count) == 0) {
    no_file = true; //pour dire que ya que des options ou c'est vide
    total_files = 1;  //pour lire un ficher meme si c'est vide
  }
  char *word = malloc(WORD_LENGTH_INITIALE + 1);
  while (f < total_files || (no_file && f < total_files)) {
    //cas ficher      cas stdiin avec des --   cas vide
    //TRACK;
    FILE *file;
    if (no_file || (argc == total_files && !no_file)) {
      HIGHLIGHT("---reading from file number", f - opt_count + 1);
      file = stdin;
    } else { //cas de fichers en argument
      file = fopen(argv[f], "r");
      if (file == NULL) {
        fprintf(stderr, "xwc: Can't open for reading file '%s'.\n", argv[f]);
        goto error;
      }
    }
    //holdall_put(wha,word);
    int ch = 0;
    size_t index_word = 0;
    while ((ch = fgetc(file)) != EOF) { //on va lire un caractère
      if (isspace(ch)) { //si c'est un caractere d'espacement
        if (index_word > 0) { //on a lu un mot hihihi
          word[index_word] = '\0'; //baybay le mot fini sealed
          //preparation pour le suivant
          //donc word a bien le mot quil nous faut
          //sinon veut dire pas de mot c'st juste des espace on fait rien on
          // continu
          // mmm
          //la faire la suite du programme ? car la dans word ya un mot
          //fprintf(stdout, "le mot: %s\n", word);
          //fprintf(stdout, "le size: %zu\n", strlen(word));
          cell = malloc(sizeof *cell); //je crois que c'est une variable globale
                                       // ca non?
          if (cell == NULL) {
            fprintf(stderr, "le probeleme\n");
          }
          //tester
          cell->mot = malloc(strlen(word) + 1);
          cell->mot[index_word] = '\0';
          index_word = 0;
          //tester
          strcpy(cell->mot, word);
          //memcpy(cell->mot, word,strlen(word));
          //fprintf(stdout, "le mot: %s\n", cell->mot);
          cell->source = f;
          //fprintf(stdout,"f = %d\n",f);
          cell->occurance = 1;
          //cell ya tt dakhel
          //void *recherche = bst_search(xwbst, cell); //on cherche le mot de
          // cell
          //cell nest pas nulle c'est sur
          //if (recherche == NULL) { //si null donc ya pas le mot on ajoute ihi
          //cword *p = malloc(sizeof *p);
          //if (p == NULL) {
          //fprintf(stderr, "erreur allocation cword");
          //goto error_capacity;
          //}
          //strcpy(p->mot, word);
          //p->source = f;
          //p->occurance = 1;
          //on a tester cell il est pas null
          void *t = bst_add_endofpath(xwbst, cell); //on ajoute cell ihi
          if (t == NULL) {
            fprintf(stderr, "erreur lors de l'ajoute du mot %s", word);
            goto error_capacity;
          }
          if (t != cell) { //donc ya deja
            if (SOURCE(t) != SOURCE(cell)) {
              SOURCE(t) = -1;
            } else { //donc la meme source
              OCC(t) += 1; //ou t kifkif je crois
            }
            free(cell->mot);
            free(cell);
          } else { //kifkif t et cell donc on vient d'ajouter
            if (holdall_put(xwha, cell) != 0) { //on ajoute au fourtout
              goto error_capacity;
            }
          }
        }
        /////////////////////
      } else { // a cette etape on construit le mot
        word[index_word] = (char) ch;
        index_word += 1; //put di index_word puis avance
        if (index_word >= WORD_LENGTH_INITIALE - 1) { //en vrai == peut pas etre
                                                      // > le pour '/0'
          //on ajoute de la place
          char *word_ = realloc(word, (WORD_LENGTH_INITIALE + 1) * 2 + 1);
          if (word_ == NULL) {
            fprintf(stderr, "probleme\n");
            goto error_capacity;
          }
          //tester ca apres si tt va bien
          word = word_;
        } //ya pas de sinon car aka
      }
      //on va lire un autre char
    }
    if (file != stdin) {
      if (fclose(file) != 0) {
        fprintf(stderr, "Error closing file: %s", strerror(errno));
        goto error;
      }
    } else {
      clearerr(stdin);
      HIGHLIGHT("---end reading from file number", f - opt_count + 1);
      fprintf(stdout, "\n");
      if (argc == f) {
        break;
      }
    }
    f++;
  }
  //pour evite les erreurs de segmentation quand c'est des fichers vides
  if (holdall_count(xwha) == 0) {
    goto error;
  }
  if (l_flag) {
    //tirer l'autre pour voir truc de cle nni
    setlocale(LC_COLLATE, "");
    holdall_sort(xwha, compar_mot);
  }
  if (n_flag) {
    if (reverse_flag) {
      holdall_sort(xwha, reverse_compar_occ);
    } else {
      holdall_sort(xwha, compar_occ);
    }
  }
  //display_files(opt_count + 1, argv); pour - - - ca les compte comme des opt?
  holdall_apply(xwha, display_words);
  goto dispose;
error_capacity:
  fprintf(stderr, "*** Error: Not enough memory\n");
  goto error;
  //error_read:
  //fprintf(stderr, "*** Error: A read error occurs\n");
  //goto error;
  //error_write:
  //fprintf(stderr, "*** Error: A write error occurs\n");
  //goto error;
error:
  r = EXIT_FAILURE;
  goto dispose;
dispose:
  bst_dispose(&xwbst);  // pas de cas d'erreur
  if (xwha != NULL) {
    holdall_apply(xwha, rfree);
  }
  holdall_dispose(&xwha);
  if (wha != NULL) {  //bien refaire c'est les mots
    holdall_apply(wha, rfree);
  }
  holdall_dispose(&wha);
  return r;
}

int compar_mot(const void *c1, const void *c2) {
  return strcoll(MOT(c1), MOT(c2));
}

int compar(const void *x, const void *y) {
  return (x > y) - (x < y);
}

int compar_mot_(const void *c1, const void *c2) {
  return strcoll(MOT(c1), MOT(c2));
}

int compar_occ(const void *c1, const void *c2) {
  if (((OCC(c1) > OCC(c2)) - (OCC(c1) < OCC(c2))) == 0) {
    return compar_mot_(c1, c2) >= 0;
  }
  return (OCC(c1) > OCC(c2)) - (OCC(c1) < OCC(c2));
}

int reverse_compar_occ(const void *c2, const void *c1) {
  if (((OCC(c1) > OCC(c2)) - (OCC(c1) < OCC(c2))) == 0) {
    return compar_mot_(c1, c2) >= 0;
  }
  return (OCC(c1) > OCC(c2)) - (OCC(c1) < OCC(c2));
}

#define TAB 4
//int display_words(void *ref) {
//return fprintf(stdout, "%s%*s%d\n",
//MOT(ref), TAB * (SOURCE(ref) + 1), " ",
//OCC(ref)) < 0;
//return fprintf(stdout,"%s\t%d\n",MOT(ref),OCC(ref)) < 0;
//}

int display_words(void *ref) {
  if (SOURCE(ref) != -1) {
    return fprintf(stdout, "%s\t%d\n", MOT(ref), OCC(ref)) < 0;
  }
  return 0;
}

//int display_files(int debut , char *files[]) {
//int i = debut;
//while (files[i] != NULL) {
//if(strcmp(files[i],NO_FILE)==0){
//TRACK;
//fprintf(stdout, "%*s%s", TAB, " f", "zef");
//}
//fprintf(stdout, "%*s%s", TAB, " ", files[i]);
//i++;
//}
//fprintf(stdout, "\n");
//return 0;
//}

int rfree(void *ptr) {
  free(MOT(ptr));
  free(ptr);
  return 0;
}

//int motfree(coid *ptr){
//free(ptr);
//return 0;
//}

void help() {
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

void usage() {
  printf("Usage: xwc [OPTION]... [FILE]...\n");
}

void version() {
  printf("xwc - 24.05.13\n"
      "This is freeware: you can redistribute it. There is NO WARRANTY.\n");
}

//sur fr
//1,346,118 allocs, 1,346,117 frees, 30,823,964 bytes allocated moi
//1,346,138 allocs, 1,346,138 frees, 30,827,949 bytes allocated hancart
