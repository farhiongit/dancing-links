
/**
 * @file
 * Exact cover search interface.
 * The aim is to find a list of subsets of the univers
 * which all together, cover the whole univers, each subset being disjoint from the others.
 * It's an exact cover of the univers.
 */

#ifndef __DANCING_LINKS__
#define __DANCING_LINKS__
/// Objet Univers
typedef struct univers *Univers;

/// Solution displayer signature
typedef void (*dlx_solution_displayer) (Univers univers, unsigned long length, const char *const *solution);

dlx_solution_displayer dlx_displayer_set (dlx_solution_displayer msd);

Univers dlx_univers_create (const char *list_of_elements) __attribute__ ((overloadable));
Univers dlx_univers_create (unsigned long nb_elements, const char *elements[]) __attribute__ ((overloadable));
int dlx_subset_define (Univers univers, const char *subset_name, const char *list_of_some_elements)
  __attribute__ ((overloadable));
int dlx_subset_define (Univers univers, const char *subset_name, unsigned long nb_elements, const char *some_elements[])
  __attribute__ ((overloadable));

int dlx_subset_require_in_solution (Univers univers, const char *subset_name);
unsigned long dlx_exact_cover_search (Univers univers, int one_only);

void dlx_univers_destroy (Univers univers);

#endif
