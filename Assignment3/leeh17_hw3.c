#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>


//Assignment 3
//Expected output: 576,460,751,766,552,576; N*N- 1/2


//Timer stuff
//Define depending on if on mastiff or BG/Q
#define onBGQ 0

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


#define input_size 1048576//1073741824 // 1,073,741,824; 2^30; final answer = 576,460,751,766,552,576

//Values will be deterministic;
//Example: bigarray[0] = 0 while bigarray[999999999%elementsperrank] = 999999999.
long long *inputData;

//Track my mpi rank and total mpi size
int mpiRank;
int mpiSize;

//Starting and ending indices for this rank to cover
int start;
int end;


//The final intended function
//How is count used? TODO
void MPI_P2P_Reduce(void* send_data, void* recv_data, int count, MPI_Datatype datatype,
    MPI_Op op, int root, MPI_Comm communicator) {

	if(op != MPI_SUM) {
		printf("  WARNING r%d: Unknown operation in MPI_P2P_Reduce!\n", mpiRank); return; }
	if(datatype != MPI_LONG_LONG) {
		printf("  WARNING r%d: Unknown datatype in MPI_P2P_Reduce!\n", mpiRank); return; }

	if(root != 0) {
		printf("  WARNING r%d: Unexpected root value. Changing to 0.\n", mpiRank);
		root = 0;
	}

	//Initialize variables
	long long localSum;
	long long recvVal;

	int i;

	//Sum this rank's part
	localSum = 0;
	for(i=0;i<input_size/mpiSize;i++){
		//Need to split?
		localSum += inputData[i];
	}

	//Make sure eveyone is here
	//printf("r%d: Finished local sum, got total of %lld\n", mpiRank, localSum);
	MPI_Barrier(MPI_COMM_WORLD);

	//Share/ send/recv to the other ranks accordingly
	int currStepSize;
	
	//Loop until one rank left; 0
	//Increase step sizes; // 2: 0,1;2,3 -> 4: 0,2;4,6 -> 8: 0,4;8,12
	for(currStepSize = 2;currStepSize<=mpiSize; currStepSize = currStepSize * 2){
		//Pairs are separated by currStepSize/2
		int halfStep = currStepSize/2;

		//if(mpiRank == 0) { printf("---Entering step %d; halfStep=%d----\n", currStepSize, halfStep);}

		MPI_Request recvRequest, sendRequest;
  		MPI_Status mpiStatus;

		//Receiving ranks; 0;2 -> 0;4 -> -;8
		if(mpiRank % halfStep == 0 && mpiRank % currStepSize == 0) {
			//printf("r%d: step %d; receiving from %d\n", mpiRank, currStepSize, mpiRank+halfStep);
			
			MPI_Irecv(&recvVal, 1, MPI_LONG_LONG, mpiRank+halfStep, 0, MPI_COMM_WORLD, &recvRequest);
			MPI_Wait(&recvRequest, &mpiStatus);  //Essentially makes recv blocking?

			//Take in the recvVal, combine with local sum
			localSum += recvVal;
			//printf("r%d: localSum=%lld, recvVal=%lld\n",mpiRank, localSum, recvVal);
		}

		//Sending ranks; 1;3 -> 2;6 -> 4->12
		else if(mpiRank % halfStep == 0 && mpiRank % currStepSize == halfStep) {
			//printf("r%d: step %d; sending to %d value %lld\n",
			//	mpiRank, currStepSize, mpiRank-halfStep, localSum);

			MPI_Isend(&localSum, 1, MPI_LONG_LONG, mpiRank-halfStep, 0, MPI_COMM_WORLD, &sendRequest);
		}
		else {
			//All others are ignorable ranks; think rank 1 past iteration 4
			//printf("r%d: step %d; idling\n", mpiRank, currStepSize);
		}

		//Wait for all ranks to finish send/receiving
		MPI_Barrier(MPI_COMM_WORLD);


		//Repeat for next set of ranks/step sizes

		/* sequential version
		//Send/recv across all step pairs
		for(i = 0; i<mpiSize; i += currStepSize * 2) {

			//first
			if()
			MPI_Irecv(&received, 1, MPI_LONG_LONG, my_mpi_rank-1, 0, MPI_COMM_WORLD, &recvRequest);
			if()
			second:
			mpi_send
		} */
	}

	MPI_Barrier(MPI_COMM_WORLD);

	//Return
	if(mpiRank == 0) {
		//printf("Final value = %lld? TODO figure out how to return via recv_data\n", localSum);
		printf("%lld\n",localSum);
		recv_data += localSum;
	}
}

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


	int finalSum = 0;
	
	//Start timer
	start_cycles = GetTimeBase();


	MPI_P2P_Reduce(inputData, &finalSum, input_size, MPI_LONG_LONG,
		MPI_SUM, 0, MPI_COMM_WORLD);


	//End timer
	end_cycles = GetTimeBase();
	timeSeconds = ((double) (end_cycles - start_cycles)) / processorFreq;

	//Print output
	/*if(mpiRank == 0) {
		//printf("$lld\n", mpiRank, finalSum);
		//printf("Runtime = %f", timeSeconds);
	}*/

	MPI_Barrier(MPI_COMM_WORLD);

	//Begin testing with MPI_Reduce

	//Initialize variables
	long long localSum = 0;

	//Sum this rank's part
	for(i=0;i<input_size/mpiSize;i++){
		//Need to split?
		localSum += inputData[i];
	}

	//Make sure eveyone is here
	//printf("r%d: Finished local sum, got total of %lld\n", mpiRank, localSum);
	MPI_Barrier(MPI_COMM_WORLD);

	long long finalReduceSum;
	MPI_Reduce(&localSum, &finalReduceSum, 1, MPI_LONG_LONG,
		MPI_SUM, 0, MPI_COMM_WORLD);

	//Print output
	if(mpiRank == 0) {
		//printf("r%d: %d=final value\n", mpiRank, finalReduceSum);
		printf("%lld\n", finalReduceSum);
		//printf("Runtime = %f", timeSeconds);
	}

	MPI_Finalize();
}
















