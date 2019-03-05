#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>


//Assignment 3
//Expected output: 576,460,751,766,552,576; N*N- 1/2


#define input_size 64//1073741824 // 1,073,741,824; 2^30

//Values will be deterministic;
//Example: bigarray[0] = 0 while bigarray[999999999%elementsperrank] = 999999999.
MPI_LONG_LONG inputData[input_size];

//WATCH MEM USAGE! inputData uses half the available per rank space!
MPI_LONG_LONG localSum;
MPI_LONG_LONG recvVal;

//Starting and ending indices for this rank to cover
int start;
int end;

//Track my mpi rank and total mpi size
int mpiRank;
int mpiSize;



//
MPI_LONG_LONG sum(int start, int end, MPI_LONG_LONG* input){
	MPI_SUM
}


//The final intended function
void MPI_P2P_Reduce(void* send_data, void* recv_data, int count, MPI_Datatype datatype,
    MPI_Op op, int root, MPI_Comm communicator) {

	if(op != MPI_SUM) {
		printf("  WARNING r%d: Unknown operation in MPI_P2P_Reduce!\n", mpiRank);
		return;
	}
	if(root != 0) {
		printf("  WARNING r%d: Unexpected root value. Changing to 0.\n", mpiRank);
		root = 0;
	}

	int i;
	//Allocate the parts of send_data to the corresponding ranks
	int chunkSize = input_size / mpiSize;
	start = mpiRank * chunkSize;
	end = (mpiRank+1) * chunkSize;

	//Sum this rank's part
	localSum = 0;
	for(i=start;i<end;i++){
		//Need to split?
		localSum += inputData[i];
	}

	//Share/ send/recv to the other ranks accordingly
	int currStepSize;
	
	//Loop until one rank left; 0
	//Increase step sizes; // 2: 0,1;2,3 -> 4: 0,2;4,6 -> 8: 0,4;8,12
	for(currStepSize = 2;currStepSize<mpiSize;currStepSize = currStepSize * 2){
		//Pairs are separated by currStepSize/2
		int halfStep = currStepSize/2;

		MPI_Request recvRequest, sendRequest;
  		MPI_Status mpiStatus;

		//Receiving ranks; 0;2 -> 0;4 -> -;8
		if(mpiRank % currStepSize == 0 && mpiRank % (currStepSize*2) == 0) {
			MPI_Irecv(&recvVal, 1, MPI_LONG_LONG, mpiRank+halfStep, 0, MPI_COMM_WORLD, &recvRequest);
			MPI_Wait(&recvRequest, &mpiStatus);  //Essentially makes recv blocking?
		}

		//Sending ranks; 1;3 -> 2;6 -> 4->12
		if(mpiRank % currStepSize == 0 && mpiRank % (currStepSize*2) == 1) {
			MPI_Isend(localSum, 1, MPI_LONG_LONG, my_mpi_rank-halfStep, 0, MPI_COMM_WORLD, &sendRequest);
		}

		//All others are ignorable ranks; think rank 1 past iteration 4


		//Take in the recvVal, combine with local sum
		localSum


		//Wait for all ranks to finish send/receiving
		MPI_Barrier(MPI_COMM_WORLD);

		//Repeat for next set of ranks/step sizes

		/* sequential version
		//Send/recv across all step pairs
		for(i = 0; i<mpiSize; i += currStepSize * 2) {

			//first
			if()
			MPI_Irecv(&received, 1, MPI_INT, my_mpi_rank-1, 0, MPI_COMM_WORLD, &recvRequest);
			if()
			second:
			mpi_send
		} */
	}
}

//Begin program run here
// Compile Code: mpicc -g -Wall leeh17_hw3.c -o leeh17_hw3.out
// Example Run Code: mpirun -np 4 ./leeh17_hw3.out test_input_1.txt test_output_1.txt
//Prints output to standard output
//Most of this will be a wrapping testing thing for MPI_P2P_Reduce to run it
int main(int argc, char** argv){


	//Initialize inputData array; try to recycle for all!
	int i;
	for(i=0; i<input_size;i++) {
		inputData[i] = i;
	}


	//Test mpi_sum
	print("mpi_sum = %d\n", MPI_SUM);

	MPI_P2P_Reduce(inputData, receiveData?, input_size?, MPI_LONG_LONG,
		MPI_SUM, 0, MPI_COMM_WORLD);
}