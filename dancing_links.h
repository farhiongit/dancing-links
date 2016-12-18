
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

/// Setter of solution displayer.
/// @param [in] msd Solution displayer to set.
/// @return Previous solution displayer.
///
/// This function \p msd is called each time a solution is found.
/// If \p msd is set to 0, solutions are displayed on standard terminal output.
dlx_solution_displayer dlx_displayer_set (dlx_solution_displayer msd);

/// Initializes a new univers.
/// @param [in] list_of_elements List of elements of the univers, separated by separators.
/// @return univers
/// @post User must call dlx_univers_destroy(Univers univers) later.
/// @note Valid separators are defined by #ELEMENT_SEPARATOR.
Univers dlx_univers_create (const char *list_of_elements) __attribute__ ((overloadable));

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
/// @return 1 if added sucessfully, 0 otherwise
/// @note Valid separators are defined by #ELEMENT_SEPARATOR.
int dlx_subset_define (Univers univers, const char *subset_name, const char *list_of_some_elements)
  __attribute__ ((overloadable));

/// Adds a subset to the univers.
/// @param [in] univers Univers
/// @param [in] subset_name Name of the added subset
/// @param [in] nb_elements Number of elements of the univers contained in the subset.
/// @param [in] some_elements Names of elements of the univers contained in the subset.
/// @return 1 if added sucessfully, 0 otherwise
int dlx_subset_define (Univers univers, const char *subset_name, unsigned long nb_elements, const char *some_elements[])
  __attribute__ ((overloadable));

/// Requires that the subset which name is 'subset_name' be included in any solution.
/// @param [in] univers Univers
/// @param [in] subset_name Name of the required subset
/// @return 1 if sucessful, 0 otherwise
///
/// The selected subset conforms to theses conditions:
/// - subset name is 'subset_name'
/// - subset was not previously required in the solution
///
/// In case of several candidate subsets (with the same name), a subset is chosen arbitrarily
/// (the subset with an element in the first element, in order of addded element to the univers,
/// then the first subset, in order of added subsets to the univers)
///
/// Removes the elements contained in the required subset
/// and all the subsets which contain these elements, the required subset included.
///
int dlx_subset_require_in_solution (Univers univers, const char *subset_name);

/// Searches for all exact cover solutions.
/// @param [in] univers Univers
/// @param [in] one_only If set, searches for the first solution only.
/// @return Number of solutions found.
unsigned long dlx_exact_cover_search (Univers univers, int one_only);

/// Releases data used by the univers.
/// @param [in] univers Univers
/// @pre Use dlx_univers_create(const char *elements) or ::dlx_univers_create(unsigned long nb_elements, const char *elements[]) first.
void dlx_univers_destroy (Univers univers);

#endif
