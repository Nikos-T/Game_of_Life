/* #ifndef UTILS_H_   /\* Include guard *\/ */
/* #define UTILS_H_ */

#define Board(x,y) board[(x)*N + (y)]
#define NewBoard(x,y) newboard[(x)*N + (y)]

/* set everthing to zero */

void initialize_board (int *board, int N);

/* add to a width index, wrapping around like a cylinder */

int xadd (int i, int a, int N);

/* add to a height index, wrapping around */

int yadd (int i, int a, int N);

/* return the number of on cells adjacent to the i,j cell */

int adjacent_to (int *board, int i, int j, int N);
void alive_or_dead_center(int *board, int i, int j, int N, int *newboard) ;

/* play the game through one generation */

void play (int **b, int **nb, int N);
void play2 (int **b, int **nb, int N, int *boundaries, int nNodes);

/* print the life board */

void print(int *board, int N);

/* generate random table */

void generate_table (int *board, int N, float threshold, int nodeID);
void glider(int *board, int N, int nodeID);
void boundar(int *board, int N, int nodeID);

/* display the table with delay and clear console */

void display_table(int *board, int N);

/* #endif // FOO_H_ */
