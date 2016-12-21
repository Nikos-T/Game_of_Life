#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <game-of-life.h>

/* set everthing to zero */

void initialize_board (int *board, int N) {
  int   i, j;
  #pragma omp parallel for
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      Board(i,j) = 0;
    }
  }
}

/* generate random table */

void generate_table (int *board, int N, float threshold, int nodeID) {

  int   i, j;
  omp_set_num_threads(8);
  srand(time(NULL)+nodeID);
  #pragma omp parallel for num_threads(8)
  for (i=0; i<N; i++) {
    if (i=0) printf("\nNum threads = %i\n", omp_get_num_threads());
    for (j=0; j<N; j++) {
      Board(i,j) = ( (float)rand() / (float)RAND_MAX ) < threshold;
    }
  }
}

/*this function serves to test function transfer_boundaries*/
void boundar(int *board, int N, int nodeID) {
  #pragma omp parallel for
  for (int i=0; i<N; i+=nodeID+1) {
    Board(0, i)=1;
    Board(i, 0)=1;
    Board(N-1, i)=1;
    Board(i, N-1)=1;
  }
}

void glider(int *board, int N, int nodeID) {
  if (nodeID==0) {
    Board(4,7)=1;
    Board(5,5)=1;
    Board(5,7)=1;
    Board(6,6)=1;
    Board(6,7)=1;
  }
}

