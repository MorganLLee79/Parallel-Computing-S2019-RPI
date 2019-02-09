#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<mpi.h>

// Compile Code: mpicc -g -Wall mpi-cla-io.c -o mpi-cla-io
// Example Run Code: mpirun -np 4 ./mpi-cla-io test_input_1.txt test_output_1.txt
// Both input and output files will 524490 bytes in size. The two additional
//    characters are due to a newline characters between each input and at the
//    end of the file.


#define HEX_INPUT_SIZE 262144

FILE *my_input_file=NULL;
FILE *my_output_file=NULL;

// Add 1 to array size because strings must be null terminated
char hex_input_a[HEX_INPUT_SIZE+1]={0};
char hex_input_b[HEX_INPUT_SIZE+1]={0};

int main(int argc, char** argv)
{
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
    }
  
  MPI_Barrier(MPI_COMM_WORLD);

  printf("Rank %d: Hello World \n", my_mpi_rank);

  MPI_Finalize();
}
