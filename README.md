# dancing-links
An implementation of Knuth's dancing links algorithm for exact cover search.

Dancing links, Donald E. Knuth, Submitted on 15 Nov 2000 (see  http://en.wikipedia.org/wiki/Dancing_Links,
http://arxiv.org/abs/cs/0011047 and https://arxiv.org/pdf/cs/0011047v1.pdf).

In computer science, Dancing Links, also known as DLX, is the technique suggested by Donald Knuth to
efficiently implement his Algorithm X. Algorithm X is a recursive, nondeterministic, depth-first, backtracking
algorithm that finds all solutions to the exact cover problem.

Some of the better-known exact cover problems include tiling, the n queens problem, and Sudoku.
The name Dancing Links comes from the way the algorithm works, as iterations of the algorithm cause the links
to "dance" with partner links so as to resemble an "exquisitely choreographed dance."

Knuth credits Hiroshi Hitotsumatsu and Kohei Noshita with having invented the idea in 1979,
but it is his paper which has popularized it.

See also http://en.wikipedia.org/wiki/Exact_cover
and http://en.wikipedia.org/wiki/Exact_cover#Sudoku for more.

dancing_links.h and dancing_links.c are used to built a static library libdlx.a.
- For details of the interface of this library, read comments in dancing_links.h.
- For details of the implementation, read comments in dancing_links.c.

Documentation (HTML and PDF) can also be generated by doxygen using command "make doc".

For usage of the library, look at examples in main.c, which file is intented for unit testing purpose only.

Code compiles with C compiler clang only as it makes use of  __attribute__ ((overloadable)) which is a C language extension (see http://clang.llvm.org/docs/AttributeReference.html#overloadable).
