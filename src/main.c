/*
 * Game of Life implementation based on
 * http://www.cs.utexas.edu/users/djimenez/utsa/cs1713-3/c/life.txt
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sys/time.h"
#include <game-of-life.h>

#include <mpi.h>
#include <omp.h>

int nodeID, nNodes;
MPI_Status status;
struct timeval tstart, tend;

unsigned char encode( int * cells) {
  unsigned char coded8 = (cells[7]<<7) | (cells[6]<<6) | (cells[5]<<5) | (cells[4]<<4) | (cells[3]<<3) | (cells[2]<<2) | (cells[1]<<1) | cells[0];
  return coded8;
}

void decode( unsigned char coded8, int * cells) {
  for (int i=0; i<8; i++) {
    cells[i] = coded8 & 0x01;
    coded8 = coded8>>1;
  }
}

void transfer_boundaries(int *board, int N, int *boundaries) {
  unsigned char coded[N/2];
  int uncoded[2*N];
  int corners[4];
  //rows to columns:
  if (nNodes == 4) {
    #pragma omp parallel for
    for (int i=0; i<N; i++) {
      uncoded[i] = board[i*N];
      uncoded[N+i] = board[N*(1+i)-1];
    }
    corners[0] = Board(N-1,N-1);
    corners[1] = Board(N-1,0);
    corners[2] = Board(0,N-1);
    corners[3] = Board(0,0);
  }
  //encode:
  #pragma omp parallel for
  for (int i=0; i<N/8; i++) {
    coded[i] = encode(&board[8*i]);
    coded[N/8+i] = encode(&board[N*(N-1)+8*i]);
  }
  if (nNodes == 4) {
    #pragma omp parallel for
    for (int i=0; i<N/8; i++) {
      coded[N/4+i] = encode(&uncoded[8*i]);
      coded[3*N/8+i] = encode(&uncoded[N+8*i]);
    }
  }
  //send-receive
  switch(nodeID) {  //tag 0 means column, tag 1 means row
    case 0:
      MPI_Send(coded, N/4, MPI_UNSIGNED_CHAR, 1, 0, MPI_COMM_WORLD);
      if (nNodes==4) {
        MPI_Send(corners, 4, MPI_INT, 3, 0, MPI_COMM_WORLD);
        MPI_Send(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 2, 1, MPI_COMM_WORLD);
        MPI_Recv(&boundaries[4*N], 4, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 2, 1, MPI_COMM_WORLD, &status);
      }
      MPI_Recv(coded, N/4, MPI_UNSIGNED_CHAR, 1, 0, MPI_COMM_WORLD, &status);
      break;
    case 1:
      MPI_Send(coded, N/4, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
      if (nNodes==4) {
        MPI_Send(corners, 4, MPI_INT, 2, 0, MPI_COMM_WORLD);
        MPI_Send(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 3, 1, MPI_COMM_WORLD);
        MPI_Recv(&boundaries[4*N], 4, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 3, 1, MPI_COMM_WORLD, &status);
      }
      MPI_Recv(coded, N/4, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, &status);
      break;
    case 2:
      MPI_Send(coded, N/4, MPI_UNSIGNED_CHAR, 3, 0, MPI_COMM_WORLD);
      MPI_Send(corners, 4, MPI_INT, 1, 0, MPI_COMM_WORLD);
      MPI_Send(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD);
      MPI_Recv(&boundaries[4*N], 4, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(coded, N/4, MPI_UNSIGNED_CHAR, 3, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD, &status);
      break;
    case 3:
      MPI_Send(coded, N/4, MPI_UNSIGNED_CHAR, 2, 0, MPI_COMM_WORLD);
      MPI_Send(corners, 4, MPI_INT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 1, 1, MPI_COMM_WORLD);
      MPI_Recv(&boundaries[4*N], 4, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(coded, N/4, MPI_UNSIGNED_CHAR, 2, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 1, 1, MPI_COMM_WORLD, &status);
      break;
  }
  //decode:
  #pragma omp parallel for
  for (int i=0; i<N*nNodes/8; i++) {
    decode(coded[i], &boundaries[8*i]);
  }
}

void display_table2(int *board, int N) {
  for (int j=0; j<N; j++) {
    MPI_Barrier(MPI_COMM_WORLD);
    usleep(5000);
    if (nodeID==0) {
      for (int i=0; i<N; i++) {
        printf ("%c", Board(i,j) ? 'x' : ' ');
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    usleep(5000);
    if (nodeID==1) {
      for (int i=0; i<N; i++) {
        printf ("%c", Board(i,j) ? 'x' : ' ');
      }
      printf("\n");
    }
  }
  if (nNodes == 2) {
    usleep(50000);
    return;
  }
  for (int j=0; j<N; j++) {
    MPI_Barrier(MPI_COMM_WORLD);
    usleep(5000);
    if (nodeID==2) {
      for (int i=0; i<N; i++) {
        printf ("%c", Board(i,j) ? 'x' : ' ');
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    usleep(5000);
    if (nodeID==3) {
      for (int i=0; i<N; i++) {
        printf ("%c", Board(i,j) ? 'x' : ' ');
      }
      printf("\n");
    }
  }
  usleep(50000);
}

int main (int argc, char *argv[]) {
  int   *board, *newboard;

  if (argc != 6 && argc != 5) { // Check if the command line arguments are correct 
    printf( "Usage: %s N thres disp\n"
            "where\n"
            "  N     : size of table (N x N) per NODE\n"
            "  thres : propability of alive cell\n"
            "  t     : number of generations\n"
            "  disp  : {1: display output, 0: hide output}\n"
            "  glid  : {1: use glider, 0:random} (optional)\n"
           , argv[0]);
    return (1);
  }
  
  /*Initialize MPI*/
  {
  int provided;
  MPI_Init_thread( &argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  if (provided!=3) {
    printf("Could not provide MULTIPLE thread calling");
    MPI_Finalize();
    return(3);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &nodeID);  //TID is task ID from MPI_COMM_WORLD
  MPI_Comm_size(MPI_COMM_WORLD, &nNodes);
  if (nNodes!=1 && nNodes!=2 && nNodes!=4) {  //check nodes=1,2 or 4
    printf("nNodes = %i\nThis many nodes not supported\n", nNodes);
    MPI_Finalize();
    return(-1);
  }
  }

  // Input command line arguments
  int N = atoi(argv[1]);        // Array size
  if (N%8!=0) N+=8-N%8;         // for encode-decode
  double thres = atof(argv[2]); // Propability of life cell
  int t = atoi(argv[3]);        // Number of generations 
  int disp = atoi(argv[4]);     // Display output?
  int glid = 0;                 // gosper glider gun
  if (argc == 6) glid = atoi(argv[5]);     
  printf("Size %dx%d with propability: %0.1f%%\n", N, N, thres*100);
  
  board = NULL;
  newboard = NULL;
  
  /*Define board and wholeboard*/
  board = (int *)malloc(N*N*sizeof(int));
  if (board == NULL) {
    printf("\nERROR: Memory allocation did not complete successfully!\n");
    return (1);
  }
  
  /*Define boundaries*/
  int * boundaries;
  boundaries = (int *)malloc(4*(N+1)*sizeof(int));

  
  /* second pointer for updated result */
  newboard = (int *)malloc(N*N*sizeof(int));
  if ((board == NULL) || ((boundaries == NULL ) && (nNodes > 1)) || (newboard == NULL)){
    printf("\nERROR: Memory allocation did not complete successfully!\n");
    return (1);
  }
  
  /* initialize and generate board */
  gettimeofday(&tstart, NULL); 
  initialize_board (board, N);
  gettimeofday(&tend, NULL);
  printf("\nNode %i:\n%f seconds to initialize board\n", nodeID, (double)((tend.tv_usec - tstart.tv_usec)/1.0e6 + tend.tv_sec - tstart.tv_sec));
  gettimeofday(&tstart, NULL); 
  if (glid) glider(board, N, nodeID); //for debug purposes
  else generate_table (board, N, thres, nodeID);
  gettimeofday(&tend, NULL);
  printf("\nNode %i:\n%f seconds to generate board\n", nodeID, (double)((tend.tv_usec - tstart.tv_usec)/1.0e6 + tend.tv_sec - tstart.tv_sec));

  /* play game of life*/
  if (nNodes == 1) {
    for (int i=0; i<t; i++) {
      gettimeofday(&tstart, NULL);
      if (disp) display_table(board, N);
      play2(&board, &newboard, N, boundaries, nNodes);
      gettimeofday(&tend, NULL);
      printf("\n%f seconds to play round\n", (double)((tend.tv_usec - tstart.tv_usec)/1.0e6 + tend.tv_sec - tstart.tv_sec));
    }
  } else {
    for (int i=0; i<t; i++) {
      MPI_Barrier(MPI_COMM_WORLD);
      gettimeofday(&tstart, NULL);
      if (disp) {
        display_table2(board, N);
      }
      transfer_boundaries(board, N, boundaries);
      play2(&board, &newboard, N, boundaries, nNodes);
      gettimeofday(&tend, NULL);
      printf("\nNode%i\n%f seconds to play round\n", nodeID, (double)((tend.tv_usec - tstart.tv_usec)/1.0e6 + tend.tv_sec - tstart.tv_sec));
    }
  }
  /* display final table */
  if (disp) {
    printf("\nFinal Table:\n=================\n");
    if (nNodes>1) display_table2(board, N);
    else display_table(board, N);
  }
  /*Free mallocs*/
  free(boundaries);
  free(board);
  free(newboard);
  MPI_Finalize();
  return (0);
}
