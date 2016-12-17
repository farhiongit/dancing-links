#include <string.h>
#include "dancing_links.h"

int
main (void)
{
  //Test 1
  // Sudoku solver
  char cell[] = "R?C?#?";
  char inCell[] = "R?C?";
  char inRow[] = "R?#?";
  char inColumn[] = "C?#?";
  char inBox[] = "B?#?";

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
