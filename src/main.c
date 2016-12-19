/*
 * Game of Life implementation based on
 * http://www.cs.utexas.edu/users/djimenez/utsa/cs1713-3/c/life.txt
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <game-of-life.h>

#include <string.h>   //memcpy
#include <mpi.h>
#include <omp.h>

MPI_Comm my_world;  //inside-processor communicator
int nodeID, nNodes;
MPI_Status status, status1, status2, status3;

/*https://www.archer.ac.uk/training/course-material/2015/10/AdvMPI_EPCC/S1-L04-Split-Comms.pdf*/
int name_to_color(char *processor_name) {
  int hash=0, i=0;
  while((unsigned char)processor_name[i]!=0) {
    hash+=(unsigned char)processor_name[i];
    i++;
  }
  return hash;
}

unsigned char encode( int * cells) {
  unsigned char coded8 = (cells[7]<<7) | (cells[6]<<6) | (cells[5]<<5) | (cells[4]<<4) | (cells[3]<<3) | (cells[2]<<2) | (cells[1]<<1) | cells[0];
  return coded8;
}

void decode( unsigned char coded8, int * cells) {
  #pragma omp parallel for
  for (int i=0; i<8; i++) {
    cells[i] = coded8 & 0x01;
    coded8 = coded8>>1;
  }
}

void transfer_board(int *board, int N, int *wholeboard, int *boundaries) {
  if (nNodes == 2) {
    unsigned char coded_board1[N*N/8];
    if (nodeID == 0) {
      MPI_Recv(coded_board1, N*N/8, MPI_UNSIGNED_CHAR, 1, 1, my_world, &status);
      //decode:
      #pragma omp parallel for
      for (int i=0; i<N*N/8; i++) {
        decode(coded_board1[i], &board[N*N+i*8]);
      }
      #pragma omp parallel for
      for (int i=0; i<N; i++) {
        #pragma omp parallel sections
        {
          #pragma omp section
          {
            boundaries[i]=wholeboard[N*N+i];
          }
          #pragma omp section
          {
            boundaries[N+i]=wholeboard[N*(2*N-1)+i];
          }
        }
      }
    } else {
      //encode:
      #pragma omp parallel for
      for (int i=0; i<N*N/8; i++) {
        coded_board1[i] = encode(&board[i*8]);
      }
      MPI_Send(coded_board1, N*N/8, MPI_UNSIGNED_CHAR, 0, 1, my_world);
    }
  } else if (nNodes ==4) {
    if (nodeID == 0) {
      unsigned char coded_columns[3*N*N/8];
      #pragma omp parallel for
      for (int i=0; i<N; i++) {
        memcpy(&wholeboard[2*N*i], &board[N*i], N*sizeof(int)); //copy board0 to wholeboard
      }
      //receive:
      #pragma omp parallel sections
      {
        #pragma omp section
        {
          for (int i=0; i<N; i++) {
            MPI_Recv(&coded_columns[(N+2*i)*N/8], N/8, MPI_UNSIGNED_CHAR, 1, i, my_world, &status1);    //2*N^2/8+2*i*N/8
            //printf("Received column %i from node1\n", (N+2*i));
          }
          printf("Received all columns from node1\n");
        }
        #pragma omp section
        {
          for (int i=0; i<N; i++) {
            MPI_Recv(&coded_columns[i*N/8], N/8, MPI_UNSIGNED_CHAR, 2, i, my_world, &status2);
            //printf("Received column %i from node2\n", i*N/8);
          }
          printf("Received all columns from node2\n");
        }
        #pragma omp section
        {
          for (int i=0; i<N; i++) {
            MPI_Recv(&coded_columns[(N+2*i+1)*N/8], N/8, MPI_UNSIGNED_CHAR, 3, i, my_world, &status3);    //2*N^2/8+(2*i+1)*N/8
            //printf("Received column %i from node3\n", (N+2*i+1));
          }
          printf("Received all columns from node3\n");
        }
      }
      //decode:
      #pragma omp parallel for
      for (int i=0; i<N; i++) {
        for (int j=0; j<N/8; j++) {
          decode(coded_columns[i*N/8+j], &wholeboard[(2*i+1)*N+8*j]);
        }
      }
      //create boundaries:
      #pragma omp parallel for
      for (int i=0; i<N; i++) {
        #pragma omp parallel sections
        {
          #pragma omp section
          {
            boundaries[i] = wholeboard[2*N*N+i];
          }
          #pragma omp section
          {
            boundaries[N+i] = wholeboard[2*N*(2*N-1)+i];
          }
          #pragma omp section
          {
            boundaries[2*N+i] = wholeboard[N*(1+2*i)];
          }
          #pragma omp section
          {
            boundaries[3*N+i] = wholeboard[2*N*(i+1)-1];
          }
        }
      } 
      #pragma omp parallel for
      for (int i=N*N/8; i<3*N*N/8; i++) {
        decode(coded_columns[i], &wholeboard[N*N+8*i]);
      }
    } else {
      unsigned char coded_columns[N*N/8];
      //encode:
      #pragma omp parallel for
      for (int i=0; i<N*N/8; i++) {
        coded_columns[i] = encode(&board[8*i]);
      }
      //Send
      for (int i=0; i<N; i++) {
        MPI_Send(&coded_columns[i*N/8], N/8, MPI_UNSIGNED_CHAR, 0, i, my_world);
      }
      printf("Node%i has sent data\n", nodeID);
    }
  }
}

void transfer_boundaries(int *board, int N, int *boundaries) {
  unsigned char coded[N/2];
  int uncoded[2*N];
  //rows to columns:
  if (nNodes == 4) {
    #pragma omp parallel for
    for (int i=0; i<N; i++) {
      #pragma omp parallel sections
      {
        #pragma omp section
        {
          uncoded[i] = i*N;
        }
        #pragma omp section
        {
          uncoded[N+i] = N*(1+i)-1;
        }
      }
    }
  }
  //encode:
  #pragma omp parallel for
  for (int i=0; i<N/8; i++) {
    #pragma omp parallel sections
    {
      #pragma omp section
      {
        coded[i] = encode(&board[8*i]);
      }
      #pragma omp section
      {
        coded[N/8+i] = encode(&board[N*(N-1)+8*i]);
      }
    }
  }
  if (nNodes == 4) {
    #pragma omp parallel for
    for (int i=0; i<N/8; i++) {
      #pragma omp parallel sections
      {
        #pragma omp section
        {
          coded[N/4+i] = encode(&uncoded[8*i]);
        }
        #pragma omp section
        {
          coded[3*N/8+i] = encode(&uncoded[N+8*i]);
        }
      }
    }
  }
  //send-receive
  switch(nodeID) {  //tag 0 means column, tag 1 means row
    case 0:
      MPI_Send(coded, N/4, MPI_UNSIGNED_CHAR, 1, 0, my_world);
      if (nNodes==4)  MPI_Send(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 2, 1, my_world);
      MPI_Recv(coded, N/4, MPI_UNSIGNED_CHAR, 1, 0, my_world, &status);
      if (nNodes==4)  MPI_Recv(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 2, 1, my_world, &status);
      break;
    case 1:
      MPI_Send(coded, N/4, MPI_UNSIGNED_CHAR, 0, 0, my_world);
      if (nNodes==4)  MPI_Send(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 3, 1, my_world);
      MPI_Recv(coded, N/4, MPI_UNSIGNED_CHAR, 0, 0, my_world, &status);
      if (nNodes==4)  MPI_Recv(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 3, 1, my_world, &status);
      break;
    case 2:
      MPI_Send(coded, N/4, MPI_UNSIGNED_CHAR, 3, 0, my_world);
      MPI_Send(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 0, 1, my_world);
      MPI_Recv(coded, N/4, MPI_UNSIGNED_CHAR, 3, 0, my_world, &status);
      MPI_Recv(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 0, 1, my_world, &status);
      break;
    case 3:
      MPI_Send(coded, N/4, MPI_UNSIGNED_CHAR, 2, 0, my_world);
      MPI_Send(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 1, 1, my_world);
      MPI_Recv(coded, N/4, MPI_UNSIGNED_CHAR, 2, 0, my_world, &status);
      MPI_Recv(&coded[N/4], N/4, MPI_UNSIGNED_CHAR, 1, 1, my_world, &status);
      break;
  }
  //decode:
  #pragma omp parallel for
  for (int i=0; i<N*nNodes/8; i++) {
    decode(coded[i], &boundaries[8*i]);
  }
}

int main (int argc, char *argv[]) {
  int   *board, *newboard, *wholeboard;

  if (argc != 5) { // Check if the command line arguments are correct 
    printf( "Usage: %s N thres disp\n"
            "where\n"
            "  N     : size of table (N x N)\n"
            "  thres : propability of alive cell\n"
            "  t     : number of generations\n"
            "  disp  : {1: display output, 0: hide output}\n"

           , argv[0]);
    return (1);
  }
  
  /*Initialize MPI*/
  int provided;
  MPI_Init_thread( &argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  if (provided!=3) {
    printf("Could not provide MULTIPLE thread calling");
    MPI_Finalize();
    return(3);
  }

  /*Procedure to delete all but one task per node
   * Firstly we create MPI_Comm which splits MPI_COMM_WORLD based on processor name and get an "inside-node" communicator
   * Secondly we set the number of openMP threads based on the "inside-node" comm size
   * Thirldly we split MPI_COMM_WORLD based on ranks of the "inside-node" comm and get the node communicator
   * Fourthly we pass the size and ranks of the node communicator to global vars
   * Lastly we delete all but one process per node based on the ranks of the "inside-node" comm (first communicator)*/
  char *pname = malloc(MPI_MAX_PROCESSOR_NAME*sizeof(char));
  int len, node_key, node_nthreads, threadID, TID;
  MPI_Get_processor_name(pname, &len);
  node_key = name_to_color(pname);  //hash processor name to a unique integer value
  MPI_Comm_split(MPI_COMM_WORLD, node_key, 0, &my_world); //here my_world is "inside-node" communicator
  MPI_Comm_size(my_world, &node_nthreads);
  omp_set_num_threads(node_nthreads); //set omp threads per node
  MPI_Comm_rank(my_world, &threadID);
  MPI_Comm_rank(MPI_COMM_WORLD, &TID);  //TID is task ID from MPI_COMM_WORLD
  MPI_Comm_split(MPI_COMM_WORLD, threadID, TID, &my_world); //here my_world is node communicator
  MPI_Comm_size(my_world, &nNodes); //pass size of communicator to global variable
  MPI_Comm_rank(my_world, &nodeID); //pass nodeID to global variable (needed for send-recv)
  if (threadID!=0) {  //close all but one process per node
    MPI_Finalize();
    return(0);
  }
  if (nNodes!=1 && nNodes!=2 && nNodes!=4) {  //check nodes=1,2 or 4
    printf("This many nodes not supported");
    MPI_Finalize();
    return(-1);
  }
  
  // Input command line arguments
  int N = atoi(argv[1]);        // Array size
  N+=8-N%8;                     // for encode-decode
  double thres = atof(argv[2]); // Propability of life cell
  int t = atoi(argv[3]);        // Number of generations 
  int disp = atoi(argv[4]);     // Display output?
  printf("Size %dx%d with propability: %0.1f%%\n", N, N, thres*100);

  board = NULL;
  newboard = NULL;
  
  /*Define board and wholeboard*/
  board = (int *)malloc(N*N*sizeof(int));
  if (nodeID == 0) {
    if (nNodes==2) {  //in this case board doesn't need to be redefined
      board = (int *) realloc(board, 2*N*N*sizeof(int));
      wholeboard = board;
    } else if (nNodes==4) { //here column size changes so we need the variable wholeboard
      wholeboard = (int *)malloc(4*N*N*sizeof(int));
      if ((wholeboard == NULL) && (nNodes >1)) {
        printf("\nERROR: Memory allocation did not complete successfully!\n");
        return (1);
      }
    }
  }
  
  /*Define boundaries*/
  int * boundaries;
  if (nNodes == 2) {  //if nodes=2 then we need to pass 2 columns to each task
    boundaries = (int *)malloc(2*N*sizeof(int));
  } else if (nNodes == 4) { //if nodes=4 we need to pass 2 columns and to rows to each task
    boundaries = (int *)malloc(4*N*sizeof(int));
  }
  
  /* second pointer for updated result */
  newboard = (int *)malloc(N*N*sizeof(int));
  if ((board == NULL) || ((boundaries == NULL ) && (nNodes > 1)) || (newboard == NULL)){
    printf("\nERROR: Memory allocation did not complete successfully!\n");
    return (1);
  }

  initialize_board (board, N);
  printf("Board%i initialized\n", nodeID);
  generate_table (board, N, thres, nodeID);  //Usually every board is generated in the same second. Simply adding nodeID to time(NULL) makes the boards differ
  printf("Board generated\n");
  
  /*play game of life*/
  if (nNodes == 1) {
    for (int i=0; i<t; i++) {
      if (disp) display_table(board, N, N);
      play(board, newboard, N, boundaries);
    }
  } else {
    for (int i=0; i<t; i++) {
      if (disp) {
        transfer_board(board, N, wholeboard, boundaries);
        if (nodeID==0) display_table(board, 2*N, nNodes*N/2);
      } else {
        transfer_boundaries(board, N, boundaries);
      }
      play(board, newboard, N, boundaries);
    }
  }
  
  if (disp) { //diplay finish board
    transfer_board(board, N, wholeboard, boundaries);
    if (nodeID==0) display_table(board, 2*N, nNodes*N/2);
  }
  
  /*Free mallocs*/
  if (nNodes>1) free(boundaries);
  free(board);
  free(newboard);
  if (nodeID==0 && nNodes==4) free(wholeboard);
  
  MPI_Finalize();
  return (0);
}
