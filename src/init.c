#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <game-of-life.h>
#include <omp.h>
/* set everthing to zero */

void initialize_board (int *board, int N) {
  
  #pragma omp parallel for collapse(2)
  for (int i=0; i<N; i++) {
    for (int j=0; j<N; j++) {
      Board(i,j) = 0;
    }
  }
}

/* generate random table */

void generate_table (int *board, int N, float threshold, int nodeID) {
  #pragma omp parallel
  {
    srand(time(NULL)*nodeID+omp_get_thread_num());
  }
  int thres = threshold*RAND_MAX;
/*#pragma omp parallel for collapse(2)
  for (int j=0; j<N; j++) {
    for (int i=0; i<N; i++) {
      Board(i,j) = rand() < thres;
    }
  }*/
  
  //#pragma omp parallel for collapse(2)
  #pragma omp parallel for
  for (int i=0; i<N*N; i++) {
    board[i] = rand() < thres;
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

