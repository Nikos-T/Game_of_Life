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

#include <mpi.h>
#include <omp.h>

/*https://www.archer.ac.uk/training/course-material/2015/10/AdvMPI_EPCC/S1-L04-Split-Comms.pdf*/
int name_to_color(char *processor_name) {
  int hash=0, i=0;
  while((unsigned char)processor_name[i]!=0) {
    hash+=(unsigned char)processor_name[i];
    i++;
  }
  return hash;
}

int main (int argc, char *argv[]) {
  int   *board, *newboard, i;

  if (argc != 5) { // Check if the command line arguments are correct 
    printf("Usage: %s M N thres disp\n"
           "where\n"
           "  N     : size of table per process (N x N)\n"
           "  thres : propability of alive cell\n"
           "  t     : number of generations\n"
           "  disp  : {1: display output, 0: hide output}\n"
           , argv[0]);
    return (1);
  }
  
  // MPI init
  int provided;
  MPI_Init_thread( &argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  
  //Create one Comm per node and delete all but one MPI-process per node. Also set the omp threads.
  char *pname = malloc(MPI_MAX_PROCESSOR_NAME*sizeof(char));  //Maybe I should declare it first
  int len;
  MPI_Get_processor_name(pname, &len);
  int node_key = name_to_color(pname);
  MPI_Comm my_world;  //inside-processor communicator
  MPI_Comm_split(MPI_COMM_WORLD, node_key, 0, &my_world);
  int node_nthreads, TID; //TID is thread ID
  MPI_Comm_size(my_world, &node_nthreads);
  omp_set_num_threads(node_nthreads); //set threads per node
  MPI_Comm_rank(my_world, &TID);
  if (TID!=0) {
    MPI_Finalize();
    return(0);
  }
  int world_size; //don't need
  MPI_Comm_rank(MPI_COMM_WORLD, &TID);  //TID is task ID from MPI_COMM_WORLD
  MPI_Comm_size(MPI_COMM_WORLD, &world_size); //don't need
  
  //debug
  printf("Hello from %i of %i\n", TID, world_size);
  
  MPI_Comm_split(MPI_COMM_WORLD, 0, TID, &my_world);  //here my_world is outside-processor communicator
  int nNodes;
  MPI_Comm_size(my_world, &nNodes);
  MPI_Comm_rank(my_world, &TID);  //TID is node ID
  
  //debug
  printf("Hello from node %i of %i", TID, nNodes);
  
  
  /*temporary
  // Input command line arguments
  int N = atoi(argv[1]);        // Horizontal size
  double thres = atof(argv[2]); // Propability of life cell
  int t = atoi(argv[3]);        // Number of generations 
  int disp = atoi(argv[4]);     // Display output?
  printf("Size %dx%d with propability: %0.1f%%\n", M, N, thres*100);

  board = NULL;
  newboard = NULL;
  
  board = (int *)malloc(part*part*sizeof(int));

  if (board == NULL){
    printf("\nERROR: Memory allocation did not complete successfully!\n");
    return (1);
  }
  temporary*/
  /* second pointer for updated result */
  /*temporary
  newboard = (int *)malloc(part*part*sizeof(int));

  if (newboard == NULL){
    printf("\nERROR: Memory allocation did not complete successfully!\n");
    return (1);
  }

  initialize_board (board, N);
  printf("Board initialized\n");
  generate_table (board, N, thres);
  printf("Board generated\n");
  temporary*/
  /* play game of life 100 times */
  /*temporary
  for (i=0; i<t; i++) {
    if (disp) display_table (board, N);
    play (board, newboard, N);    
  }
  printf("Game finished after %d generations.\n", t);
  temporary*/
  MPI_Finalize();
  return(0);
}
