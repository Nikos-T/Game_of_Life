#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <game-of-life.h>

/* add to a width index, wrapping around like a cylinder */

int xadd (int i, int a, int N) {
  i += a;
  while (i < 0) i += N;
  while (i >= N) i -= N;
  return i;
}

/* add to a height index, wrapping around */

int yadd (int i, int a, int N) {
  i += a;
  while (i < 0) i += N;
  while (i >= N) i -= N;
  return i;
}

/* return the number of on cells adjacent to the i,j cell */

int adjacent_to (int *board, int i, int j, int N) {
  int   k, l, count;

  count = 0;

  /* go around the cell */

  for (k=-1; k<=1; k++)
    for (l=-1; l<=1; l++)
      /* only count if at least one of k,l isn't zero */
      if (k || l)
        if (Board(xadd(i,k,N),yadd(j,l,N))) count++;
  return count;
}

void alive_or_dead_center(int *board, int i, int j, int N, int *newboard) {
  
  int count = 0;
  
  for (int k=-1; k<=1; k++) {
    for (int l=-1; l<=1; l++) {
      if (k||l) {
        if (Board(i+k, j+l)) {
          count++;
        }
      }
    }
  }
  if (count == 2) NewBoard(i,j) = Board(i,j);
  if (count == 3) NewBoard(i,j) = 1;
  if (count < 2) NewBoard(i,j) = 0;
  if (count > 3) NewBoard(i,j) = 0;
}
