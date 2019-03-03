#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>


#define input_size 576460751766552576 //576,460,751,766,552,576; n*n-1/2

//Values will be deterministic;
//Example: bigarray[0] = 0 while bigarray[999999999%elementsperrank] = 999999999.
int data


//Begin program run here
// Compile Code: mpicc -g -Wall leeh17_hw3.c -o leeh17_hw3.out
// Example Run Code: mpirun -np 4 ./leeh17_hw3.out test_input_1.txt test_output_1.txt
//Creates output file according to argv[2]
int main(int argc, char** argv){

}