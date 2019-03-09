#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>


//Assignment 3
//Expected output: 576,460,751,766,552,576; N*N- 1/2


//Timer stuff
//Define depending on if on mastiff or BG/Q
#define onBGQ 1

#ifdef onBGQ
#include<hwi/include/bqc/A2_inlines.h>
#else
#define GetTimeBase MPI_Wtime
#endif

//Timer vars
double timeSeconds = 0;
double processorFreq = 1600000000.0;
unsigned long long start_cycles = 0;
unsigned long long end_cycles = 0;


#define input_size 1073741824 // 1,073,741,824; 2^30; final answer = 576,460,751,766,552,576

//Values will be deterministic;
//Example: bigarray[0] = 0 while bigarray[999999999%elementsperrank] = 999999999.
long long *inputData;

//Track my mpi rank and total mpi size
int mpiRank;
int mpiSize;

//Starting and ending indices for this rank to cover
int start;
int end;


//Begin program run here
// Compile Code: mpicc -g -Wall leeh17_hw3.c -o leeh17_hw3.out
// Example Run Code: mpirun -np 4 ./leeh17_hw3.out
//Prints output to standard output
//Most of this will be a wrapping testing thing for MPI_P2P_Reduce to run it
int main(int argc, char** argv){
	//Initialize mpi
	mpiSize = -1;
	mpiRank = -1;

	MPI_Init( &argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);

	//if(mpiRank == 0) {printf("MPI Initialized\n");}

	//Intialize values
	//localSum = malloc()

	//Initialize inputData array; try to recycle for all!
	int i;
	//Allocate the parts of send_data to the corresponding ranks
	int chunkSize = input_size / mpiSize;
	start = mpiRank * chunkSize;
	end = (mpiRank+1) * chunkSize;
	//printf("r%d: start=%d; end=%d\n", mpiRank, start, end);

	//Allocate inputData
	inputData = (long long *) malloc( (chunkSize+1) * sizeof(long long));
	long long offset = chunkSize * mpiRank;
	for(i=0; i<chunkSize;i++) {
		inputData[i] = offset + i;
	}
	//printf("rank %d finished initalizing\n", mpiRank);

	MPI_Barrier(MPI_COMM_WORLD);

	//Start timer
	start_cycles = GetTimeBase();


	//MPI_P2P_Reduce(inputData, &finalSum, input_size, MPI_LONG_LONG,
	//	MPI_SUM, 0, MPI_COMM_WORLD);
	//Initialize variables
	long long localSum;

	//Sum this rank's part
	localSum = 0;
	for(i=0;i<input_size/mpiSize;i++){
		//Need to split?
		localSum += inputData[i];
	}

	//Make sure eveyone is here
	//printf("r%d: Finished local sum, got total of %lld\n", mpiRank, localSum);
	MPI_Barrier(MPI_COMM_WORLD);

	long long finalSum;
	MPI_Reduce(&localSum, &finalSum, 1, MPI_LONG_LONG,
		MPI_SUM, 0, MPI_COMM_WORLD);

	//End timer
	end_cycles = GetTimeBase();
	timeSeconds = ((double) (end_cycles - start_cycles)) / processorFreq;


	//Print output
	if(mpiRank == 0) {
		printf("r%d: %lld=final value\n", mpiRank, finalSum);
		printf("Runtime = %f", timeSeconds);
	}

	MPI_Finalize();
}
















