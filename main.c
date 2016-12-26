#include <stdio.h>
#include <string.h>
#include "dancing_links.h"

static void
my_dlx_solution_displayer (Univers univers, unsigned long length, const char *const *solution)
{
  printf ("\n---\nUnivers %p\nSolution: %lu elements\n", (void *) univers, length);
  for (unsigned long i = 0; i < length; i++)
    printf ("'%s' ; ", solution[i]);
  printf ("\n---\n");
}

static void
test_sudoku (void)
{
  //Test 1
  // Sudoku solver
  char cell[] = "R?C?#?";
  char inCell[] = "R?C?";
  char inRow[] = "R?#?";
  char inColumn[] = "C?#?";
  char inBox[] = "B?#?";

  // 324 columns
  char columns[strlen (inCell) * 81 + 81 + strlen (inRow) * 81 + 81 +
               strlen (inColumn) * 81 + 81 + strlen (inBox) * 81 + 81 + 1];
  *columns = 0;
  for (int i = 1; i <= 9; i++)
    for (int j = 1; j <= 9; j++)
    {
      inCell[1] = '0' + i;
      inCell[3] = '0' + j;
      strcat (columns, inCell);
      strcat (columns, "|");

      inRow[1] = '0' + i;
      inRow[3] = '0' + j;
      strcat (columns, inRow);
      strcat (columns, "|");

      inColumn[1] = '0' + i;
      inColumn[3] = '0' + j;
      strcat (columns, inColumn);
      strcat (columns, "|");

      inBox[1] = '0' + i;
      inBox[3] = '0' + j;
      strcat (columns, inBox);
      strcat (columns, "|");
    }

  Univers sudoku = dlx_univers_create (columns);

  char line[strlen (inCell) + 1 + strlen (inRow) + 1 + strlen (inColumn) + 1 + strlen (inBox) + 1];

  // 729 lines
  for (int row = 1; row <= 9; row++)
    for (int column = 1; column <= 9; column++)
      for (int number = 1; number <= 9; number++)
      {
        cell[1] = '0' + row;
        cell[3] = '0' + column;
        cell[5] = '0' + number;

        *line = 0;
        inCell[1] = '0' + row;
        inCell[3] = '0' + column;
        strcat (line, inCell);
        strcat (line, "|");

        inRow[1] = '0' + row;
        inRow[3] = '0' + number;
        strcat (line, inRow);
        strcat (line, "|");

        inColumn[1] = '0' + column;
        inColumn[3] = '0' + number;
        strcat (line, inColumn);
        strcat (line, "|");

        inBox[1] = '0' + 3 * ((row - 1) / 3) + (column - 1) / 3 + 1;
        inBox[3] = '0' + number;
        strcat (line, inBox);

        dlx_subset_define (sudoku, cell, line);
      }

  // Initial grid
  dlx_subset_require_in_solution (sudoku, "R1C4#8");
  dlx_subset_require_in_solution (sudoku, "R1C6#1");
  dlx_subset_require_in_solution (sudoku, "R2C8#4");
  dlx_subset_require_in_solution (sudoku, "R2C9#3");
  dlx_subset_require_in_solution (sudoku, "R3C1#5");
  dlx_subset_require_in_solution (sudoku, "R4C5#7");
  dlx_subset_require_in_solution (sudoku, "R4C7#8");
  dlx_subset_require_in_solution (sudoku, "R5C7#1");
  dlx_subset_require_in_solution (sudoku, "R6C2#2");
  dlx_subset_require_in_solution (sudoku, "R6C5#3");
  dlx_subset_require_in_solution (sudoku, "R7C1#6");
  dlx_subset_require_in_solution (sudoku, "R7C8#7");
  dlx_subset_require_in_solution (sudoku, "R7C9#5");
  dlx_subset_require_in_solution (sudoku, "R8C3#3");
  dlx_subset_require_in_solution (sudoku, "R8C4#4");
  dlx_subset_require_in_solution (sudoku, "R9C4#2");
  dlx_subset_require_in_solution (sudoku, "R9C7#6");

  dlx_exact_cover_search (sudoku, 0);

  dlx_univers_destroy (sudoku);
}

/// Pentomino unit test.
/// - F, L, N, P, and Y can be oriented in 8 ways: 4 by rotation, and 4 more for the mirror image.
///   Their symmetry group consists only of the identity mapping.
/// - T, and U can be oriented in 4 ways by rotation.
///   They have an axis of reflection aligned with the gridlines.
///   Their symmetry group has two elements, the identity and the reflection in a line parallel to the sides of the squares.
/// - V and W also can be oriented in 4 ways by rotation.
///   They have an axis of reflection symmetry at 45° to the gridlines.
///   Their symmetry group has two elements, the identity and a diagonal reflection.
/// - Z can be oriented in 4 ways: 2 by rotation, and 2 more for the mirror image.
///   It has point symmetry, also known as rotational symmetry of order 2.
///   Its symmetry group has two elements, the identity and the 180° rotation.
/// - I can be oriented in 2 ways by rotation.
///   It has two axes of reflection symmetry, both aligned with the gridlines.
///   Its symmetry group has four elements, the identity, two reflections and the 180° rotation.
///   It is the dihedral group of order 2, also known as the Klein four-group.
/// - X can be oriented in only one way.
///   It has four axes of reflection symmetry, aligned with the gridlines and the diagonals, and rotational symmetry of order 4.
///   Its symmetry group, the dihedral group of order 4, has eight elements.
///
/// The F, L, N, P, Y, and Z pentominoes are chiral; adding their reflections (F', J, N', Q, Y', S) brings the number of one-sided pentominoes to 18.
/// If rotations are also considered distinct, then the pentominoes from the first category count eightfold, the ones from the next three categories (T, U, V, W, Z) count fourfold, I counts twice, and X counts only once. This results in 5×8 + 5×4 + 2 + 1 = 63 fixed pentominoes.
/// The 8×8 rectangle with a 2×2 hole in the center, was solved by Dana Scott as far back as 1958, and has 65 solutions.
/// @see https://en.wikipedia.org/wiki/Pentomino
static void
test_pentomino (void)
{
/* *INDENT-OFF* */
  struct
  {
    char *name;
    struct
    {
      int x, y;
    } cell;
  } grid[] =
  {
    {"11", {0, 0}}, {"12", {0, 1}}, {"13", {0, 2}}, {"14", {0, 3}}, {"15", {0, 4}}, {"16", {0, 5}}, {"17", {0, 6}}, {"18", {0, 7}},
    {"21", {1, 0}}, {"22", {1, 1}}, {"23", {1, 2}}, {"24", {1, 3}}, {"25", {1, 4}}, {"26", {1, 5}}, {"27", {1, 6}}, {"28", {1, 7}},
    {"31", {2, 0}}, {"32", {2, 1}}, {"33", {2, 2}}, {"34", {2, 3}}, {"35", {2, 4}}, {"36", {2, 5}}, {"37", {2, 6}}, {"38", {2, 7}},
    {"41", {3, 0}}, {"42", {3, 1}}, {"43", {3, 2}},                                 {"46", {3, 5}}, {"47", {3, 6}}, {"48", {3, 7}},
    {"51", {4, 0}}, {"52", {4, 1}}, {"53", {4, 2}},                                 {"56", {4, 5}}, {"57", {4, 6}}, {"58", {4, 7}},
    {"61", {5, 0}}, {"62", {5, 1}}, {"63", {5, 2}}, {"64", {5, 3}}, {"65", {5, 4}}, {"66", {5, 5}}, {"67", {5, 6}}, {"68", {5, 7}},
    {"71", {6, 0}}, {"72", {6, 1}}, {"73", {6, 2}}, {"74", {6, 3}}, {"75", {6, 4}}, {"76", {6, 5}}, {"77", {6, 6}}, {"78", {6, 7}},
    {"81", {7, 0}}, {"82", {7, 1}}, {"83", {7, 2}}, {"84", {7, 3}}, {"85", {7, 4}}, {"86", {7, 5}}, {"87", {7, 6}}, {"88", {7, 7}},
  };

  struct
  {
    char *name;
    struct
    {
      int x, y;
    } tile[5];
    int rotation;
    int chirality;
  } pentomino[] =
  {
    {"I", {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}}, 2, 0},  // I  11  12  13  14  15
    {"N", {{0, 0}, {1, 0}, {1, 1}, {2, 1}, {3, 1}}, 4, 1},  // N  16  26  27  37  47, chiral
    {"L", {{0, 0}, {0, 1}, {1, 1}, {2, 1}, {3, 1}}, 4, 1},  // L  17  18  28  38  48, chiral
    {"U", {{0, 0}, {0, 1}, {1, 0}, {2, 0}, {2, 1}}, 4, 0},  // U  21  22  31  41  42
    {"X", {{0, 0}, {1,-1}, {1, 0}, {1, 1}, {2, 0}}, 1, 0},  // X  23  32  33  34  43
    {"W", {{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}}, 4, 0},  // W  24  25  35  36  46
    {"P", {{0, 0}, {0, 1}, {0, 2}, {1, 1}, {1, 2}}, 4, 1},  // P  51  52  53  62  63, chiral
    {"F", {{0, 0}, {1,-2}, {1,-1}, {1, 0}, {2,-1}}, 4, 1},  // F  56  64  65  66  75, chiral
    {"Z", {{0, 0}, {0, 1}, {1, 0}, {2,-1}, {2, 0}}, 2, 1},  // Z  57  58  67  76  77, chiral
    {"T", {{0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 0}}, 4, 0},  // T  61  71  72  73  81
    {"V", {{0, 0}, {1, 0}, {2,-2}, {2,-1}, {2, 0}}, 4, 0},  // V  68  78  86  87  88
    {"Y", {{0, 0}, {1,-2}, {1,-1}, {1, 0}, {1, 1}}, /*4, 1*/ 1, 0 /* breaking symetry */},  // Y  74  82  83  84  85, chiral
  };
/* *INDENT-ON* */

  // Initialize univers
  char *columns[sizeof (pentomino) / sizeof (*pentomino) + sizeof (grid) / sizeof (*grid)];

  for (int pento = 0; pento < sizeof (pentomino) / sizeof (*pentomino); pento++)
    columns[pento] = pentomino[pento].name;

  for (int cell = 0; cell < sizeof (grid) / sizeof (*grid); cell++)
    columns[sizeof (pentomino) / sizeof (*pentomino) + cell] = grid[cell].name;

  Univers univers = dlx_univers_create (sizeof (columns) / sizeof (*columns), (const char **) columns);

  // Initialize subsets
  int nb_subsets = 0;
  char *subset[1 + sizeof (pentomino[0].tile) / sizeof (*pentomino[0].tile)];

  int nb_fixed_pentominoes = 0;
  for (int pento = 0; pento < sizeof (pentomino) / sizeof (*pentomino); pento++)
  {
    subset[0] = pentomino[pento].name;
    for (int chirality = 1; chirality >= -1; chirality -= 2)
    {
      for (int rotation = 0; rotation < pentomino[pento].rotation; rotation++)
      {
        for (int cell = 0; cell < sizeof (grid) / sizeof (*grid); cell++)
        {
          int invalid_subset = 0;

          for (int tile = 0; tile < sizeof (pentomino[pento].tile) / sizeof (*pentomino[pento].tile); tile++)
          {
            int I = grid[cell].cell.x;
            int J = grid[cell].cell.y;

            switch (rotation)
            {
              case 0:
                I += pentomino[pento].tile[tile].x * chirality;
                J += pentomino[pento].tile[tile].y;
                break;
              case 1:
                I += pentomino[pento].tile[tile].y;
                J -= pentomino[pento].tile[tile].x * chirality;
                break;
              case 2:
                I -= pentomino[pento].tile[tile].x * chirality;
                J -= pentomino[pento].tile[tile].y;
                break;
              case 3:
                I -= pentomino[pento].tile[tile].y;
                J += pentomino[pento].tile[tile].x * chirality;
                break;
            }

            subset[1 + tile] = 0;
            for (int other_cell = 0; other_cell < sizeof (grid) / sizeof (*grid); other_cell++)
              if (grid[other_cell].cell.x == I && grid[other_cell].cell.y == J)
              {
                subset[1 + tile] = grid[other_cell].name;
                break;
              }

            if (subset[1 + tile] == 0)
            {
              invalid_subset = 1;
              break;
            }
          }                     // for tile

          if (!invalid_subset)
          {
            dlx_subset_define (univers, "", sizeof (subset) / sizeof (*subset), (const char **) subset);
            nb_subsets++;
		  }
        }                       // for cell
        nb_fixed_pentominoes++;
      }                         // for rotation

      if (!pentomino[pento].chirality)
        break;
    }                           // for chirality
  }                             // for pento
  fprintf (stderr, "%i fixed pentominoes.\n", nb_fixed_pentominoes);
  fprintf (stderr, "%i subsets defined.\n", nb_subsets);

  dlx_exact_cover_search (univers, 0);

  dlx_univers_destroy (univers);
}

static void
various_tests (void)
{
  // Set solution displayer.
  dlx_displayer_set (my_dlx_solution_displayer);

  //Test 2
  Univers m = dlx_univers_create ("A;B;C;D;E;F;G");

  dlx_subset_define (m, "L1", "C;E;F");
  dlx_subset_define (m, "L2", "A;D;G");
  dlx_subset_define (m, "L3", "B;C;F");
  dlx_subset_define (m, "L4", "A;D");
  dlx_subset_define (m, "L5", "B;G");
  dlx_subset_define (m, "L6", "D;E;G");

  dlx_subset_define (m, "L7", "A;B;C;D;E;F");
  dlx_subset_define (m, "Lg", "G");
  dlx_subset_define (m, "Le", "E");

  dlx_exact_cover_search (m, 0);

  dlx_univers_destroy (m);

  // Unset solution displayer.
  dlx_displayer_set (0);

  //Test 3
  m = dlx_univers_create ("A;A;B;A");

  dlx_subset_define (m, "L", "");
  dlx_subset_define (m, "L", "A");
  dlx_subset_define (m, "", "B");
  dlx_subset_define (m, "L", "A;B");

  dlx_exact_cover_search (m, 0);

  dlx_univers_destroy (m);

  //Test 4
  m = dlx_univers_create ("A;B");

  dlx_subset_define (m, "La", "A");

  dlx_exact_cover_search (m, 0);

  dlx_univers_destroy (m);

  //Test 4nis
  m = dlx_univers_create ("A;B");

  dlx_subset_define (m, "La", "A");

  dlx_subset_require_in_solution (m, "La");

  dlx_exact_cover_search (m, 0);

  dlx_univers_destroy (m);

  //Test 5
  m = dlx_univers_create ("A;B");

  dlx_subset_define (m, "La", "A");
  dlx_subset_define (m, "Lb", "B");

  dlx_subset_require_in_solution (m, "La");
  dlx_subset_require_in_solution (m, "Lb");

  dlx_exact_cover_search (m, 0);

  dlx_univers_destroy (m);

  //Test 6
  m = dlx_univers_create ("A;B");

  dlx_subset_define (m, "La", "A;A;H");
  dlx_subset_define (m, "Lb", "B");
  dlx_subset_define (m, "L", "A;B;A");

  dlx_subset_require_in_solution (m, "La");
  dlx_subset_require_in_solution (m, "Lb");

  dlx_exact_cover_search (m, 0);

  dlx_univers_destroy (m);

  //Test 7
  m = dlx_univers_create ("A;B");

  dlx_subset_define (m, "La", "A");
  dlx_subset_define (m, "Lb", "B");
  dlx_subset_define (m, "L", "A;B");

  dlx_subset_require_in_solution (m, "La");
  dlx_subset_require_in_solution (m, "L");

  dlx_exact_cover_search (m, 0);

  dlx_univers_destroy (m);
}

int
main (void)
{
  dlx_trace = 1;

  test_sudoku ();

  various_tests ();

  test_pentomino ();
}
