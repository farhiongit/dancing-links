
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

/// Flag to trace execution on standard error terminal.
/// Trace if set, keep quiet otherwise (default).
extern int dlx_trace;

/// Solution displayer signature
/// @param [in] univers Univers
/// @param [in] length Number of subsets in the solution
/// @param [in] solution List of the \p length names of the subsets in the solution.
/// @param [in] data Pointer to user defined and allocated data passed to \p dlx_displayer_set().
typedef void (*dlx_solution_displayer) (Univers univers, unsigned long length, const char *const *solution, void *data);

/// Setter of solution displayer.
/// @param [in] univers Univers
/// @param [in] displayer Solution displayer to set.
/// @param [in] data Pointer to user defined and allocated data passed.
/// @return Solution displayer set by the previous call to dlx_displayer_set() (or \p NULL on first call).
///
/// The function pointed to by \p displayer passed as an argument, if set, is called by dlx_exact_cover_search() every time a solution is found.
dlx_solution_displayer dlx_displayer_set (Univers univers, dlx_solution_displayer displayer, void *data);

/// Initializes a new univers.
/// @param [in] list_of_elements List of elements of the univers, separated by separators.
/// @param [in] separators List of accepted separators, terminated by \0.
///             The separators argument specifies a set of bytes that delimit the tokens in the parsed string.
///             The caller may specify different strings in separators in successive calls that parse the same string.
/// @return univers
/// @post User must call dlx_univers_destroy(Univers univers) later.
Univers dlx_univers_create (const char *list_of_elements, const char *separators) __attribute__ ((overloadable));

/// Initializes a new univers.
/// @param [in] nb_elements Number of elements of the univers.
/// @param [in] elements Names of element of the univers.
/// @return univers
/// @post User must call dlx_univers_destroy(Univers univers) later.
Univers dlx_univers_create (unsigned long nb_elements, const char *elements[]) __attribute__ ((overloadable));

/// Adds a subset to the univers.
/// @param [in] univers Univers
/// @param [in] subset_name Name of the added subset
/// @param [in] list_of_some_elements List of elements of the univers contained in the subset, separated by separators.
/// @param [in] separators List of accepted separators, terminated by \0.
///             The separators argument specifies a set of bytes that delimit the tokens in the parsed string.
///             The caller may specify different strings in separators in successive calls that parse the same string.
/// @return 1 if added sucessfully, 0 otherwise
int dlx_subset_define (Univers univers, const char *subset_name, const char *list_of_some_elements, const char *separators)
  __attribute__ ((overloadable));

/// Adds a subset to the univers.
/// @param [in] univers Univers
/// @param [in] subset_name Name of the added subset
/// @param [in] nb_elements Number of elements of the univers contained in the subset.
/// @param [in] some_elements Names of elements of the univers contained in the subset.
/// @return 1 if added sucessfully, 0 otherwise
int dlx_subset_define (Univers univers, const char *subset_name, unsigned long nb_elements, const char *some_elements[])
  __attribute__ ((overloadable));

/// Requires that a subset be included in any solution.
/// @param [in] univers Univers
/// @param [in] subset_name Name of the required subset
/// @return 1 if sucessful, 0 otherwise
int dlx_subset_require_in_solution (Univers univers, const char *subset_name);

/// Searches for all exact cover solutions.
/// @param [in] univers Univers
/// @param [in] one_only If set, searches for the first solution only.
/// @return Number of solutions found.
///
/// Every time (or only the first time if \p one_only is set) a solution is found,
/// the function \p displayer declared by a previous call to dlx_displayer_set(dlx_solution_displayer displayer) is called with three arguments:
/// - the identifier of the univers \p univers,
/// - the number of subsets included in the solution,
/// - the list of the names of the subsets included in the solution.
///
/// If dlx_displayer_set() was not called or was called with an argument equal to 0, solutions are displayed on standard terminal output.
///
unsigned long dlx_exact_cover_search (Univers univers, int one_only);

/// Releases data used by the univers.
/// @param [in] univers Univers
/// @pre Use dlx_univers_create(const char *elements, const char *separators) or dlx_univers_create(unsigned long nb_elements, const char *elements[]) first.
void dlx_univers_destroy (Univers univers);

#endif
