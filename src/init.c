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

void glider(int *board, int N, int nodeID) {
  Board(14,17)=1;
  Board(15,15)=1;
  Board(15,17)=1;
  Board(16,16)=1;
  Board(16,17)=1;
}

