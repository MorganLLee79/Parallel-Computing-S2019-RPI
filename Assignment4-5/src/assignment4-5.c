/***************************************************************************/
/* Template for Asssignment 4/5 ********************************************/
/* Eric Johnson, Harrison Lee    *******************************************/
/***************************************************************************/

/***************************************************************************/
/* Includes ****************************************************************/
/***************************************************************************/

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clcg4.h>

#include <mpi.h>
#include <pthread.h>

#ifdef __bgq__
#include <hwi/include/bqc/A2_inlines.h>
#else
#define GetTimeBase MPI_Wtime
#endif

/***************************************************************************/
/* Defines *****************************************************************/
/***************************************************************************/

#define ALIVE 1
#define DEAD 0

#define ROW_LENGTH 32768 // 32,768

/***************************************************************************/
/* Global Vars *************************************************************/
/***************************************************************************/

typedef unsigned char cell_t;

double g_time_in_secs = 0;
double g_processor_frequency = 1600000000.0; // processing speed for BG/Q
unsigned long long g_start_cycles = 0;
unsigned long long g_end_cycles = 0;

// TODO: temporary, probably replace with a function to handle ghost rows
cell_t **board;

// Per-experiment values
/// Total number of threads to use for each rank, including the initial thread
int threads_per_rank;
double threshold;
int number_ticks;

int rows_per_rank; // Use for getting RNG stream indices; [local_row +
                   // (rows_per_rank * mpiRank)]

cell_t *ghost_row_top;
cell_t *ghost_row_bot;

// Track the number of alive cells per tick
int *alive_cells;

/***************************************************************************/
/* Function Decs ***********************************************************/
/***************************************************************************/

// You define these
/**
 * Updates a single cell at the specified position according to the rules.
 */
void update_cell(int row, int col);

void *run_simulation(int *thread_num);

/***************************************************************************/
/* Function: Main **********************************************************/
/***************************************************************************/

int main(int argc, char *argv[]) {
  int mpi_rank;
  int mpi_size;
  // Example MPI startup and using CLCG4 RNG
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

  // Init 32,768 RNG streams - each rank has an independent stream
  InitDefault();

  // Note, used the mpi_rank to select which RNG stream to use.
  // You must replace mpi_rank with the right row being used.
  // This just show you how to call the RNG.
  printf("Rank %d of %d has been started and a first Random Value of %lf\n",
         mpi_rank, mpi_size, GenVal(mpi_rank));

  MPI_Barrier(MPI_COMM_WORLD);

  // Insert your code

  // Set up initial variables
  // Experimental variables
  if (argc != 4) {
    printf("Error: Expecting threads_per_rank, threshold (0.25), and "
           "number_ticks\n");
    return -1;
  }
  threads_per_rank = atoi(argv[1]);
  threshold = atof(argv[2]);
  number_ticks = atoi(argv[3]);

  // error handling
  if (threads_per_rank <= 0) {
    printf("Error: threads_per_rank must be greater than 0\n");
    return -1;
  }

  if (number_ticks <= 0) {
    printf("Error: number_ticks must be greater than 0\n");
    return -1;
  }

  // Start recording time base
  if (mpi_rank == 0) {
    g_start_cycles = GetTimeBase();
  }

  // Allocate universe chunks and "ghost" rows

  // Initialize universe, all cells alive
  board = calloc(ROW_LENGTH, ROW_LENGTH / mpi_size);
  memset(board, ALIVE, ROW_LENGTH * ROW_LENGTH / mpi_size);

  // Initialize/allocate ghost rows
  ghost_row_top = calloc(sizeof(cell_t), ROW_LENGTH);
  memset(ghost_row_top, ALIVE, ROW_LENGTH);
  ghost_row_bot = calloc(sizeof(cell_t), ROW_LENGTH);
  memset(ghost_row_bot, ALIVE, ROW_LENGTH);

  // Initialize/allocate alive_cells
  alive_cells = calloc(sizeof(int), number_ticks);
  memset(alive_cells, 0, number_ticks);

  // Create Pthreads, go into for-loop
  int *thread_num;
  // allocate array to hold thread handles
  pthread_t *threads = calloc(threads_per_rank - 1, sizeof *threads);
  // create n-1 more threads
  for (int i = 0; i < threads_per_rank - 1; ++i) {
    thread_num = malloc(sizeof(int));
    *thread_num = i + 1;
    pthread_create(&threads[i], NULL, (void *(*)(void *))run_simulation,
                   (void *)thread_num);
  }

  // we are thread 0, so we need to run the simulation as well
  thread_num = malloc(sizeof(int));
  *thread_num = 0;
  run_simulation(thread_num);

  // Simulation finished
  // join all threads
  for (int i = 0; i < threads_per_rank - 1; ++i) {
    pthread_join(threads[i], NULL);
  }
  free(threads);

  // MPI_reduce sums of alive cells per tick; will be a vector (array) for all
  // ticks

  // Stop timer
  if (mpi_rank == 0) {
    g_end_cycles = GetTimeBase();
    g_time_in_secs =
        ((double)(g_end_cycles - g_start_cycles) / g_processor_frequency);
  }

  // If needed for this run/experiment:
  // Output using MPI_file_write_at
  // Collect I/O performance from rank 0/thread 0

  // If needed for this run/experiment:
  // Construct heatmap of 32kx32k cell universe
  // Use MPI collective operations of your choice.
  // Rank 0 will output the heat map to a standard unix file. Expecting small
  // 1-4MB size Import data for graphing

  if (mpi_rank == 0) {
    // Print alive tick stats, compute (I/O if needed) performance stats
  }

  // END -Perform a barrier and then leave MPI
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}

/***************************************************************************/
/* Other Functions - You write as part of the assignment********************/
/***************************************************************************/

void *run_simulation(int *thread_num_param) {
  /*int thread_num = *thread_num_param;*/
  free(thread_num_param);
  thread_num_param = NULL;
  for (int i = 0; i < number_ticks; i++) {
    // Exchange row data. MPI_Isend/Irecv from thread 0 within each MPI rank,
    // with MPI_Test/Wait

    // Note in pdf

    // Every Pthread process its row. {make it its own function?}
    // Checklist:
    //-Update universe to use correct row rng stream
    //-Factor in threshold percentages to rng values
    //-Use correct ghost row data ayrank boundaries
    //-Track number alive cells per tick across all threads in a rank group
    //^Use Pthread_mutex_trylock around shared counter variables if needed
  }

  return NULL;
}

void update_cell(int row, int col) {
  cell_t old_state = board[row][col];
  cell_t new_state;
  // determine whether we follow GOL rules or randomize the cell state
  if (GenVal(row) < threshold) {
    // set state to ALIVE or DEAD with 50-50 chance
    new_state = (int)lround(GenVal(row));
  } else {
    // count neighbors
    int neighbors = 0;
    for (int i = -1; i <= 1; ++i) {
      for (int j = -1; j <= 1; ++j) {
        if (i == 0 && j == 0)
          continue;
        if (board[row + i][col + j])
          ++neighbors;
      }
    }

    // update based on GOL rules
    if (neighbors == 2)
      new_state = ALIVE;
    else if (neighbors == 3)
      new_state = old_state;
    else
      new_state = DEAD;
  }
  board[row][col] = new_state;
}
