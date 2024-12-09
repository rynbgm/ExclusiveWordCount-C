#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <locale.h>
#include "bst.h"
#include "holdall.h"
#include "hashtable.h"

//-------------------------------GENERALE--------------------------------------

#define WORD_LENGTH_INITIALE 26
#define WORD_ADDED_LENGTH 10
#define TRACK fprintf(stderr, "*** %s:%d\n", __func__, __LINE__)
#define ANSI_BG_RED "\033[44m"
#define ANSI_BG_RESET "\033[49m"
#define START_READING_FILE "--- starts reading for FILE"
#define END_READING_FILE "--- ends reading for FILE"
#define HIGHLIGHT(text, f) fprintf(stdout,                                    \
    ANSI_BG_RED "%s %d" ANSI_BG_RESET "\n", text, f)
#define BASE_10 10
#define END_CHAR '\0'
#define END_LINE '\n'
#define NOT_ENOUGH_MEM fprintf(stderr, "xwc: Not enough memory.\n")

//-------------------------------LES OPTIONS-----------------------------------

#define SHORT_HELP "-?"
#define LONG_HELP "--help"
#define VERSION "--version"
#define USAGE "--usage"
#define BINARY_TREE "-a"
#define LONG_BINARY_TREE "avl-binary-tree"
#define HASH_TABLE "-h"
#define LONG_HASH_TABLE "hash-table"
#define WORD_PROCES "-w"
#define LEX_SORT "-l"
#define LONG_LEX "lexicographical"
#define NUM_SORT "-n"
#define LONG_NUM "numeric"
#define REVERSE_SORT "-R"
#define CUT "-i"
#define MAX_CUT_I 10000000
#define PUNCTUATION "-p"
#define NO_FILE "-"
#define SORT "-s"
#define CAP_SORT "-S"
#define NONE_SORT "none"

#define NON_IMPLEMENTED_OPTION(opt) fprintf(stderr,                           \
    "This option %s is not yet implemented\n", opt);

#define TRY fprintf(stderr, "Try 'xwc %s' for more information.\n", LONG_HELP)
#define MISSING_ARG(opt) fprintf(stderr, "xwc: Missing argument '%s'.\n", opt)
#define INVALID_ARG(opt) fprintf(stderr, "xwc: Invalid argument '%s'.\n", opt)
#define OVERFLOW_ARG(opt) fprintf(stderr, "xwc: Overflowing argument '%s'.\n", \
    opt)
#define UNKNOWN_OPT(opt)  fprintf(stderr, "xwc: Unrecognized option '%s'.\n", \
    opt)
#define PRINT_HELP help()
#define PRINT_USAGE fprintf(stdout, "Usage: xwc [OPTION]... [FILE]...\n")
#define  PRINT_VERSION printf("xwc - 24.05.13\n"                              \
    "This is freeware: you can redistribute it. There is NO WARRANTY.\n")

//------------------------------STRUCTURE CWORD--------------------------------

#define CAST(p) ((cword *) (p))
#define OCC(p) (CAST(p)->occurance)
#define SOURCE(p) (CAST(p)->source)
#define MOT(p) ((CAST(p)->mot))

//structure pour les mots qu'on va lire depuis les fichers
//le champ mot fait reference au mot lu et la structure permet
//aussi de stocker les occurances du mot et sa source
typedef struct cword cword;
struct cword {
  char *mot;
  int occurance;
  int source;
};

//-----------------------Fonctions auxiliaires---------------------------------

//ensemble de fonction de comparaison
//compar_mot et reverse_compar_mot comparent les mots dans l'ordre croissant
// et decroissant respectivement selon strcoll
int compar_mot(const void *c1, const void *c2);
int reverse_compar_mot(const void *c2, const void *c1);
//compar_occ_mot compare les mots selon le nombre d'occurances en premiere clé
//et grace a compare_occ_ et selon compar_mot en second clé
int compar_occ_mot(const void *c1, const void *c2);
int compar_occ_(const void *c1, const void *c2);
//la fonction reverse_compar_occ_mot est similaire a compar_occ_mot sauf que
//que l'ordr de la premiere cle est decroissant
int reverse_compar_occ_mot(const void *c2, const void *c1);

//fonction de hashage pour le module hashtable
size_t str_hashfun(const char *s);

//display_words fonction qui sert a afficher sur la sortie
//standard les mots en colomns suivies de leurs nombre d'occurance avec
//un espace proportionel entre les deux de sort a etre en accord avec
//la fonction display_files qui affiche les noms des fichers avec "" si lecteur
// de stdin
int display_words(void *ref);
void display_files(char *files[]);

// affichage du menu des options sur la sortie standard
void help();

//Dans se programme ptr pointe vers une cellule cword , la fonction free libre
// donc
//cette derniere apres avoir libre le champs mot de cette cellule et revoi 0
int rfree(void *ptr);

//process_arg_opt verifie si ya un argument apres un indice index qui repere
// la position de l'option dont on veut verfier la presence
// d'argument sur la ligne de commande , affecte l'argument a la variable
// pointee par value et incremente l'indice de l'index deja traité,
//puis renvoi true, false autrement
bool process_arg_opt(char **argv, int *index, char **value);

//set_first fonction qui permet d'activer le boolean associer au premier
// parametre
//lui affectant true et false aux reste des deux arguments
void set_first(bool *t_flag, bool *f_flag, bool *f_flag2);

//------------------------------MAIN-------------------------------

int main(int argc, char *argv[]) {
  errno = 0;
  int r = EXIT_FAILURE;
  bool h_flag = true;
  bool no_file = false;
  char *s_value = NULL;
  bool no_sort = true;
  bool l_flag = false;
  bool n_flag = false;
  bool reverse_flag = false;
  bool i_flag = false;
  size_t cut_length = 0;
  bool p_flag = false;
  char *w_value = NULL;
  char *optionArgument = NULL;
  int opt_count = 0;
  bool options_ended = false;

  //--------------------GESTION DES OPTIONS----------------------

  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];
    if (arg[0] != '-' || options_ended) {
      break;
    }
    if (strcmp(arg, SHORT_HELP) == 0 || strcmp(arg, LONG_HELP) == 0) {
      PRINT_HELP;
      return r;
    } else if (strcmp(arg, USAGE) == 0) {
      PRINT_USAGE;
      return r;
    } else if (strcmp(arg, VERSION) == 0) {
      PRINT_VERSION;
      return r;
    } else if (strcmp(arg, HASH_TABLE) == 0) {
      h_flag = true;
      opt_count++;
    } else if (strcmp(arg, BINARY_TREE) == 0) {
      h_flag = false;
      opt_count++;
    } else if (strcmp(arg, LEX_SORT) == 0) {
      set_first(&l_flag, &n_flag, &no_sort);
      opt_count++;
    } else if (strcmp(arg, NUM_SORT) == 0) {
      set_first(&n_flag, &l_flag, &no_sort);
      opt_count++;
    } else if (strcmp(arg, REVERSE_SORT) == 0) {
      reverse_flag = true;
      opt_count++;
    } else if (strcmp(arg, CAP_SORT) == 0) {
      set_first(&no_sort, &n_flag, &l_flag);
      opt_count++;
    } else if (strcmp(arg, PUNCTUATION) == 0) {
      p_flag = true;
      opt_count++;
    } else if (strcmp(arg, CUT) == 0) {
      if (i + 1 < argc && process_arg_opt(argv, &i, &optionArgument)) {
        const char *s = optionArgument;
        char *endptr;
        long t = strtol(s, &endptr, BASE_10);
        if (endptr == s || *endptr != END_CHAR || t < 0) {
          INVALID_ARG(arg);
          TRY;
          return r;
        }
        if (errno == ERANGE) {
          OVERFLOW_ARG(arg);
          TRY;
          return r;
        }
        if (t > MAX_CUT_I) {
          NOT_ENOUGH_MEM;
          TRY;
          return r;
        }
        cut_length = (size_t) t;
        if (cut_length > 0) {
          i_flag = true;
        }
        opt_count += 2;
      } else {
        MISSING_ARG(arg);
        TRY;
        return r;
      }
    } else if (strcmp(arg, SORT) == 0) {
      if (process_arg_opt(argv, &i, &optionArgument)) {
        if (strcmp(optionArgument, LONG_NUM) != 0
            && strcmp(optionArgument, LONG_LEX) != 0
            && strcmp(optionArgument, NONE_SORT) != 0) {
          INVALID_ARG(arg);
          TRY;
          return r;
        }
        s_value = optionArgument;
        if (strcmp(s_value, LONG_NUM) == 0) {
          set_first(&n_flag, &l_flag, &no_sort);
          opt_count += 2;
        }
        if (strcmp(s_value, LONG_LEX) == 0) {
          set_first(&l_flag, &n_flag, &no_sort);
          opt_count += 2;
        }
        if (strcmp(s_value, NONE_SORT) == 0) {
          set_first(&no_sort, &l_flag, &n_flag);
          opt_count += 2;
        }
      } else {
        MISSING_ARG(arg);
        TRY;
        return r;
      }
    } else if (strcmp(arg, WORD_PROCES) == 0) {
      if (process_arg_opt(argv, &i, &optionArgument)) {
        if (strcmp(optionArgument, LONG_BINARY_TREE) != 0
            && strcmp(optionArgument, LONG_HASH_TABLE) != 0) {
          INVALID_ARG(arg);
          TRY;
          return r;
        }
        w_value = optionArgument;
        opt_count += 2;
        if (strcmp(w_value, LONG_HASH_TABLE) == 0) {
          h_flag = true;
        } else if (strcmp(w_value, LONG_BINARY_TREE) == 0) {
          h_flag = false;
        }
      } else {
        MISSING_ARG(arg);
        TRY;
        return r;
      }
    } else if (strcmp(arg, NO_FILE) == 0) {
      no_file = true;
      options_ended = true;
      TRACK;
      break;
    } else {
      options_ended = true;
      UNKNOWN_OPT(arg);
      TRY;
      return r;
    }
  }
  //----------DEBUT DU TRAITEMENT---------------------
  bst *xwbst = bst_empty(compar_mot);
  hashtable *xwht = hashtable_empty((int (*)(const void *, const void *))strcmp,
      (size_t (*)(const void *))str_hashfun);
  holdall *xwha = holdall_empty();
  if (xwbst == NULL || xwha == NULL || xwht == NULL) {
    goto error_capacity;
  }
  argv += opt_count;
  int f = 1;
  int total_files = argc - opt_count - 1;
  if (total_files == 0) {
    no_file = true;
    total_files = 1;
  }
  char *word = malloc(WORD_LENGTH_INITIALE + 1);
  if (word == NULL) {
    goto error_capacity;
  }
  int ch = 0;
  int index_word = 0;
  int taille_mot = WORD_LENGTH_INITIALE;
  while (f <= total_files || (no_file && f <= total_files)) {
    FILE *file;
    if ((argv[f] == NULL) || (no_file && strcmp(argv[f], NO_FILE) == 0)
        || strcmp(argv[f], NO_FILE) == 0) {
      HIGHLIGHT(START_READING_FILE, f);
      file = stdin;
    } else {
      file = fopen(argv[f], "r");
      if (file == NULL) {
        fprintf(stderr, "xwc: Can't open for reading file '%s'.\n", argv[f]);
        goto error;
      }
    }
    int line_number = 1;
    while ((ch = fgetc(file)) != EOF) {
      if (ch == END_LINE) {
        line_number++;
      }
      if (isspace(ch) || (p_flag && ispunct(ch))) {
        if (index_word > 0) {
          word[index_word] = END_CHAR;
          cword temp_cword;
          if (i_flag) {
            if (strlen(word) > cut_length) {
              word[cut_length] = END_CHAR;
              fprintf(stderr,
                  "xwc: Word from file '%s' at line %d cut: '%s...'.\n",
                  argv[f], line_number, word);
            }
          }
          temp_cword.mot = word;
          void *recherche = NULL;
          if (h_flag) {
            recherche = hashtable_search(xwht, word);
          } else {
            recherche = bst_search(xwbst, &temp_cword);
          }
          if (recherche == NULL) {
            cword *final_cword = malloc(sizeof *final_cword);
            if (final_cword == NULL) {
              goto error_capacity;
            }
            final_cword->occurance = 1;
            final_cword->source = f;
            final_cword->mot = malloc(strlen(word) + 1);
            if (final_cword->mot == NULL) {
              goto error_capacity;
            }
            strcpy(final_cword->mot, word);
            void *t = NULL;
            if (h_flag) {
              t = hashtable_add(xwht, final_cword->mot, final_cword);
            } else {
              t = bst_add_endofpath(xwbst, final_cword);
            }
            if (t == NULL) {
              fprintf(stderr, "Error will adding the word: %s\n", word);
              goto error_capacity;
            }
            if (holdall_put(xwha, final_cword) != 0) {
              goto error_capacity;
            }
          } else {
            if (SOURCE(recherche) != f) {
              SOURCE(recherche) = -1;
            } else {
              OCC(recherche) += 1;
            }
          }
          index_word = 0;
          taille_mot = WORD_LENGTH_INITIALE;
        }
      } else {
        word[index_word] = (char) ch;
        index_word += 1;
        if (index_word == taille_mot - 1) {
          taille_mot += WORD_ADDED_LENGTH;
          char *word_ = realloc(word, (size_t) taille_mot);
          if (word_ == NULL) {
            goto error_capacity;
          }
          word = word_;
        }
      }
    }
    if (file != stdin) {
      if (fclose(file) != 0) {
        fprintf(stderr, "Error closing file: %s\n", argv[f]);
        goto error;
      }
    } else {
      //clearerr(stdin); //dans le rapport
      rewind(stdin); //dans le rapport
      HIGHLIGHT(END_READING_FILE, f);
      if (argc == f) {
        break;
      }
    }
    f++;
  }
  free(word);
  display_files(argv);
  if (holdall_count(xwha) == 0) {
    goto error;
  }
  if (!no_sort) {
    setlocale(LC_COLLATE, "");
    if (l_flag) {
      if (reverse_flag) {
        holdall_sort(xwha, reverse_compar_mot);
      } else {
        holdall_sort(xwha, compar_mot);
      }
    }
    if (n_flag) {
      if (reverse_flag) {
        holdall_sort(xwha, reverse_compar_occ_mot);
      } else {
        holdall_sort(xwha, compar_occ_mot);
      }
    }
  }
  holdall_apply(xwha, display_words);
  r = EXIT_SUCCESS;
  goto dispose;
error_capacity:
  NOT_ENOUGH_MEM;
  goto error;
error:
  goto dispose;
dispose:
  bst_dispose(&xwbst);
  hashtable_dispose(&xwht);
  if (xwha != NULL) {
    holdall_apply(xwha, rfree);
  }
  holdall_dispose(&xwha);
  return r;
}

void set_first(bool *t_flag, bool *f_flag, bool *f_flag2) {
  *t_flag = true;
  *f_flag = false;
  *f_flag2 = false;
}

bool process_arg_opt(char **argv, int *index, char **value) {
  if (argv[(*index) + 1] != NULL) {
    *value = argv[*index + 1];
    (*index)++;
    return true;
  }
  return false;
}

int rfree(void *ptr) {
  free(MOT(ptr));
  free(ptr);
  return 0;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 41 * h + *p;
  }
  return h;
}

int compar_mot_hashtable(const void *key1, const void *key2) {
  return strcmp(key1, key2);
}

int compar_mot(const void *c1, const void *c2) {
  return strcoll(MOT(c1), MOT(c2));
}

int reverse_compar_mot(const void *c1, const void *c2) {
  return -compar_mot(c1, c2);
}

int compar_occ_(const void *c1, const void *c2) {
  return (OCC(c1) > OCC(c2)) - (OCC(c1) < OCC(c2));
}

int compar_occ_mot(const void *c1, const void *c2) {
  int res = compar_occ_(c1, c2);
  return res == 0 ? compar_mot(c1, c2) : res;
}

int reverse_compar_occ_mot(const void *c1, const void *c2) {
  int res = -compar_occ_(c1, c2);
  return res == 0 ? compar_mot(c1, c2) : res;
}

//fonction qui affiche n fois la tabulation sur la sortie standard
void print_n_tabs(int n) {
  int i = 0;
  while (i < n) {
    fprintf(stdout, "\t");
    i++;
  }
}

int display_words(void *ref) {
  if (SOURCE(ref) == -1) {
    return 0;
  }
  fprintf(stdout, "%s", MOT(ref));
  print_n_tabs(SOURCE(ref));
  fprintf(stdout, "%d\n", OCC(ref));
  return 0;
}

#define PRINT_FILE_NAME(f)  fprintf(stdout, "\t%s", f)
#define UNNAMED_FILE "\"\""

void display_files(char *files[]) {
  int i = 1;
  if (files[i] == NULL) {
    PRINT_FILE_NAME(UNNAMED_FILE);
  }
  while (files[i] != NULL) {
    if (strcmp(files[i], NO_FILE) == 0) {
      PRINT_FILE_NAME(UNNAMED_FILE);
    } else {
      PRINT_FILE_NAME(files[i]);
    }
    i++;
  }
  fprintf(stdout, "\n");
}

void help() {
  fprintf(stdout,
      "Usage: xwc [OPTION]... [FILE]...\n\n"
      "Exclusive word counting. Print the number of occurrences"
      "of each word that appears in one and only one of given text FILES.\n\n"
      "A word is, by default, a maximum length sequence of characters that do"
      " not belong to the white-space characters set.\n\n"
      "Results are displayed in columns on the standard output. Columns are "
      "separated by the tab character. Lines are terminated by the end-of-line"
      " character. A header line shows the FILE names: the name of the"
      " first FILE appears in the second column, that of the second in"
      " the third, and so on. For the following lines, a word appears in the"
      " first column, its number of occurrences in the FILE in which it "
      "appears to the exclusion of all others in the column associated with "
      "the FILE. No tab characters are written on a line after the number of"
      " occurrences.\n\n Read the standard input when no FILE is given "
      "or for any FILE which is \"-\". In such cases, \"\" is displayed"
      " in the column associated with the FILE on the header line.\n\n"
      "The locale specified by the environment affects sort order. Set "
      "'LC_ALL=C' to get the traditional sort order that uses native"
      " byte values.\n\n Mandatory arguments to long options are mandatory"
      " for short options too.\n\n"
      "Program Information\n"
      "  -?, --help    Print this help message and exit.\n\n"
      "      --usage   Print a short usage message and exit.\n\n"
      "      --version    Print version information.\n\n"
      "Processing\n"
      "  -a            Same as -w avl-binary-tree.\n\n"
      "  -h            Same as -w hash-table.\n\n"
      "  -w TYPE,           Process the words according "
      "to the data structure specified by TYPE. The available values for"
      " TYPE are self explanatory:"
      " 'avl-binary-tree' and 'hash-table'. Default is 'hash-table'.\n\n"
      "Input Control\n"
      "  -i VALUE,           Set the maximal number of significant initial"
      " letters for words to VALUE. 0 means without limitation."
      " Default is 0.\n\n"
      "  -p,           Make the punctuation characters"
      " play the same role as white-space characters in the meaning"
      " of words.\n\n"
      "Output Control\n"
      "  -l            Same as -s lexicographical.\n\n"
      "  -n            Same as -s numeric.\n\n"
      "  -S            Same as -s none.\n\n"
      "  -s TYPE    Sort the results in ascending order, by default,"
      " according to TYPE. The available values for TYPE are:"
      " 'lexicographical', sort on words, 'numeric', sort on number of"
      " occurrences, first key, and words, second key, and"
      " 'none', don't try to sort, take it as it comes. Default is 'none'.\n\n"
      "  -R,    Sort in descending order on the single or first key"
      " instead of ascending order. This option has no effect if the"
      " -S option is enable.\n"
      );
}
