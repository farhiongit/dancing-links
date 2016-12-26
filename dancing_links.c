
/** @mainpage Exact cover search using dancing links Knuth's algorithm
 *
 * Dancing links, Donald E. Knuth, Submitted on 15 Nov 2000 (see  http://en.wikipedia.org/wiki/Dancing_Links,
 * http://arxiv.org/abs/cs/0011047 and https://arxiv.org/pdf/cs/0011047v1.pdf).
 *
 *  In computer science, Dancing Links, also known as DLX, is the technique suggested by Donald Knuth to
 *  efficiently implement his Algorithm X. Algorithm X is a recursive, nondeterministic, depth-first, backtracking
 *  algorithm that finds all solutions to the exact cover problem.
 *
 *  Some of the better-known exact cover problems include tiling, the n queens problem, and Sudoku.
 *  The name Dancing Links comes from the way the algorithm works, as iterations of the algorithm cause the links
 *  to "dance" with partner links so as to resemble an "exquisitely choreographed dance."
 *
 *  Knuth credits Hiroshi Hitotsumatsu and Kohei Noshita with having invented the idea in 1979,
 *  but it is his paper which has popularized it.
 *
 *  See also http://en.wikipedia.org/wiki/Exact_cover
 *       and http://en.wikipedia.org/wiki/Exact_cover#Sudoku for more.
 */

/**
 * @file
 * Exact cover search implementation.
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

#include "dancing_links.h"

/// If set, the non-determistic choice (by \p dlx_univers_choose_element) of column is optimized heuristically.
#define OPTIMIZE_CHOICE 1

int dlx_trace = 0;

/// Display to terminal standard error if \p dlx_trace is set.
#define DLX_PRINT(...) \
do { if (dlx_trace) fprintf (stderr, __VA_ARGS__); } while (0)

/// Call the callback function \p dlx_displayer if set.
#define DLX_DISPLAY_SOLUTION(univers, length, solution) \
do { if (dlx_displayer) dlx_displayer (univers, length, solution); } while (0)

/// Allowed separators for lists of names of elements.
static const char *ELEMENT_SEPARATOR = ",;:|";

/// Structure of an element (the head, an element of the univers or an element of a subset)
///
/// There are three types of elements:
/// - The head is not a real element but the entry point to the elements of the univers. Its name is "|HEAD|".
/// - Elements of the univers. Each element of the univers is an entry point to the elements in subsets.
/// - Elements into subsets. An element is included in a subset when it is linked to an element of the univers.
///
/// The head and elements of univers are doubly linked as circular lists (\p previousElement and \p nextElement).
/// An element of the univers and subsets containing this element of the univers are doubly linked as circular lists (\p elementInPreviousSubsetContainingThisElementOfUnivers and \p elementInNextSubsetContainingThisElementOfUnivers).
/// The elements of a subset are doubly linked as circular lists (\p previousElement and \p nextElement).
///
/// In OOP,
/// - an abstract base class \p A would contain attributes \p previousElement and \p nextElement (of type \p A),
/// - a class \p H would describe the head of a univers and would heritate from the base class \p A extended with an attribute \p size, the total number of subsets.
/// - an abstract base class \p B would heritate from class \p A extended with attributes
///   \p elementInPreviousSubsetContainingThisElementOfUnivers and \p elementInNextSubsetContainingThisElementOfUnivers (of type B).
/// - a class \p U would describe elements of the univers, heritate from class \p B extended with attributes \p size,
///   the number of subsets containing this element, and \p name, the name of the element
/// - a class \p S would describe elements part of subsets, heritate from class \p B extended with attributes \p elementInUnivers
///   (of type \p U), the element of the univers included in the subset and \p name, the name of the subset containing this element.
///
/// In C, all attributes are gathered into a common structure and unnecessary attributes are left undefined.
struct element
{
  char *name;                   ///< Name of the head ("|HEAD|"), of the element of the univers, or of the subset containing the element of a subset.
  unsigned long int size;       ///< Number of subsets (for head) or of subsets containing an element of the univers. Left undefined for elements of subsets.

  struct element *previousElement;      ///< Link to the previous element in univers or in the subset.
  struct element *nextElement;  ///< Link to the previous element in univers or in the subset.
  struct element *elementInPreviousSubsetContainingThisElementOfUnivers;        ///< Link to the same element in the previous subset. Left undefined for head.
  struct element *elementInNextSubsetContainingThisElementOfUnivers;    ///< Link to the same element in the next subset. Left undefined for head.

  struct element *elementInUnivers;     ///< Link to the element in univers. Left undefined for head and elements in univers.
};

/// The Univers object.
///
/// Links to the first element of the univers (head), the solutions found, as well as subsets required in solutions.
struct univers
{
  struct element *head;         ///< The pointer to the head element that references elements and subsets.

  char **solution;              ///< Buffer to store the current solution (a list of names of subsets.)
  unsigned long solution_length;        ///< Length of the buffer (number of subsets in solution.)

  struct element **uncover_column;      ///< List of subsets required in solutions (an element of this subset.)
  unsigned long uncover_column_length;  ///< Number of subsets required in solutions. 
};

/// Callback function to display a solution
static dlx_solution_displayer dlx_displayer = 0;

/// Gets an element by its name.
/// @param [in] head Univers head
/// @param [in] element_name Name of the element in univers to be fetched
static struct element *
dlx_head_get_element_by_name (struct element *head, const char *element_name)
{
  struct element *element = 0;

  for (element = head->nextElement; element != head && strcmp (element->name, element_name);
       element = element->nextElement)
     /**/;
  if (element != head)
    return element;
  else
    return 0;
}

/// Adds an element in the univers.
/// @param [in] head Univers head
/// @param [in] name Name of the element to be added
static int
dlx_head_add_element (struct element *head, const char *name)
{
  if (dlx_head_get_element_by_name (head, name))
    return 0;

  /// Initializes the element in the univers.
  struct element *element = malloc (sizeof (*element));

  element->name = strdup (name);

  element->size = 0;

  element->elementInPreviousSubsetContainingThisElementOfUnivers =
    element->elementInNextSubsetContainingThisElementOfUnivers = element;

  /// The head and elements of univers are doubly linked as circular lists (\p previousElement and \p nextElement).
  element->nextElement = head;
  element->previousElement = head->previousElement;

  // Other unused components of element are left undefined.

  head->previousElement->nextElement = element;
  head->previousElement = element;

  return 1;
}

/// Chooses an element in the univers.
/// @param [in] head Univers head
/// @return Chosen element in univers
/// @note Makes use of flag #OPTIMIZE_CHOICE to select the element with the minimal number of subsets that contain it (if set) or the first element, in order of the elements declared in the univers (if not).
static struct element *
dlx_head_choose_element (struct element *head)
{
  struct element *element = head->nextElement;  // Default choice in case j->Size would not be equal to ULONG_MAX for any element.

#if OPTIMIZE_CHOICE
  unsigned long s = ULONG_MAX;

  for (struct element * j = head->nextElement; j != head; j = j->nextElement)
    if (j->size < s)
    {
      element = j;
      s = j->size;
    }
#else
#warning The non-determistic choice of column is not optimized heuristically.
#endif

  return element;
}

/// Removes an element and all the elements of subsets which contain this element.
/// @param [in] elementInUnivers Element to be removed.
/// @post User must call dlx_element_uncover(struct element *elementInUnivers) later.
///
/// We remove the element from the univers.
/// The elements in subsets that contains this element are also removed from the univers.
static void
dlx_element_cover (struct element *elementInUnivers)
{
  elementInUnivers->nextElement->previousElement = elementInUnivers->previousElement;
  elementInUnivers->previousElement->nextElement = elementInUnivers->nextElement;

  for (struct element * i = elementInUnivers->elementInNextSubsetContainingThisElementOfUnivers; i != elementInUnivers; i = i->elementInNextSubsetContainingThisElementOfUnivers)       // all subsets containing the element.
    for (struct element * j = i->nextElement; j != i; j = j->nextElement)       // all other elements in the subset
    {
      j->elementInNextSubsetContainingThisElementOfUnivers->elementInPreviousSubsetContainingThisElementOfUnivers =
        j->elementInPreviousSubsetContainingThisElementOfUnivers;
      j->elementInPreviousSubsetContainingThisElementOfUnivers->elementInNextSubsetContainingThisElementOfUnivers =
        j->elementInNextSubsetContainingThisElementOfUnivers;
      j->elementInUnivers->size--;      // The number of subsets containing this element is decremented.
    }
}

/// Restores an element and all the elements of subsets which contain this element.
/// @param [in] elementInUnivers Element to be restored.
/// @pre Use dlx_element_cover(struct element *elementInUnivers) first.
static void
dlx_element_uncover (struct element *elementInUnivers)
{
  for (struct element * i = elementInUnivers->elementInPreviousSubsetContainingThisElementOfUnivers;
       i != elementInUnivers; i = i->elementInPreviousSubsetContainingThisElementOfUnivers)
    for (struct element * j = i->previousElement; j != i; j = j->previousElement)
    {
      j->elementInUnivers->size++;      // The number of subsets containing this element is incremented.
      j->elementInPreviousSubsetContainingThisElementOfUnivers->elementInNextSubsetContainingThisElementOfUnivers = j;
      j->elementInNextSubsetContainingThisElementOfUnivers->elementInPreviousSubsetContainingThisElementOfUnivers = j;
    }

  elementInUnivers->previousElement->nextElement = elementInUnivers;
  elementInUnivers->nextElement->previousElement = elementInUnivers;
}

/// Displays a solution.
/// @param [in] univers Univers
/// @param [in] solutions Solutions to be dispplayed. Each solution is a list of elements.
static void
dlx_univers_display_solutions (Univers univers, struct element **solutions)
{
  unsigned long length = univers->solution_length - univers->head->size;

  for (unsigned long i = univers->solution_length - univers->head->size; i < univers->solution_length; i++)
  {
    free (univers->solution[i]);
    univers->solution[i] = 0;
  }

  DLX_PRINT ("Exact cover solution:\n");
  if (!univers->head->size || !solutions || !solutions[0])
  {
    DLX_PRINT ("  Already exactly covered. No more subsets required.\n");
  }
  else
  {
    for (unsigned long k = 0; k < univers->head->size && solutions[k]; k++)
    {
      DLX_PRINT ("  [%lu]\tSubset %s:", univers->solution_length - univers->head->size + k + 1, *solutions[k]->name ? solutions[k]->name : "(unnamed)");        // line name
      struct element *elementInSubset = solutions[k];

      do
      {
        DLX_PRINT (" %s", elementInSubset->elementInUnivers->name);     // name pf element
        elementInSubset = elementInSubset->nextElement;
      }
      while (elementInSubset != solutions[k]);

      DLX_PRINT ("\n");

      univers->solution[univers->solution_length - univers->head->size + k] = strdup (solutions[k]->name);
      length++;
    }
  }

  // Why can't it be passed a char ** to a function which expects a const char ** (Warning : discards qualifiers in nested pointer types) ?
  // The reason that you cannot assign a char ** value to a const char ** pointer in C is somewhat obscure.
  // It is allowed in C++, but not in C. References: ISO Sec. 6.1.2.6, Sec. 6.3.16.1, Sec. 6.5.3.
  DLX_DISPLAY_SOLUTION (univers, length, (const char *const *) univers->solution);

  for (unsigned long i = univers->solution_length - univers->head->size; i < univers->solution_length; i++)
  {
    free (univers->solution[i]);
    univers->solution[i] = 0;
  }
}

/// Recursive function to search for solutions.
/// @param [in] univers Univers
/// @param [in] solutions Solution to be dispplayed.
/// @param [in] k Depth of search
/// @param [in] one_only If set, searches for the first solution only.
/// @return Number of solutions found.
static unsigned long
dlx_univers_search (Univers univers, struct element **solutions, unsigned long k, int one_only)
{
  // If there is no more element in the univers, this means all elements have been covered successfully.
  // We have found a solution that we can display.
  if (univers->head->nextElement == univers->head)
  {
    dlx_univers_display_solutions (univers, solutions);
    return 1;
  }

  // Otherwise, we search for an exact cover search: a group of subsets such that the union of them
  // contains all the elements of the univers and any intersection between two of them is empty.

  int solution_found = 0;

  // We peek an element deterministically (all elements will have been peeked sucessively at last.)
  // We keep a reference to the uncovered element for further access.
  struct element *c = dlx_head_choose_element (univers->head);

  // We can remove this element from the univers, since we will retain one of the subsets containig it in the solution
  // and we won't have to consider this element anymore.
  dlx_element_cover (c);

  // One and only one of the subsets containing this element will have to be included in the solution.
  // One after the other, we try to keep each subset containing the element in the solution,
  // one after the other, nondeterministically.
  for (struct element * r = c->elementInNextSubsetContainingThisElementOfUnivers; r != c;
       r = r->elementInNextSubsetContainingThisElementOfUnivers)
  {
    // We consider, as a trial-and-error approach, that the subset containing the element is part of the solution.
    solutions[k] = r;

    // This subset containing the element might also contain other elements which are
    // de facto included in the solution.
    for (struct element * j = r->nextElement; j != r; j = j->nextElement)
    {
      // We won't have to search for a valid element in these columns containing j.
      // We can therefore remove these columns from the matrix.

      // Furthermore, the solution can not contain other subsets that
      // contain the same elements, otherwise,
      // there would be more than one subset containig the same element in the solution.
      // Thus, elements in those other subsets can be removed from the univers.
      dlx_element_cover (j->elementInUnivers);
    }

    /// Calls \p dlx_univers_search recursively (backtracking), incrementing \p k.
    solution_found += dlx_univers_search (univers, solutions, k + 1, one_only);

    solutions[k] = 0;

    for (struct element * j = r->previousElement; j != r; j = j->previousElement)
      dlx_element_uncover (j->elementInUnivers);

    if (solution_found && one_only)
      break;
  }

  dlx_element_uncover (c);

  // The univers is fully restored (all elements uncovered).

  return solution_found;
}

dlx_solution_displayer
dlx_displayer_set (dlx_solution_displayer msd)
{
  dlx_solution_displayer old = dlx_displayer;

  dlx_displayer = msd;
  return old;
}

Univers dlx_univers_create (unsigned long nb_elements, const char *elements[]) __attribute__ ((overloadable))
{
  if (!nb_elements || !elements)
    return 0;

  Univers univers = malloc (sizeof (*univers));

  univers->head = malloc (sizeof (*univers->head));

  univers->head->previousElement = univers->head->nextElement = univers->head;
  univers->head->size = 0;
  univers->head->name = "|HEAD|";
  // Other unused components of head are left undefined.

  univers->solution = 0;
  univers->solution_length = 0;
  univers->uncover_column = 0;
  univers->uncover_column_length = 0;

  DLX_PRINT ("Elements in univers:");
  int redo = 0;

  for (unsigned long i = 0; i < nb_elements; i++)
  {
    if (!elements[i] || !*elements[i])
      continue;

    DLX_PRINT (" %s", elements[i]);
    if (!dlx_head_add_element (univers->head, elements[i]))
    {
      DLX_PRINT (" (already exists ==> not added)");
      redo = 1;
    }
  }

  if (redo)
  {
    DLX_PRINT (" =");
    for (struct element * element = univers->head->nextElement; element != univers->head;
         element = element->nextElement)
      DLX_PRINT (" %s", element->name);
  }

  DLX_PRINT ("\n");

  return univers;
}

Univers dlx_univers_create (const char *elements) __attribute__ ((overloadable))
{
  if (!elements)
    return 0;

  char *sccpy;
  char *saveptr;
  unsigned long nb_cols;

  sccpy = strdup (elements);
  nb_cols = 0;
  for (char *c = sccpy; strtok_r (c, ELEMENT_SEPARATOR, &saveptr); c = 0)
    nb_cols++;
  free (sccpy);

  if (!nb_cols)
    return 0;

  const char *cols[nb_cols];
  char *colname;

  sccpy = strdup (elements);
  nb_cols = 0;
  for (char *c = sccpy; (colname = strtok_r (c, ELEMENT_SEPARATOR, &saveptr)); c = 0)
    cols[nb_cols++] = colname;

  /// @overload
  Univers ret = dlx_univers_create (nb_cols, cols);

  free (sccpy);

  return ret;
}

int
dlx_subset_define (Univers univers, const char *subset_name, unsigned long nb_elements, const char *elements[])
__attribute__ ((overloadable))
{
  if (!univers || !subset_name || !nb_elements || !elements)
    return 0;

  DLX_PRINT ("Elements in subset %s:", *subset_name ? subset_name : "(unnamed)");
  int redo = 0;

  struct element *first_element = 0;

  for (unsigned long i = 0; i < nb_elements; i++)
  {
    if (!elements[i] || !*elements[i])
      continue;

    DLX_PRINT (" %s", elements[i]);
    struct element *elementInUnivers = 0;

    if (!(elementInUnivers = dlx_head_get_element_by_name (univers->head, elements[i])))
    {
      DLX_PRINT (" (unknown element)");
      redo = 1;
      continue;
    }

    struct element *elementInSubset = 0;

    if (first_element)
    {
      int already_included = 0;

      elementInSubset = first_element;

      do
      {
        if (!strcmp (elementInSubset->elementInUnivers->name, elements[i]))
          already_included = 1; // element already included in subset
        elementInSubset = elementInSubset->nextElement;
      }
      while (elementInSubset != first_element && !already_included);

      if (already_included)
      {
        DLX_PRINT (" (element already included in subset ==> ignored)");
        redo = 1;
        continue;
      }
    }

    // Add element in subset
    elementInSubset = malloc (sizeof (*elementInSubset));

    elementInSubset->name = strdup (subset_name);
    elementInSubset->elementInUnivers = elementInUnivers;
    elementInUnivers->size++;   // Number of subsets containing the element is incremented

    /// The element of the univers and subsets containing this element of the univers are doubly linked as circular lists (\p elementInPreviousSubsetContainingThisElementOfUnivers and \p elementInNextSubsetContainingThisElementOfUnivers).
    elementInSubset->elementInNextSubsetContainingThisElementOfUnivers = elementInUnivers;
    elementInSubset->elementInPreviousSubsetContainingThisElementOfUnivers =
      elementInUnivers->elementInPreviousSubsetContainingThisElementOfUnivers;
    elementInUnivers->elementInPreviousSubsetContainingThisElementOfUnivers->
      elementInNextSubsetContainingThisElementOfUnivers = elementInSubset;
    elementInUnivers->elementInPreviousSubsetContainingThisElementOfUnivers = elementInSubset;

    if (first_element == 0)
    {
      elementInSubset->nextElement = elementInSubset->previousElement = elementInSubset;
      first_element = elementInSubset;
    }
    else
    {
      /// The elements of a subset are doubly linked as circular lists (\p previousElement and \p nextElement).
      elementInSubset->nextElement = first_element;
      elementInSubset->previousElement = first_element->previousElement;
      first_element->previousElement->nextElement = elementInSubset;
      first_element->previousElement = elementInSubset;
    }
  }

  if (first_element)            // At least one element was added to the subset
  {
    univers->head->size++;      // Number of subsets

    univers->solution_length++;
    univers->solution = realloc (univers->solution, univers->solution_length * sizeof (*univers->solution));
    univers->solution[univers->solution_length - 1] = 0;

    if (redo)
    {
      DLX_PRINT (" =");
      struct element *elementInSubset = first_element;

      do
      {
        DLX_PRINT (" %s", elementInSubset->elementInUnivers->name);
        elementInSubset = elementInSubset->nextElement;
      }
      while (elementInSubset != first_element);
    }

    DLX_PRINT ("\n");
    return 1;
  }
  else
  {
    DLX_PRINT (" (empty subset)\n");
    return 0;
  }
}

int dlx_subset_define (Univers univers, const char *subset_name, const char *some_elements)
  __attribute__ ((overloadable))
{
  if (!some_elements)
    return 0;

  char *sccpy;
  char *saveptr;
  unsigned long nb_cols;

  sccpy = strdup (some_elements);
  nb_cols = 0;
  for (char *c = sccpy; strtok_r (c, ELEMENT_SEPARATOR, &saveptr); c = 0)
    nb_cols++;
  free (sccpy);

  if (!nb_cols)
    return 0;

  const char *cols[nb_cols];
  char *colname;

  sccpy = strdup (some_elements);
  nb_cols = 0;
  for (char *c = sccpy; (colname = strtok_r (c, ELEMENT_SEPARATOR, &saveptr)); c = 0)
    cols[nb_cols++] = colname;

  /// @overload
  int ret = dlx_subset_define (univers, subset_name, nb_cols, cols);

  free (sccpy);

  return ret;
}

int
dlx_subset_require_in_solution (Univers univers, const char *subset_name)
{
  if (!univers || !subset_name)
    return 0;

  DLX_PRINT ("Subset required in any solution:\n");
  DLX_PRINT ("  [%lu]\tSubset %s:", univers->solution_length - univers->head->size + 1, subset_name);

  // In case of several candidate subsets (with the same name), a subset is chosen arbitrarily
  // (the subset with an element in the first element, in order of addded element to the univers,
  // then the first subset, in order of added subsets to the univers)
  for (struct element * elementInUnivers = univers->head->nextElement; elementInUnivers != univers->head;
       elementInUnivers = elementInUnivers->nextElement)
    // Look for the first element of the subset 'subset_name' (in order of creation in univers).
    for (struct element * elementInSubset = elementInUnivers->elementInNextSubsetContainingThisElementOfUnivers;
         elementInSubset != elementInUnivers;
         elementInSubset = elementInSubset->elementInNextSubsetContainingThisElementOfUnivers)
      if (!strcmp (elementInSubset->name, subset_name))
      {
        // The selected subset conforms to theses conditions:
        // - subset name is 'subset_name'
        // - subset was not previously required in the solution
        struct element *j = elementInSubset;

        // Removes the elements contained in the required subset
        // and all the subsets which contain these elements, the required subset included.
        do
        {
          DLX_PRINT (" %s", j->elementInUnivers->name); // Name of the element.

          // This subset containing element might also contain elements which are
          // de facto included in the solution.
          // We can therefore remove these elements from the univers.

          // Furthermore, the solution can not contain subsets that
          // contain this element, otherwise,
          // there would be more than one subset containing this element in the solution.
          // Thus, those elements can be removed from the univers.

          dlx_element_cover (j->elementInUnivers);

          // Keep a reference to the uncovered element for further access in dlx_univers_destroy.
          univers->uncover_column_length++;
          univers->uncover_column =
            realloc (univers->uncover_column, univers->uncover_column_length * sizeof (*univers->uncover_column));
          univers->uncover_column[univers->uncover_column_length - 1] = j->elementInUnivers;

          j = j->nextElement;
        }
        while (j != elementInSubset);

        DLX_PRINT ("\n");

        univers->solution[univers->solution_length - univers->head->size] = strdup (subset_name);
        univers->head->size--;

        // break loops and return.
        return 1;
      }                         // if (!strcmp (element->Name, subset_name))

  DLX_PRINT (" (unknown or incompatible subset ==> not required in solutions)\n");

  // The required subset can not be part of the solution : either it's unknown by name, or it is not
  // compatible with other subsets already included in a required solution.
  return 0;
}

unsigned long
dlx_exact_cover_search (Univers univers, int one_only)
{
  if (!univers)
    return 0;

  DLX_PRINT ("Searching for %s exact cover solution%s.\n", one_only ? "the first" : "all", one_only ? "" : "s");

  unsigned long nb_solutions = 0;

  if (univers->head->size)
  {
    struct element *solutions[univers->head->size];

    for (unsigned long i = 0; i <= univers->head->size; i++)
      solutions[i] = 0;

    nb_solutions = dlx_univers_search (univers, solutions, 0, one_only);
  }
  else
    nb_solutions = dlx_univers_search (univers, 0, 0, one_only);

  if (!nb_solutions)
    DLX_DISPLAY_SOLUTION (univers, 0, 0);

  DLX_PRINT ("%lu solution%s found.\n\n", nb_solutions, nb_solutions == 1 ? "" : "s");

  return nb_solutions;
}

void
dlx_univers_destroy (Univers univers)
{
  if (!univers)
    return;

  for (unsigned long i = univers->uncover_column_length; i > 0; i--)
    dlx_element_uncover (univers->uncover_column[i - 1]);
  free (univers->uncover_column);

  struct element *nc = 0;

  for (struct element * elementInUnivers = univers->head->nextElement; elementInUnivers != univers->head;
       elementInUnivers = nc)
  {
    struct element *ne = 0;

    for (struct element * elementInSubset = elementInUnivers->elementInNextSubsetContainingThisElementOfUnivers;
         elementInSubset != elementInUnivers; elementInSubset = ne)
    {
      free (elementInSubset->name);
      ne = elementInSubset->elementInNextSubsetContainingThisElementOfUnivers;
      free (elementInSubset);
    }

    free (elementInUnivers->name);
    nc = elementInUnivers->nextElement;
    free (elementInUnivers);
  }
  free (univers->head);

  for (unsigned long i = 0; i < univers->solution_length; i++)
    free (univers->solution[i]);
  free (univers->solution);

  free (univers);
}
