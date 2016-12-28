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
  time_t start, end;
  
  srand(time(NULL)*(nodeID+1));
  int thres = threshold*RAND_MAX;
  time(&start);
  for (int i=0; i<N*N; i++) {
    board[i]=rand();
  }
  time(&end);
  printf("Node%i:\n%i seconds to populate with rands\n", nodeID, (int)(end-start));
  time(&start);
  #pragma omp parallel for collapse(2)
  for (int i=0; i<N; i++) {
    for (int j=0; j<N; j++) {
      Board(i,j)=Board(i,j)<thres;
    }
  }
  time(&end);
  printf("Node%i:\n%i seconds to compare with rands\n", nodeID, (int)(end-start));

  /*
  int res[omp_get_num_threads()];
  int thrID;
  #pragma omp parallel for
  for (int i=0; i<N*N; i++) {
    thrID=omp_get_thread_num();
    #pragma omp critical
    res[thrID] = rand();
    board[i] = res[thrID] < thres;
  }*/
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
  if (1) {
    Board(4,7)=1;
    Board(5,5)=1;
    Board(5,7)=1;
    Board(6,6)=1;
    Board(6,7)=1;
  }
}

