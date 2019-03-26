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

#define ROW_LENGTH 32768  // 32,768

/***************************************************************************/
/* Global Vars *************************************************************/
/***************************************************************************/

double g_time_in_secs = 0;
double g_processor_frequency = 1600000000.0;  // processing speed for BG/Q
unsigned long long g_start_cycles = 0;
unsigned long long g_end_cycles = 0;

// You define these
double THRESHOLD = 0.0;

// TODO: temporary, probably replace with a function to handle ghost rows
int board[32768][32768];

// Per-experiment values
int thread_per_node;
int threshold;
int number_ticks;

int rows_per_rank;  // Use for getting RNG stream indices; [local_row +
                    // (rows_per_rank * mpiRank)]

int ghostRow[] = {0};

/***************************************************************************/
/* Function Decs ***********************************************************/
/***************************************************************************/

// You define these
/**
 * Updates a single cell at the specified position according to the rules.
 */
void update_cell(int row, int col);

/***************************************************************************/
/* Function: Main **********************************************************/
/***************************************************************************/

int main(int argc, char *argv[]) {
  //    int i = 0;
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

  // MPI already set up.

  // Start recording time base
  if (mpiRank == 0) {
    g_start_cycles = GetTimeBase();
  }

  // Allocate universe chunks and "ghost" rows

  // Initialize universe, all cells alive

  // Create Pthreads, go into for-loop
  int i;
  for (i = 0; i < number_ticks; i++) {
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
  // Simulation finished

  // MPI_reduce sums of alive cells per tick; will be a vector (array) for all
  // ticks

  // Stop timer
  if (mpiRank == 0) {
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

  if (mpiRank == 0) {
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

void update_cell(int row, int col) {
  int old_state = board[row][col];
  int new_state;
  // determine whether we follow GOL rules or randomize the cell state
  if (GenVal(row) < THRESHOLD) {
    new_state = (int)lround(GenVal(row));
  } else {
    int neighbors = 0;
    for (int i = -1; i <= 1; ++i) {
      for (int j = -1; j <= 1; ++j) {
        if (i == 0 && j == 0)
          continue;
        if (board[row + i][col + j])
          ++neighbors;
      }
    }

    if (neighbors == 2)
      new_state = ALIVE;
    else if (neighbors == 3)
      new_state = old_state;
    else
      new_state = DEAD;
  }
  board[row][col] = new_state;
}
