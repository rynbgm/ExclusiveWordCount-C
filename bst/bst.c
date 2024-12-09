//  bst.c : partie implantation d'un module polymorphe pour la spécification
//   ABINR du TDA ABinR(T).

#include "bst.h"

//=== Type cbst ================================================================

//--- Définition cbst ----------------------------------------------------------

typedef struct cbst cbst;

struct cbst {
  cbst *next[2];
  const void *ref;
  int height;
};

//--- Raccourcis cbst ----------------------------------------------------------

#define EMPTY()     NULL
#define IS_EMPTY(p) ((p) == NULL)
#define LEFT(p)     ((p)->next[0])
#define RIGHT(p)    ((p)->next[1])
#define REF(p)      ((p)->ref)
#define HEIGHT(p) ((p)->height)
#define NEXT(p, d)  ((p)->next[(d) > 0])

//--- Divers -------------------------------------------------------------------

static size_t add__size_t(size_t x1, size_t x2) {
  return x1 + x2;
}

static int max__int(int x1, int x2) {
  return x1 > x2 ? x1 : x2;
}

static size_t min__size_t(size_t x1, size_t x2) {
  return x1 < x2 ? x1 : x2;
}

//--- Fonctions cbst -----------------------------------------------------------

//  DEFUN_CBST__MEASURE : définit la fonction récursive de nom « cbst__ ## fun »
//    et de paramètre un pointeur d'arbre binaire, qui renvoie zéro si l'arbre
//    est vide et « 1 + oper(r0, r1) » sinon, où r0 et r1 sont les valeurs
//    renvoyées par les appels récursifs de la fonction avec les pointeurs des
//    sous-arbres gauche et droit de l'arbre comme paramètres.
#define DEFUN_CBST__MEASURE(fun, oper)                                         \
  static size_t cbst__ ## fun(const cbst * p) {                                \
    return IS_EMPTY(p)                                                         \
      ? 0                                                                      \
      : 1 + oper(cbst__ ## fun(LEFT(p)), cbst__ ## fun(RIGHT(p)));             \
  }

//  cbst__size, cbst__height, cbst__distance : définition.

DEFUN_CBST__MEASURE(size, add__size_t)
DEFUN_CBST__MEASURE(distance, min__size_t)

static int cbst__height(const cbst *p) {
  return IS_EMPTY(p)
    ? 0
    : HEIGHT(p);
}

static void cbst__update_height(cbst *p) {
  HEIGHT(p)
    = 1 + max__int(cbst__height(LEFT(p)), cbst__height(RIGHT(p)));
}

static int cbst__balance(const cbst *p) {
  return IS_EMPTY(p)
    ? 0
    : cbst__height(LEFT(p)) - cbst__height(RIGHT(p));
}

static void cbst__rotation_left(cbst **pp) {
  cbst *p = RIGHT(*pp);
  RIGHT(*pp) = LEFT(p);
  LEFT(p) = *pp;
  cbst__update_height(*pp);
  cbst__update_height(p);
  *pp = p;
}

static void cbst__rotation_right(cbst **pp) {
  cbst *p = LEFT(*pp);
  LEFT(*pp) = RIGHT(p);
  RIGHT(p) = *pp;
  cbst__update_height(*pp);
  cbst__update_height(p);
  *pp = p;
}

static void cbst__rotation_left_right(cbst **pp) {
  cbst__rotation_left(&LEFT(*pp));
  cbst__rotation_right(pp);
}

static void cbst__rotation_right_left(cbst **pp) {
  cbst__rotation_right(&RIGHT(*pp));
  cbst__rotation_left(pp);
}

static int cbst__balancing(cbst **pp) {
  cbst__update_height(*pp);
  int b = cbst__balance(*pp);
  if (b <= 1 && b >= -1) {
    return 0;
  }
  if (b >= 2) {
    if (cbst__balance(LEFT(*pp)) >= 0) {
      cbst__rotation_right(pp);
    } else {
      cbst__rotation_left_right(pp);
    }
  } else if (b <= -2) {
    if (cbst__balance(RIGHT(*pp)) <= 0) {
      cbst__rotation_left(pp);
    } else {
      cbst__rotation_right_left(pp);
    }
  }
  return 1;
}

//  cbst__dispose : libère les ressources allouées à l'arbre binaire pointé par
//    p.
static void cbst__dispose(cbst *p) {
  if (!IS_EMPTY(p)) {
    cbst__dispose(LEFT(p));
    cbst__dispose(RIGHT(p));
    free(p);
  }
}

//  cbst_add_endofpath :Recherche dans
//    l'arbre binaire de recherche associé à t la référence d'un objet égal à
//    celui de référence ref au sens de la fonction de comparaison. Si la
//    recherche est positive, renvoie la référence trouvée. Tente sinon
//    d'ajouter la référence selon la méthode de l'ajout en bout de chemin ;
//    renvoie NULL en cas de dépassement de capacité ; renvoie sinon ref.
static void *cbst__add_endofpath(cbst **pp, const void *ref,
    int (*compar)(const void *, const void *)) {
  if (IS_EMPTY(*pp)) {
    *pp = malloc(sizeof **pp);
    if (*pp == NULL) {
      return NULL;
    }
    HEIGHT(*pp) = 1;
    LEFT(*pp) = EMPTY();
    RIGHT(*pp) = EMPTY();
    REF(*pp) = ref;
    return (void *) ref;
  }
  int d;
  if ((d = compar(ref, REF(*pp))) == 0) {
    return (void *) REF(*pp);
  }
  void *r = cbst__add_endofpath(&NEXT(*pp, d), ref, compar);
  cbst__balancing(pp);
  return r;
}

static void *cbst__remove_max(cbst **pp) {
  if (IS_EMPTY(*pp)) {
    return NULL;
  }
  if (IS_EMPTY(RIGHT(*pp))) {
    cbst *t = *pp;
    void *ref = (void *) REF(t);
    *pp = LEFT(t);
    free(t);
    return ref;
  }
  void *r = cbst__remove_max(&(RIGHT(*pp)));
  cbst__balancing(pp);
  return r;
}

static void cbst__remove_root(cbst **pp) {
  if (IS_EMPTY(LEFT(*pp))) {
    cbst *t = *pp;
    *pp = RIGHT(t);
    free(t);
  } else {
    REF(*pp) = cbst__remove_max(&(LEFT(*pp)));
  }
}

//  bst_remove_climbup_left : renvoie NULL si ref vaut NULL. Recherche sinon
//    dans l'arbre binaire de recherche associé à t la référence d'un objet égal
//    à celui de référence ref au sens de la fonction de comparaison. Si la
//    recherche est négative, renvoie NULL. Retire sinon la référence trouvée
//    selon la méthode du retrait par remontée gauche et renvoie la référence
//    trouvée.

static void *cbst__remove_climbup_left(cbst **pp, const void *ref,
    int (*compar)(const void *, const void *)) {
  if (IS_EMPTY(*pp)) {
    return NULL;
  }
  int d = compar(ref, REF(*pp));
  if (d == 0) {
    void *r = (void *) REF(*pp);
    cbst__remove_root(pp);
    return r;
  }
  void *r = cbst__remove_climbup_left(&(NEXT(*pp, d)), ref, compar);
  cbst__update_height(*pp);
  return r;
}

static void *cbst__search(const cbst *p, const void *ref,
    int (*compar)(const void *, const void *)) {
  if (IS_EMPTY(p)) {
    return EMPTY();
  }
  int d = compar(ref, REF(p));
  if (d == 0) {
    return (void *) REF(p);
  }
  return cbst__search(NEXT(p, d), ref, compar);
}

static size_t cbst__number(const cbst *p, const void *ref,
    int (*compar)(const void *, const void *), size_t number) {
  if (IS_EMPTY(p)) {
    return number;
  }
  int d = compar(REF(p), ref);
  if (d == 0) {
    return number;
  }
  if (d > 0) {
    return cbst__number(LEFT(p), ref, compar, number * 2);
  }
  return cbst__number(RIGHT(p), ref, compar, number * 2 + 1);
}

//   cbst_rank :Calcule
//    le rang du nœud de l'arbre binaire de recherche associé à t
//    dont la valeur est égale à celle de l'objet pointé par ref au sens de la
//    fonction de comparaison.ce rang si une telle valeur
//    existe. Renvoie sinon le rang qu'aurait le nœud si la référence
//    ref était ajoutée à l'arbre.
static size_t cbst__rank(const cbst *p, const void *ref,
    int (*compar)(const void *, const void *), size_t rank) {
  if (IS_EMPTY(p)) {
    return rank;
  }
  size_t t = cbst__size(LEFT(p));
  int c = compar(REF(p), ref);
  if (c == 0) {
    return t + rank;
  }
  if (c < 0) {
    return cbst__rank(RIGHT(p), ref, compar, t + 1 + rank);
  }
  return cbst__rank(LEFT(p), ref, compar, rank);
}

#define REPR__TAB 4
#define REPR_GRAPH_VIDE "|"

static void cbst__repr_graphic(const cbst *p, void (*put)(
    const void *ref), size_t level) {
  if (IS_EMPTY(p)) {
    printf("%*s" REPR_GRAPH_VIDE "\n", (int) (REPR__TAB * level), " ");
  } else {
    cbst__repr_graphic(RIGHT(p), put, level + 1);
    printf("%*s", (int) level * REPR__TAB, "");
    put(REF(p));
    // printf(" h = %d  b = %d ",HEIGHT(p),cbst__balance(p));
    printf("\n");
    cbst__repr_graphic(LEFT(p), put, level + 1);
  }
}

//=== Type bst =================================================================

//--- Définition bst -----------------------------------------------------------

struct bst {
  int (*compar)(const void *, const void *);
  cbst *root;
};

//--- Fonctions bst ------------------------------------------------------------

bst *bst_empty(int (*compar)(const void *, const void *)) {
  bst *t = malloc(sizeof *t);
  if (t == NULL) {
    return NULL;
  }
  t->compar = compar;
  t->root = EMPTY();
  return t;
}

void bst_dispose(bst **tptr) {
  if (*tptr == NULL) {
    return;
  }
  cbst__dispose((*tptr)->root);
  free(*tptr);
  *tptr = NULL;
}

//  bst_add_endofpath : renvoie NULL si ref vaut NULL. Recherche sinon dans
//    l'arbre binaire de recherche associé à t la référence d'un objet égal à
//    celui de référence ref au sens de la fonction de comparaison. Si la
//    recherche est positive, renvoie la référence trouvée. Tente sinon
//    d'ajouter la référence selon la méthode de l'ajout en bout de chemin ;
//    renvoie NULL en cas de dépassement de capacité ; renvoie sinon ref.

void *bst_add_endofpath(bst *t, const void *ref) {
  if (ref == NULL) {
    return NULL;
  }
  return cbst__add_endofpath(&(t->root), ref, t->compar);
}

void *bst_remove_climbup_left(bst *t, const void *ref) {
  if (ref == NULL) {
    return NULL;
  }
  return cbst__remove_climbup_left(&(t->root), ref, t->compar);
}

void *bst_search(bst *t, const void *ref) {
  if (ref == NULL) {
    return NULL;
  }
  return cbst__search(t->root, ref, t->compar);
}

size_t bst_size(bst *t) {
  return cbst__size(t->root);
}

size_t bst_height(bst *t) {
  return (size_t) cbst__height(t->root);
}

size_t bst_distance(bst *t) {
  return cbst__distance(t->root);
}

size_t bst_number(bst *t, const void *ref) {
  if (ref == NULL) {
    return (size_t) -1;
  }
  return cbst__number(t->root, ref, t->compar, 1);
}

size_t bst_rank(bst *t, const void *ref) {
  if (ref == NULL) {
    return (size_t) -1;
  }
  return cbst__rank(t->root, ref, t->compar, 0);
}

void bst_repr_graphic(bst *t, void (*put)(const void *ref)) {
  cbst__repr_graphic(t->root, put, 0);
}
