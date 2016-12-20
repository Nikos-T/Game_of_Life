#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <game-of-life.h>

void play (int *board, int *newboard, int N) {
  /*
    (copied this from some web page, hence the English spellings...)

    1.STASIS : If, for a given cell, the number of on neighbours is 
    exactly two, the cell maintains its status quo into the next 
    generation. If the cell is on, it stays on, if it is off, it stays off.

    2.GROWTH : If the number of on neighbours is exactly three, the cell 
    will be on in the next generation. This is regardless of the cell's
    current state.

    3.DEATH : If the number of on neighbours is 0, 1, 4-8, the cell will 
    be off in the next generation.
  */
  int   i, j, a;

  /* for each cell, apply the rules of Life */
  
  for (i=0; i<N; i++)
    for (j=0; j<N; j++) {
      a = adjacent_to (board, i, j, N);
      if (a == 2) NewBoard(i,j) = Board(i,j);
      if (a == 3) NewBoard(i,j) = 1;
      if (a < 2) NewBoard(i,j) = 0;
      if (a > 3) NewBoard(i,j) = 0;
    }

  /* copy the new board back into the old board */

  for (i=0; i<N; i++)
    for (j=0; j<N; j++) {
      Board(i,j) = NewBoard(i,j);
    }
}

void play2(int *board, int *newboard, int N, int *boundaries, int nNodes) {
  
  int a=0, k, l;
  #pragma omp parallel for
  for (int i=1; i<N-1; i++) {
    #pragma omp parallel for
    for (int j=1; j<N-1; j++) {
      for (k=-1; k<=1; k++) {
        for (l=-1; l<=1; l++) {
          /* only count if at least one of k,l isn't zero */
          if (k || l) {
            if (Board(i+k,j+l)) a++;
          }
        }
      }
      if (a == 2) NewBoard(i,j) = Board(i,j);
      if (a == 3) NewBoard(i,j) = 1;
      if (a < 2) NewBoard(i,j) = 0;
      if (a > 3) NewBoard(i,j) = 0;
      a=0;
    }
  }
  if (nNodes == 2) {
    #pragma omp parallel for
    for (int i=0; i<N; i++) {
      boundaries[2*N+i] = Board(0, i);
      boundaries[3*N+i] = Board(N-1, i);
    }
    //corners:
    boundaries[4*N] = boundaries[2*N-1];
    boundaries[4*N+1] = boundaries[N];
    boundaries[4*N+2] = boundaries[N-1];
    boundaries[4*N+3] = boundaries[0];
  }
  
  #pragma omp parallel for
  for (int i=1; i<N-1; i++) {
    //j=0:
    a=boundaries[3*N+i-1]+boundaries[3*N+i]+boundaries[3*N+i+1]+Board(i-1,0)+Board(i+1,0)+Board(i-1, 1)+Board(i, 1)+Board(i+1, 1);
    if (a == 2) NewBoard(i,0) = Board(i,0);
    if (a == 3) NewBoard(i,0) = 1;
    if (a < 2) NewBoard(i,0) = 0;
    if (a > 3) NewBoard(i,0) = 0;
    //j=N-1:
    a=boundaries[2*N+i-1]+boundaries[2*N+i]+boundaries[2*N+i+1]+Board(i-1, N-1)+Board(i+1, N+1)+Board(i-1, N-2)+Board(i, N-2)+Board(i+1, N-2);
    if (a == 2) NewBoard(i,N-1) = Board(i,N-1);
    if (a == 3) NewBoard(i,N-1) = 1;
    if (a < 2) NewBoard(i,N-1) = 0;
    if (a > 3) NewBoard(i,N-1) = 0;
    //from here i means j and opposite
    //i=0:
    a=boundaries[N+i-1]+boundaries[N+i]+boundaries[N+i+1]+Board(0, i-1)+Board(0, i+1)+Board(1, i-1)+Board(1, i)+Board(1, i+1);
    if (a == 2) NewBoard(0,i) = Board(0,i);
    if (a == 3) NewBoard(0,i) = 1;
    if (a < 2) NewBoard(0,i) = 0;
    if (a > 3) NewBoard(0,i) = 0;
    //i=N-1
    a=boundaries[i-1]+boundaries[i]+boundaries[i+1]+Board(N-1, i-1)+Board(N-1, i+1)+Board(N-2, i-1)+Board(N-2,i)+Board(N-2, i+1);
    if (a == 2) NewBoard(N-1,i) = Board(N-1,i);
    if (a == 3) NewBoard(N-1,i) = 1;
    if (a < 2) NewBoard(N-1,i) = 0;
    if (a > 3) NewBoard(N-1,i) = 0;
  }
  //corners:
  //(0,0):
  //sections?
  a = boundaries[N]+boundaries[N+1]+boundaries[3*N]+boundaries[3*N+1]+boundaries[4*N]+Board(0,1)+Board(1,0)+Board(1,1);
  if (a == 2) NewBoard(0,0) = Board(0,0);
  if (a == 3) NewBoard(0,0) = 1;
  if (a < 2) NewBoard(0,0) = 0;
  if (a > 3) NewBoard(0,0) = 0;
  //(0, N-1):
  a = boundaries[3*N]+boundaries[3*N+1]+boundaries[2*N-1]+boundaries[2*N-2]+boundaries[4*N+1]+Board(0, N-2)+Board(1, N-2)+Board(1, N-1);
  if (a == 2) NewBoard(0,N-1) = Board(0,N-1);
  if (a == 3) NewBoard(0,N-1) = 1;
  if (a < 2) NewBoard(0,N-1) = 0;
  if (a > 3) NewBoard(0,N-1) = 0;
  //(N-1, 0):
  a = boundaries[0]+boundaries[1]+boundaries[4*N-1]+boundaries[4*N-2]+boundaries[4*N+2]+Board(N-2, 0)+Board(N-2, 1)+Board(N-1, 1);
  if (a == 2) NewBoard(N-1,0) = Board(N-1,0);
  if (a == 3) NewBoard(N-1,0) = 1;
  if (a < 2) NewBoard(N-1,0) = 0;
  if (a > 3) NewBoard(N-1,0) = 0;
  //(N-1, N-1):
  a = boundaries[N-1]+boundaries[N-2]+boundaries[3*N-1]+boundaries[3*N-2]+boundaries[4*N+3]+Board(N-1, N-2)+Board(N-2, N-1)+Board(N-2,N-2);
  if (a == 2) NewBoard(N-1,N-1) = Board(N-1,N-1);
  if (a == 3) NewBoard(N-1,N-1) = 1;
  if (a < 2) NewBoard(N-1,N-1) = 0;
  if (a > 3) NewBoard(N-1,N-1) = 0;
  
  /* copy the new board back into the old board */
  #pragma omp parallel for
  for (int i=0; i<N; i++)
    for (int j=0; j<N; j++) {
      Board(i,j) = NewBoard(i,j);
    }
}