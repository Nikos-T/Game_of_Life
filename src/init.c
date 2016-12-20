#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <game-of-life.h>

/* set everthing to zero */

void initialize_board (int *board, int N) {
  int   i, j;
  
  for (i=0; i<N; i++)
    for (j=0; j<N; j++) 
      Board(i,j) = 0;
}

/* generate random table */

void generate_table (int *board, int N, float threshold, int nodeID) {

  int   i, j;
  int counter = 0;

  srand(time(NULL)+nodeID);

  for (j=0; j<N; j++) {

    for (i=0; i<N; i++) {
      Board(i,j) = ( (float)rand() / (float)RAND_MAX ) < threshold;
      counter += Board(i,j);
    }
  }
}

void gosper_glider_gun (int *board, int N, int nodeID) {
  int i[17], j[17];
  switch (nodeID) {
    case 0:
      i={1, 1, 2, 2, 9, 9, 10, 10, 11, 11, 17, 17, 17, 18, 19, 23, 23};
      j={11, 12, 11, 12, 12, 13, 11, 13, 11, 12, 13, 14, 15, 13, 14, 10, 11};
    case 1:
      i={0, 0, 1, 1, 1, 1, 2, 2, 11, 11, 12, 12, 12, 12, 12, 13, 14};
      j={9, 11, 9, 10, 21, 22, 21, 23, 21, 9, 10, 9, 10, 16, 18, 16, 17};
  }
  for (int k=0; k<17; k++) {
    Board(i[k], j[k])=1;
  }
}

