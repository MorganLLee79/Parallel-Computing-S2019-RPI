#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<mpi.h>

// Compile Code: mpicc -g -Wall mpi-cla-io_example.c -o mpi-cla-io_example.out
// Example Run Code: mpirun -np 4 ./mpi-cla-io_example.out test_input_1.txt test_output_1.txt
// Both input and output files will 524490 bytes in size. The two additional
//    characters are due to a newline characters between each input and at the
//    end of the file.


#define HEX_INPUT_SIZE 262144

FILE *my_input_file=NULL;
FILE *my_output_file=NULL;

// Add 1 to array size because strings must be null terminated
char hex_input_a[HEX_INPUT_SIZE+1]={0};
char hex_input_b[HEX_INPUT_SIZE+1]={0};

int *testInput = NULL;

int receiver[3];

int main(int argc, char** argv) {

  int i;
  testInput = (int *) malloc(9 * sizeof(int));
  //receiver = (int *) malloc(3 * sizeof(int));

  int my_mpi_size = -1;
  int my_mpi_rank = -1;
  MPI_Init( &argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &my_mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_mpi_rank);

  if( argc != 3 )
    {
      printf("Not sufficient arguments, only %d found \n", argc);
      exit(-1);
    }

  if( 0 == my_mpi_rank ) // compare 0 first ensures == operator must be used and not just =
    {
      printf("MPI Rank %d: Attempt to Read File Data \n", my_mpi_rank );

      if( (my_input_file = fopen( argv[1], "r")) == NULL )
	{
	  printf("Failed to open input data file: %s \n", argv[1]);
	}
      if( (my_output_file = fopen( argv[2], "w")) == NULL )
	{
	  printf("Failed to open input data file: %s \n", argv[2]);
	}

      fscanf( my_input_file, "%s %s", hex_input_a, hex_input_b );
      fprintf( my_output_file, "%s\n%s\n", hex_input_a, hex_input_b );
      
      printf("MPI Rank %d: Finished Reading and Write File Data \n", my_mpi_rank );

      fclose( my_input_file );
      fclose( my_output_file );

      //Fill input with test data
      for(i=0; i<9;i++){
      	testInput[i] = 2*i;
      }
    }
  
  MPI_Barrier(MPI_COMM_WORLD);
  printf("Rank %d/%d: Hello World. init;\n", my_mpi_rank, my_mpi_size);

  MPI_Scatter(testInput, (9/4), MPI_INT, &receiver, (9/4), MPI_INT, 0, MPI_COMM_WORLD);

  printf("Rank %d/%d: Hello World. Test = %d, %d \n", my_mpi_rank, 
  	my_mpi_size, //input[(9/4)*my_mpi_rank], input[(9/4*my_mpi_rank)+1]);
  	receiver[0], receiver[1]);

  MPI_Barrier(MPI_COMM_WORLD);

  //Testing isend, irecv
  MPI_Request request1, request2;
  MPI_Status testStatus;

  int testRecv = -1;
  int testSend = 3;
  int changeTest = 0;
  if(my_mpi_rank == 1) {

  	MPI_Irecv(&testRecv, 1, MPI_INT, 0, 23, MPI_COMM_WORLD, &request1);

  	//printf("Rank %d/%d: Receive test: %d\n", my_mpi_rank, my_mpi_size, testRecv);

  	receiver[0] = 10;
  	receiver[1] = 30;

  	MPI_Wait(&request1, &testStatus);
  } else if(my_mpi_rank == 0) {
  	testSend = 5;

  	MPI_Isend(&testSend, 1, MPI_INT, 1, 23, MPI_COMM_WORLD, &request2);
  }

  if(my_mpi_rank == 0) {

  	MPI_Irecv(&testRecv, 1, MPI_INT, 1, 23, MPI_COMM_WORLD, &request1);

  	//printf("Rank %d/%d: Receive test: %d\n", my_mpi_rank, my_mpi_size, testRecv);
  	MPI_Wait(&request1, &testStatus);
  } else if(my_mpi_rank == 1) {
  	testSend = 8;

  	MPI_Isend(&testSend, 1, MPI_INT, 0, 23, MPI_COMM_WORLD, &request2);
  }

  //Test using the sent stuff
  changeTest += testRecv;

  //MPI_Wait(&request1, &testStatus);
  //MPI_Wait(&request2, &testStatus);
  MPI_Barrier(MPI_COMM_WORLD);
  printf("Rank %d/%d: Receive test: %d; changeTest=%d\n", my_mpi_rank, my_mpi_size,
  	testRecv, changeTest);

  //Testing mpi_gather
  int gather[9];

  MPI_Gather(&receiver, 2, MPI_INT, &gather, 2, MPI_INT, 0, MPI_COMM_WORLD);

  if(my_mpi_rank == 0) {
  	for(i=0;i<8;i++) {
  		receiver[i] += i;
  		printf("Rank %d/%d: Gather test: i=%d; gather[i]=%d\n", my_mpi_rank, my_mpi_size,
  			i, gather[i]);
  		printf("Rank %d/%d: receiver changed to: i=%d; receiver[i]=%d\n", my_mpi_rank, my_mpi_size,
  			i, receiver[i]);
  	}
  } else {
	printf("Rank %d/%d: receiver: %d,%d\n", my_mpi_rank, my_mpi_size,
		receiver[0], receiver[1]);
  }

  MPI_Finalize();
}
