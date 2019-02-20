#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>


/***** Data Structures (Provided) *********/
//Transfer with scp file.c username@mastiff.edu:hw2

//----From mpi_cla_io
// Compile Code: mpicc -g -Wall leeh17_hw2_v1.c -o leeh17_hw2.out
// Example Run Code: mpirun -np 32 ./leeh17_hw2.out tests/test_input_2.txt tests/output2.txt
// Both input and output files will 524490 bytes in size. The two additional
//    characters are due to a newline characters between each input and at the
//    end of the file.


#define testing_RunTime 1
#define testing_Barriers 1

#define HEX_INPUT_SIZE 262144

FILE *my_input_file=NULL;
FILE *my_output_file=NULL;

//----From mpi_cla_io
// EXAMPLE DATA STRUCTURE DESIGN AND LAYOUT FOR CLA
#define input_size 262144    //New input size, 1 million bits
#define block_size 32         //New block size, 32 bit blocks and 32 MPI ranks

//Do not touch these defines
#define digits 262144                 //   262,145
#define bits digits * 4                       //i, 1,048,580
#define ngroups bits/block_size               //j, 32,768 +1?
#define nsections ngroups/block_size          //k, 1024
#define nsupersections nsections/block_size   //l, 32

//Global definitions of the various arrays used in steps for easy access
int gi[bits] = {0};
int pi[bits] = {0};
int ci[bits] = {0};

int ggj[ngroups] = {0};
int gpj[ngroups] = {0};
int gcj[ngroups] = {0};

int sgk[nsections] = {0};
int spk[nsections] = {0};
int sck[nsections] = {0};

int ssgl[nsupersections] = {0} ;
int sspl[nsupersections] = {0} ;
int sscl[nsupersections] = {0} ;

int sumi[bits] = {0};

//Integer array of inputs in binary form
//int* bin1 = NULL;
//int* bin2 = NULL;
int bin1[bits];
int bin2[bits];

int* inputBin1;
int* inputBin2;

int my_mpi_size;
int my_mpi_rank;

int received;

//int startIndex;
//int endIndex;

//Character array of inputs in hex form
char* hex1 = NULL;
char* hex2 = NULL;


/********** I/O and Setup **********/

//Convert the given hex string into a usable number.
//Input:  A hexadecimal string representing an integer
//Return (through param pointer): Binary array form of the given hex string through pointer input
void convertToNumber(char *inputString, int* result) {

  int i;  //Track hex index
  int j;  //Track binary index

  j = bits-1;
  //printf("Converting to number %s\n", inputString + digits-30);
  
  //Iterate through the string
  for(i = 0; i < digits; i++) {

    //Read the current digit, converting to a binary value
    // Does this with simple if cases for each potential expected char.a
    if(inputString[i] == 'F' ){  // F = 1111
      result[j] = 1;  result[j-1] = 1;  result[j-2] = 1;  result[j-3] = 1;

    } else if(inputString[i] == 'E') { // E = 1110
      result[j] = 1;  result[j-1] = 1;  result[j-2] = 1;  result[j-3] = 0;

    } else if(inputString[i] == 'D') { // D = 1100
      result[j] = 1;  result[j-1] = 1;  result[j-2] = 0;  result[j-3] = 1;

    } else if(inputString[i] == 'C') { // C = 0000
      result[j] = 1;  result[j-1] = 1;  result[j-2] = 0;  result[j-3] = 0;

    } else if(inputString[i] == 'B') { // B = 0000
      result[j] = 1;  result[j-1] = 0;  result[j-2] = 1;  result[j-3] = 1;

    } else if(inputString[i] == 'A') { // A = 0000
      result[j] = 1;  result[j-1] = 0;  result[j-2] = 1;  result[j-3] = 0;

    } else if(inputString[i] == '9') { // 0 = 0000
      result[j] = 1;  result[j-1] = 0;  result[j-2] = 0;  result[j-3] = 1;

    } else if(inputString[i] == '8') { // 0 = 0000
      result[j] = 1;  result[j-1] = 0;  result[j-2] = 0;  result[j-3] = 0;

    } else if(inputString[i] == '7') { // 0 = 0000
      result[j] = 0;  result[j-1] = 1;  result[j-2] = 1;  result[j-3] = 1;

    } else if(inputString[i] == '6') { // 0 = 0000
      result[j] = 0;  result[j-1] = 1;  result[j-2] = 1;  result[j-3] = 0;

    } else if(inputString[i] == '5') { // 0 = 0000
      result[j] = 0;  result[j-1] = 1;  result[j-2] = 0;  result[j-3] = 1;

    } else if(inputString[i] == '4') { // 0 = 0000
      result[j] = 0;  result[j-1] = 1;  result[j-2] = 0;  result[j-3] = 0;

    } else if(inputString[i] == '3') { // 0 = 0000
      result[j] = 0;  result[j-1] = 0;  result[j-2] = 1;  result[j-3] = 1;

    } else if(inputString[i] == '2') { // 0 = 0000
      result[j] = 0;  result[j-1] = 0;  result[j-2] = 1;  result[j-3] = 0;

    } else if(inputString[i] == '1') { // 0 = 0000
      result[j] = 0;  result[j-1] = 0;  result[j-2] = 0;  result[j-3] = 1;

    } else if(inputString[i] == '0') { // 0 = 0000
      result[j]   = 0;  result[j-1] = 0;  result[j-2] = 0;  result[j-3] = 0;

    } else {
      printf("ERROR: Unrecognized hex: \'%c\' at index %d.\n", inputString[i], i);
    }

    if((/*i % 128 == 0 ||*/ i < 4) && 0) { //Only part of the hex. && true for debuging/disabling
      printf("Converted %c to %d%d%d%d.\n", inputString[i], 
          result[j], result[j-1], result[j-2], result[j-3]);
    }

    j = j - 4; //Iterate down; 4095 is most significant, 0 is least

  }

  //printf("Finished converting to number, final i:%d final j: %d\n", i, j);

  //return result;
}


// Begin reading in input files. Parses them as well
//Input:  The file path
void readInput(char *inputFilePath) {

  printf("READINPUT RAN");

  //bin1 = (int *) malloc( (bits) * sizeof(int));
  //bin2 = (int *) malloc( (bits) * sizeof(int));

  hex1 = (char *) malloc( (digits+1) * sizeof(char)); //+1 to account for \0
  hex2 = (char *) malloc( (digits+1) * sizeof(char));

  //Read from file into hex1 and hex2
  scanf("%s %s", hex1, hex2);
  //hex1[0] = '0';        //Add leading 0
  hex1[digits] = '\0';  //Add null terminator

  //hex2[0] = '0';
  hex2[digits] = '\0';

  //printf("TEST: %ld\n%s\n", strlen(hex2), hex2);
  //printf("Read numbers:\n%s\n%s\n", hex1+digits-10, hex2+digits-10);
  
  //Convert the two hexadecimal strings into usable numbers, saving to globals bin1/2
  convertToNumber(hex1, bin1);
  convertToNumber(hex2, bin2);

  //printf("bin1: %d%d%d%d \nbin2: %d%d%d%d\n", bin1[4095], bin1[4094], bin1[4093], bin1[4092],
  //  bin2[4095], bin2[4094], bin2[4093], bin2[4092]);
  
}


char* convertToHexString(int* inputBinary){
  int i;
  int currIndex;  //Current sum index

  int a;  //To represent some of the 4 bits' values
  int b;
  int c;
  int d;
  int byteTotal;
  char temp;

  int warningCount = 0;

  //char* hexSum[digits + 1];
  char* hexSum = (char *) malloc( (digits + 1) * sizeof(char));
  
  //Iterate up from the expected 
  for(i=0; i < digits; i++) {

    currIndex = bits - (4*i) - 1;

    a = inputBinary[currIndex - 0];  //Most significant index
    b = inputBinary[currIndex - 1];
    c = inputBinary[currIndex - 2];
    d = inputBinary[currIndex - 3];  //Least significant index

    byteTotal = (8 * a) + (4 * b) + (2 * c) + (1 * d);

    //if(i > digits - 20) {
    //  printf("%d%d%d%d, at index %d, byte total: %d\n", a, b, c, d, currIndex, byteTotal); }

    if(byteTotal == 15)        {  temp = 'F';
    } else if(byteTotal == 14) {  temp = 'E';
    } else if(byteTotal == 13) {  temp = 'D';
    } else if(byteTotal == 12) {  temp = 'C';
    } else if(byteTotal == 11) {  temp = 'B';
    } else if(byteTotal == 10) {  temp = 'A';
    } else if(byteTotal == 9)  {  temp = '9';
    } else if(byteTotal == 8)  {  temp = '8';
    } else if(byteTotal == 7)  {  temp = '7';
    } else if(byteTotal == 6)  {  temp = '6';
    } else if(byteTotal == 5)  {  temp = '5';
    } else if(byteTotal == 4)  {  temp = '4';
    } else if(byteTotal == 3)  {  temp = '3';
    } else if(byteTotal == 2)  {  temp = '2';
    } else if(byteTotal == 1)  {  temp = '1';
    } else if(byteTotal == 0)  {  temp = '0';
    } else {
      temp = '?';

      if(warningCount < 4) {
        printf("WARNING: Found incorrect byteTotal \'%d\' in convertToHexString().\n", byteTotal);
        printf("  Offending \'byte\': %d%d%d%d\n", a, b, c, d);

        if(warningCount > 3){
          printf("Too many warnings, suppressing future warnings.\n");
        }
      }

      warningCount++;
    }

    //Write the new char into our result string
    hexSum[i] = temp;

  }

  if(warningCount > 3){
    printf("WARNING: %d total warnings.\n", warningCount);
  }
/*
  printf("Ended at i:%d, bin index:%d; started at bin index:%d\n", i, currIndex-3, bits - 1);
  printf("%d%d%d%d-printoutput test\n", inputBinary[bits-3], inputBinary[bits-4],
    inputBinary[bits-5], inputBinary[bits-6]); */

  hexSum[i] = '\0'; //End the string.

  return hexSum;

}


// Takes in the number to be printed out
//  The given file will have the number in hexadecimal format
// Prints out to stdout.
void printOutput(int* finalSum, FILE* my_output_file) {
  //int i;
  char *hexString;

  //Convert number to usable/printable string
  hexString = convertToHexString(finalSum);

  /*
  //printf("Sum:\n");
  //for(i=0;i<bits;i++){  printf("%d", sumi[i]);}
  //printf("\n"); */

  //Print

  //hexString[80] = '\0';
  //printf("Result: %s\n", hexString);
  fprintf(my_output_file, "%s\n", hexString);

  free(hexString);
}


/************** Program ******************/

//Calculate g_i and p_i for all 4096 bits i
void step1() {

  //gi = (int *) malloc( (bits) * sizeof(int));
  //pi = (int *) malloc( (bits) * sizeof(int));

  int i;

  //printf("TEST XOR: %d, %d, %d, %d\n", 0^0, 1^0,0^1,1^1);

  for(i = 0; i < bits/my_mpi_size; i++){
    gi[i] = bin1[i] && bin2[i]; //g_i = a_i and b_i
    pi[i] = bin1[i] || bin2[i];
  }

  //printf("%d-%d-%d-%d--%d-\n", gi[0], pi[0], bin1[0], bin2[0], gi[bits-1]);
  //printf("step 1: %d, %d\n", gi[i-1], pi[i-1]);
  
  /*
  for(i=0; i<64;i++) { printf("%d", bin1[i]); }
  printf("\n");
  for(i=0; i<64;i++) { printf("%d", bin2[i]); }
  printf("\n");
  for(i=0; i<64;i++) { printf("%d", gi[i]); }
  printf("-\n");*/
}


//Calculate gg_j and gp_j for all 512 groups j using g_i and p_i
void step2() {

  int tempPropagates[block_size] = {0};
  int tempGG;
  int tempGP;

  int x;
  int j;
  int i = 0;

  for(j=0; j < ngroups/my_mpi_size; j++) {

    tempGG = 0;
    tempPropagates[block_size-1] = 1; //Default to true in order to ignore in ands
    tempGP = pi[i];
    for(x = block_size-2; x >= 0; x = x - 1) { //Iterate through the block backwards
      tempPropagates[x] = tempPropagates[x+1] && pi[i+x+1];

      //if(j < 2) {printf("%d: %d\n", x, tempPropagates[x]);}
    }

    for(x = 0; x < block_size; x++) {
      tempGG = tempGG || (tempPropagates[x] && gi[i+x]);
      tempGP = tempGP && pi[i+x];
    }

    ggj[j] = tempGG;
    gpj[j] = tempGP;

    //Iterate/manage i as well. i represents the base index of this block
    i = i + block_size;
    if(i > bits/my_mpi_size) { printf("WARNING: In step2, i surpassed bits! It is now: %d.\n", i); }
  }

  /*if(my_mpi_rank == 1) {
    for(j=0;j<20 && j<ngroups;j++) { printf("%d", ggj[j]); }
    printf("-step2 g\n"); 
    for(j=0;j<20 && j<ngroups;j++) { printf("%d", gpj[j]); }
    printf("-step2 p\n");
  }*/
}

//Calculate sg_k and sp_k for all 64 sections k using ggj and gpj (larger subsections)
void step3() {

  //sgk = (int *) malloc( (bits) * sizeof(int));
  //spk = (int *) malloc( (bits) * sizeof(int));

  int tempPropagates[block_size] = {0};
  int tempSG;
  int tempSP;

  int x;
  int k;
  int j = 0;

  for(k=0; k < nsections/my_mpi_size; k++) {

    tempSG = 0;
    tempPropagates[block_size-1] = 1; //Default to true in order to ignore in ands
    tempSP = gpj[j];
    for(x = block_size-2; x >= 0; x = x - 1) { //Iterate through the block backwards
      tempPropagates[x] = tempPropagates[x+1] && gpj[j+x+1];
    }

    //TODO save array of propagate sets, and in?
    for(x = 0; x < block_size; x++) {
      //Temps represent the sum of all earlier values
      tempSG = tempSG || (tempPropagates[x] && ggj[j+x]);
      tempSP = tempSP && gpj[j+x];
    }

    sgk[k] = tempSG;
    spk[k] = tempSP;

    //Iterate/manage j as well.
    j = j + block_size;
    if(j > ngroups/my_mpi_size) {
      printf("WARNING: In step2, j surpassed ngroups! It is now: %d.\n", j);
    }

  }

  /*if(my_mpi_rank == 1) {
    for(j=0;j<20 && j<nsections;j++) { printf("%d", sgk[j]); }
    printf("-step3 g\n"); 
    for(j=0;j<20 && j<nsections;j++) { printf("%d", spk[j]); }
    printf("-step3 p\n");
  }*/
}


//Calculat ss_gl and sp_l for all 8 super sections l using sg_k and sp_k
void step4() {

  int tempPropagates[block_size] = {0};
  int tempSSG;
  int tempSSP;
  int x;

  int l;
  int k = 0;

  for(l=0; l < nsupersections/my_mpi_size; l++) {

    tempSSG = 0;
    tempPropagates[block_size-1] = 1; //Default to true in order to ignore in ands
    tempSSP = spk[k];
    for(x = block_size-2; x >= 0; x = x - 1) { //Iterate through the block backwards
      tempPropagates[x] = tempPropagates[x+1] && spk[k+x+1];
    }

    //TODO save array of propagate sets, and in?
    for(x = 0; x < block_size; x++) {
      //Temps represent the sum of all earlier values
      tempSSG = tempSSG || (tempPropagates[x] && sgk[k+x]);
      tempSSP = tempSSP && spk[k+x];
    }

    ssgl[l] = tempSSG;
    sspl[l] = tempSSP;

    //Iterate/manage k as well.
    k = k + block_size;

    if(k > nsections/my_mpi_size) {
      printf("WARNING: In step4, k surpassed nsections! It is now: %d.\n", k);
    }
  }

/*
  if(my_mpi_rank == 1) {
    for(k=0;k<20 && k<nsupersections;k++) { printf("%d", ssgl[k]); }
    printf("-step4 g\n"); 
    for(k=0;k<20 && k<nsupersections;k++) { printf("%d", sspl[k]); }
    printf("-step4 p\n");
  } */
}


//Calculate ssc_l using ssg_l and ssp_l for all l super sections and 0 for ssc_-1
// Re-factored to use MPI_Isend and MPI_Irecv
void step5() {
  //Send and receive down the ranks, 0->32

  MPI_Request recvRequest, sendRequest; //Send request will be unused
  MPI_Status mpiStatus;

  int i;

  received = -1; //The previous sscl. -1 as placeholder

  //rank 0 doesn't receive anything
  if(my_mpi_rank != 0) {
    //Tag 0 for ssgl, 1 for sspl
    MPI_Irecv(&received, 1, MPI_INT, my_mpi_rank-1, 0, MPI_COMM_WORLD, &recvRequest);
    //MPI_Irecv(&sspl[my_mpi_rank], 1, MPI_INT, my_mpi_rank-1, 1, MPI_COMM_WORLD, &req_p_recv);
    //sscl[my_mpi_rank] = ssgl[my_mpi_rank] || (sspl[my_mpi_rank] && _RECEIVED_);

    MPI_Wait(&recvRequest, &mpiStatus);  //Essentially makes recv blocking?
  } else {

    //sscl[0] = ssgl[0] || (sspl[0] && 0);
    received = 0;
    
    //int i;
    //for(i=0;i<my_mpi_size;i++) { printf("-%d%d", ssgl[i], sspl[i]); }
  }

  if(received != 0 && received != 1) {
    printf("ERROR: Rank %d: Non-valid sscl \'%d\' received in step5.\n", my_mpi_rank, received);
  }

  //Calculate all sscl[]s
  sscl[0] = ssgl[0] || (sspl[0] && received);
  //printf("Rank%d: Set %d as sscl. g=%d || (p=%d && sscl=%d)\n", my_mpi_rank,
  //  sscl[0], ssgl[0], sspl[0], received);

  for(i=1; i<nsupersections/my_mpi_size;i++) {
    sscl[i] = ssgl[i] || (sspl[i] && sscl[i-1]);

    //printf("Rank%d: Set %d as sscl. g=%d || (p=%d && sscl=%d); i=%d\n", my_mpi_rank,
    //  sscl[i], ssgl[i], sspl[i], sscl[i-1], i);
  }
  
  //printf("Rank %d: Step5, checking indices. c=%d, g=%d, p=%d, last sscl=%d.\n", my_mpi_rank,
  //  sscl[my_mpi_rank], ssgl[0], sspl[0], received);

  //rank 31 doesn't send anything
  if(my_mpi_rank != my_mpi_size-1) {
    //Send this latest sscl
    MPI_Isend(&sscl[i-1], 1, MPI_INT, my_mpi_rank+1, 0, MPI_COMM_WORLD, &sendRequest);
    //MPI_Isend(&sspl[my_mpi_rank], 1, MPI_INT, my_mpi_rank+1, 1, MPI_COMM_WORLD, &req_p_send);

    //printf("Rank%d: Sent %d as sscl to rank %d.\n", my_mpi_rank, 
    //  sscl[my_mpi_rank+i-1], my_mpi_rank+1);
  }


  
/*  int l;

  //ssc_-1 = 0
  sscl[0] = ssgl[0] || (sspl[0] && 0);

  // Not using for loop to partially simulate parallelism. Below should be effective the same
  for(l=1; l < nsupersections; l++) {

    sscl[l] = ssgl[l] || (sspl[l] && sscl[l-1]);
    //printf("step5: %d->%d; %d, %d\n", l, sscl[l], ssgl[l], sspl[l]);
  } */

  //for(l=0; l<nsupersections; l++) { printf("%d", ssgl[l]);}
  //printf("-Step 5\n");
}

void step6v2() {
  int k;
  for(k=0; k<ngroups/my_mpi_size;k++) {
    if(k == 0) {
      sck[k] = sgk[k] || (spk[k] && received);
    } else if(k%block_size == 0) {
      sck[k] = sgk[k] || (spk[k] && sscl[(k/block_size)-1]);
    } else {
      sck[k] = sgk[k] || (spk[k] && sck[k-1]);
    }
  }
}

//Calculate sc_k using sg_k and sp_k and correct ssc_l, l==k div 8 as
//  super sectional carry-in for all sections k
void step6() {
  int k;
  int x;

  for(k=0; k < nsections/my_mpi_size; k = k + block_size) {
    
    //if(k < 30) {printf("step6: %d %d, %d\n", k, sscl[k/block_size], k/block_size); }

    /*
    int carryIn;
    if(k == 0) {
      carryIn = sscl[0];
    } else {
      carryIn = sscl[(k/block_size)];
    }
    sck[k] = sgk[k] || (spk[k] && carryIn); */
    sck[k] = sgk[k] || (spk[k] && sscl[k/block_size-1]);

    //if(k<80 && my_mpi_rank == 1) {
    //printf("r%d: step6: sscl=%d, at index %d, %d; g=%d, p=%d. result=%d\n", my_mpi_rank, 
    //  sscl[k/block_size], k/block_size, k, sgk[k], spk[k], sck[k]); }

    for(x=1; x<block_size;x++){  //Each group
      sck[k+x] = sgk[k+x] || (spk[k+x] && sck[k+x-1]);


      //if(k+x<=511 && k+x > 508 && my_mpi_rank == 1) {
      //printf("r%d: step6: sck=%d, at index %d, %d; g=%d, p=%d. result=%d\n", my_mpi_rank, 
      //  sck[k+x-1], k/block_size, k+x, sgk[k+x], spk[k+x], sck[k+x]); }
    }

    /*if(k == 0) {
      sck[k] = sgk[k] || (spk[k] && sscl[0]);
      printf("r%d: ---step6, sck[0] == %d, sscl[0] == %d\n", my_mpi_rank, sck[0], sscl[0]);
    }*/
  }
  //sck[0] = 0;

  //printf("step6:_sck[56]:%d %d, %d, %d.\n", sck[56], sgk[56], spk[56], sscl[7]);
}

void step7v2() {
  int j;
  for(j=0; j<ngroups/my_mpi_size;j++) {
    if(j==0) {
      gcj[j] = ggj[j] || (gpj[j] && received);
    } else if(j%block_size == 0) {
      gcj[j] = ggj[j] || (gpj[j] && sck[(j/block_size)-1]);
    } else {
      gcj[j] = ggj[j] || (gpj[j] && gcj[j-1]);
    }
  }
}

//Calculate gc_j using gg_j, gp_j, and correct sc_k, k = j div 8 as sectional carry-in for all groups j
void step7() {
  int j;
  int x;

  
  for(j=0; j < ngroups/my_mpi_size; j = j + block_size) {
//    if(j > 430 && j < 460) {printf("step7:_%d %d, %d, %d.\n", j, ggj[j], gpj[j], sck[j/8]); }

    //if(j==16382 && my_mpi_rank == 1) {
    //printf("r%d: step7: sck=%d, at index %d, %d; g=%d, p=%d. result=%d\n", my_mpi_rank, 
    //  sck[j/block_size], j/block_size, j, ggj[j], gpj[j], gcj[j+x]); }

    /*int carryIn;
    if(j == 0) {
      carryIn = sck[0];
    } else {
      carryIn = sck[(j/block_size)];
    }
    gcj[j] = ggj[j] || (gpj[j] && carryIn);*/
    gcj[j] = ggj[j] || (gpj[j] && sck[j/block_size-1]);

    for(x=1; x<block_size;x++){  //Each group
      gcj[j+x] = ggj[j+x] || (gpj[j+x] && gcj[j+x-1]);


      //if(j+x<=16382 && j==16352 && my_mpi_rank == 1) {
      //printf("r%d: step7: gcj=%d, at index %d, %d; g=%d, p=%d. result=%d\n", my_mpi_rank, 
      //  gcj[j+x-1], j/block_size, j+x, ggj[j+x], gpj[j+x], gcj[j+x]); }
    }


    /*if(j == 0){
      gcj[j] = ggj[j] || (gpj[j] && sscl[0]);
    }*/

//      if(j > 430 && j < 460) {printf("step7: %d %d, %d, %d.\n", 
//         j+x, ggj[j+x], gpj[j+x], gcj[j+x-1]); }

  }
  //gcj[0] = 0;


  //printf("Step 7: sck:%d, %d\n", sck[0], j);
}

void step8v2() {
  int i;
  for(i=0;i<bits/my_mpi_size;i++){
    if(i==0) {
      ci[i] = gi[i] || (pi[i] && received);
    } else if(i%block_size == 0) {
      ci[i] = gi[i] || (pi[i] && gcj[(i/block_size)-1]);
    } else {
      ci[i] = gi[i] || (pi[i] && ci[i-1]);
    }
  }
}

//Calculate c_i using g_i, p_i, and correct gc_j, j = i div 8 as group carry-in for all bits i
void step8() {
  int i;
  int x;

  for(i=0; i < bits/my_mpi_size; i = i + block_size) {

//    if(i < 3550 && i > 3560) {printf("stepblock_size:_%d %d, %d\n", i, gcj[i/block_size], i+x); }
    /*int carryIn;
    if(i == 0) {
      carryIn = gcj[0];
    } else {
      carryIn = gcj[(i/block_size)];
    }
    ci[i] = gi[i] || (pi[i] && carryIn);*/
    ci[i] = gi[i] || (pi[i] && gcj[i/block_size-1]);

    //if(i<524250 && i > 524200 && my_mpi_rank == 1) {
    //printf("r%d: step8: gcj=%d, at index %d, %d; g=%d, p=%d; result = %d\n", my_mpi_rank, 
    //  gcj[i/block_size], i/block_size, i, gi[i], pi[i], ci[i]); }

    for(x=1; x<block_size;x++){  //Each group
      ci[i+x] = gi[i+x] || (pi[i+x] && ci[i+x-1]);

      //if(i<524250 && i > 524200 && x < 8&& my_mpi_rank == 1) {
      //printf("r%d: step8: c used=%d, at index %d; g=%d, p=%d; result = %d\n", my_mpi_rank, 
      //  ci[i+x-1], i+x, gi[i+x], pi[i+x], ci[i+x]); }
      //if(i < 20 && my_mpi_rank == 0) {printf("step8: %d %d, %d\n", i+x, pi[i+x], ci[i+x]); }
    }

    /*if(i == 0){
      ci[i] = gi[i] || (pi[i] && sscl[0]);
    }*/
  }
  //ci[0] = 0;

  //printf("Step 8: c=%d g=%d p=%d c-1=%d; %d\n", ci[1], gi[0], pi[0], gcj[0], i);
  /*printf("Step 8: i=%d, x=%d %d%d%d%d %d%d%d%d %d%d%d%d\n", i, x,
    gi[i-x+1], gi[i-x+2], gi[i-x+3], gi[i-x+4],
    pi[i-x+1], pi[i-x+2], pi[i-x+3], pi[i-x+4],
    ci[i-x+2], ci[i-x+3], ci[i-x+4], ci[i-x+5]); */
}

//Calculate sum_i using a_i * b_i * c_i-1 for all i where * is xor
void step9() {
  int i = 0;

  //bin1[i] = a_i
  //bin2[i] = b_i

  //sumi[i] = gi[i] ^ pi[i] ^ 0; //0 represents nothing being carried in, since first index
  if(my_mpi_rank > 0) {           //Use the received sscl value (? TODO assess again)
    sumi[i] = bin1[i] ^ bin2[i] ^ received;
    //printf("___ran using sscl[0]=%d, rank=%d. sum=%d\n", ci[0], my_mpi_rank, sumi[i]);
  } else if(my_mpi_rank == 0) {   //rank 0 just uses 0 as the carry in value
    sumi[i] = bin1[i] ^ bin2[i] ^ 0;
  } else{
    printf("ERROR: Negative rank ran in step 9? Rank:%d.\n", my_mpi_rank);
  }
  //if(i < 80 && my_mpi_rank == 0) { printf("-"); }

  //This would actually be a parallel.
  for(i = 1; i < bits/my_mpi_size; i++) {
    sumi[i] = bin1[i] ^ bin2[i] ^ ci[i-1];
    //sumi[i] = gi[i] ^ pi[i] ^ ci[i-1];

    //if(i < 80 && my_mpi_rank == 0) { printf("%d", ci[i]); }

    //  printf("%d: %d %d %d, i=%d\n", sumi[i], bin1[i], bin2[i], ci[i-1], i); }
      //ci[3584] is failiing?
  }

  //Now gather the parts from each rank
  /*if(my_mpi_rank == 1) {
    //printf("-");
    //printf("rank %d: -sum=%d, c=%d, g=%d, p=%d, b1=%d, b2=%d.i=%d\n", my_mpi_rank,
    //  sumi[0], sscl[0], gi[0], pi[0], bin1[0], bin2[0], (bits/my_mpi_size));
    //for(i=bits/my_mpi_size-68+1;i<bits/my_mpi_size-58;i++) { //bits/my_mpi_size;i+=1024){

    for(i=bits/2;i<bits/2+8;i++){
      //printf("%d", sumi[i]);
      //printf(",%d%d", ci[i-1], ci[i]);
      /printf("rank %d: sum=%d, c=%d, g=%d, p=%d, b1=%d, b2=%d.i=%d\n", my_mpi_rank,
        sumi[i], ci[i-1], gi[i], pi[i], bin1[i], bin2[i], i+(bits/my_mpi_size));
    }
    //printf("Step8\nc[0] = %d, g=%d, p=%d, gcj=%d\n", ci[0], gi[0], pi[0], gcj[0]);
    printf("transferring/using carries correctly Check p?\n");
  } else if(my_mpi_rank == 0) {
    //Print last 8 bits
    for(i=bits/my_mpi_size-8;i<bits/my_mpi_size;i++) { //bits/my_mpi_size;i+=1024){
      //printf(",%d%d", ci[i-1], ci[i]);
      printf("rank 0: sum=%d, c=%d, g=%d, p=%d, b1=%d, b2=%d.i=%d\n",
        sumi[i], ci[i-1], gi[i], pi[i], bin1[i], bin2[i], i);
    }
    //printf("Step8\nc[0] = %d, g=%d, p=%d, gcj=%d\n", ci[0], gi[0], pi[0], gcj[0]);
    printf("What is at 16384?\n");
  }*/

  /*
  i = 48;
  printf("TESTING: sum:%d, %d, %d\n", sumi[i-1], bits, i);
  printf("%d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d\n", 
    sumi[i-1], sumi[i-2], sumi[i-3], sumi[i-4], sumi[i-5], sumi[i-6], sumi[i-7], sumi[i-8],
    sumi[i-9], sumi[i-10], sumi[i-11], sumi[i-12], sumi[i-13], sumi[i-14], sumi[i-15], sumi[i-16]);
  printf("%d %d %d\n", bin1[i-6], bin2[i-6], ci[i-7]);*/
  

  //for(i=0;i<bits;i++){ printf("%d", sumi[i]);
  //} printf("\n");
}


//Master CLA routine
// Input/Output via global variables, as in provided data structures.
void cla(int runBarriers) {

  step1();  //Initial gi and pi generation
  //if(runBarriers == 1) {MPI_Barrier(MPI_COMM_WORLD); }
  //if(my_mpi_rank == 0) { printf("STEP1.\n"); }
  step2();
  //if(runBarriers == 1) {MPI_Barrier(MPI_COMM_WORLD); }
  //if(my_mpi_rank == 0) { printf("STEP2.\n"); }
  step3();
  //if(runBarriers == 1) {MPI_Barrier(MPI_COMM_WORLD); }
  //if(my_mpi_rank == 0) { printf("STEP3.\n"); }
  step4();
  //if(runBarriers == 1) {MPI_Barrier(MPI_COMM_WORLD); }
  //if(my_mpi_rank == 0) { printf("STEP4.\n"); }

  step5();  //Merging carry values
  //if(runBarriers == 1) {MPI_Barrier(MPI_COMM_WORLD); }
  //if(my_mpi_rank == 0) { printf("STEP5.\n"); }

  step6v2();
  //if(runBarriers == 1) {MPI_Barrier(MPI_COMM_WORLD); }
  //if(my_mpi_rank == 0) { printf("STEP6.\n"); }
  
  step7v2();
  //if(runBarriers == 1) {MPI_Barrier(MPI_COMM_WORLD); }
  //if(my_mpi_rank == 0) { printf("STEP7.\n"); }
  
  step8v2();
  //if(runBarriers == 1) {MPI_Barrier(MPI_COMM_WORLD); }
  //if(my_mpi_rank == 0) { printf("STEP8.\n"); }
  
  step9();  //Final summing
  //if(runBarriers == 1) {MPI_Barrier(MPI_COMM_WORLD); }
  //if(my_mpi_rank == 0) { printf("STEP9.\n"); }

  //Sum has now been set! It is a binary array, with most significant bit being at sumi[4095]

}


//Simple ripple carry tester. Mostly checking binary.
void simpleRippleCarryTest(){
  int i;
  //int newGi; going to actually try using what was generated earlier.
  //int newPi;

  int rippleSum[bits] = {0};
  int oldC = 0;
  int g=0;
  int p=0;

  printf("HERE\n");

  printf("--%d--", inputBin2[0]);


  for(i=0; i<24;i++){

    g = bin1[i] && bin2[i];
    p = bin1[i] || bin2[i];

    if(i < 24 && 1){  //Testing
      printf("Ripple: oldC=%d, gi=%d, pi=%d:b1=%d or b2=%d; result=%d.\n", 
        oldC, g, p, bin1[i], bin2[i], g || (p && oldC));
    }

    rippleSum[i] = bin1[i] ^ bin2[i] ^ oldC;
    oldC = g || (p && oldC);

  }

  printf("Ripple Carry Test Results:\n%s\n\n", convertToHexString(rippleSum));

  if(0) {
    printf("\nRIPPLE\n");
    for(i=0;i<bits;i++){  printf("%d", g);}
    printf("\n");
    for(i=0;i<bits;i++){  printf("%d", p);}
    printf("\nRIPPLE\n");
  }
}


//Begin program run here
// Compile Code: mpicc -g -Wall mpi-cla-io.c -o mpi-cla-io
// Example Run Code: mpirun -np 4 ./mpi-cla-io test_input_1.txt test_output_1.txt
//Creates output file according to argv[2]
int main(int argc, char** argv){

  my_mpi_size = -1;
  my_mpi_rank = -1;

  MPI_Init( &argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &my_mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_mpi_rank);
  
  int rankSize = bits/my_mpi_size;

  //Have to make sure all ranks allocate
  //bin1 = (int *) malloc( (rankSize) * sizeof(int));
  //bin2 = (int *) malloc( (rankSize) * sizeof(int));

  inputBin1 = (int *) malloc( (bits) * sizeof(int));
  inputBin2 = (int *) malloc( (bits) * sizeof(int));
  
  //printf("Rank %d/%d: Initial Program Open. rankSize=%d\n", my_mpi_rank, my_mpi_size, rankSize);
  

  if( argc != 3 ) {
    printf("Not sufficient arguments, only %d found \n", argc);
    exit(-1);
  }

  if( 0 == my_mpi_rank ) { // compare 0 first ensures == operator must be used and not just =
    //printf("MPI Rank %d: Attempt to Read File Data \n", my_mpi_rank );

    if( (my_input_file = fopen( argv[1], "r")) == NULL ) {
      printf("Failed to open input data file: %s \n", argv[1]);
    }
    if( (my_output_file = fopen( argv[2], "w")) == NULL ) {
      printf("Failed to open input data file: %s \n", argv[2]);
    }

    //Allocate and fill input hex strings
    hex1 = (char *) malloc( (digits+1) * sizeof(char)); //+1 to account for \0
    hex2 = (char *) malloc( (digits+1) * sizeof(char));

    //Read in our two input hex strings
    fscanf( my_input_file, "%s %s", hex1, hex2);
    //fprintf( my_output_file, "%s\n%s\n", hex_input_a, hex_input_b );

    hex1[digits] = '\0';  //Add null terminator
    hex2[digits] = '\0';

    //Convert the two hexadecimal strings into usable numbers, saving to globals bin1/2
    convertToNumber(hex1, inputBin1);
    convertToNumber(hex2, inputBin2);
    //Makes old function readInput obsolete. TODO delete it

    //printf("MPI Rank %d: Finished Reading File Data. %d, %d\n",
    //  my_mpi_rank, inputBin1[bits], inputBin2[bits]);

    fclose( my_input_file );
    //fclose( my_output_file );

//Begin timer
int result;//rippleSum[bits] = {0};
int oldC = 0;
int g=0;
int p=0;
int i=0;
double finish_time = -1;
double start_time = MPI_Wtime();
int x;
for(x = 0; x<10;x++) {
start_time = MPI_Wtime();

//rippleSum[bits] = {0};
oldC = 0;
g=0;
p=0;
i=0;

for(i=0; i<bits;i++){

  //g = inputBin1[i] && inputBin2[i];
  //p = inputBin1[i] || inputBin2[i];


  //rippleSum[i]
  result = inputBin1[i] ^ inputBin2[i] ^ oldC;


  /*if(i>=bits/2 && i<bits/2+8){//i % 1024 == 0 && i/1024 < 80){  //Testing
    printf("Ripple: sum=%d, C=%d, gi=%d, pi=%d:b1=%d or b2=%d. i=%d\n", 
      rippleSum[i], oldC, g, p, inputBin1[i], inputBin2[i], i);
    //printf(",%d%d", oldC, g||(p&&oldC));
    //printf("%d", rippleSum[i]);//oldC);
  } */

  oldC = g || (p && oldC);

}
//printf("\n");
//char* temp = convertToHexString(rippleSum);
//temp[80] = '\0';
//printf("Ripple Carry Test Results:\n%s\n\n", temp+digits-50);

finish_time = MPI_Wtime();
printf("%lf,",finish_time - start_time);
}
printf("\nresult=%d\n", result);

  }
  

  //printf("Rank %d: Reached point before scatter\n", my_mpi_rank);
  MPI_Barrier(MPI_COMM_WORLD);
  return 0; //Just testing RCA here

  //Begin timer
  double finish_time = -1;
  double start_time = MPI_Wtime();

  //Split bin1 and bin2 up for the X ranks
  MPI_Scatter(inputBin1, rankSize, MPI_INT,  //Distribute both bin1 and bin2
    &bin1, rankSize, MPI_INT,
    0, MPI_COMM_WORLD);
  MPI_Scatter(inputBin2, rankSize, MPI_INT,
    &bin2, rankSize, MPI_INT,
    0, MPI_COMM_WORLD);

  //Done with inputBins, can free
  free(inputBin1);
  free(inputBin2);


  //printf("Rank %d: Finished scattering data.\n", my_mpi_rank);


  //startIndex =  my_mpi_rank      * rankSize;
  //endIndex   = (my_mpi_rank + 1) * rankSize;

  MPI_Barrier(MPI_COMM_WORLD);

  cla(testing_Barriers);
  //step1(); step2();

  //printf("Rank %2.d/%d: Step2. g=%d,%d, p=%d,%d.\n", my_mpi_rank, my_mpi_size,
  //  ggj[0],ggj[ngroups/my_mpi_size-1], gpj[0], gpj[ngroups/my_mpi_size-1]);

  //printf("Rank %d/%d: Hello World. RankSize=%d.\n", my_mpi_rank, my_mpi_size, rankSize);

  MPI_Barrier(MPI_COMM_WORLD);
  //End the timer
  finish_time = MPI_Wtime();


  //Collect the sums
  int finalSum[bits] = {0}; //Final collecting array
  MPI_Gather(&sumi, rankSize, MPI_INT, &finalSum, rankSize, MPI_INT, 0, MPI_COMM_WORLD);

  //Print the results
  /*MPI_Request finalRecvReq, finalSendReq;
  MPI_Status finalStatus;
  int tempSender = -1;
  int finalCarry = -1;*/
  if(my_mpi_rank == 0) {
    /*int j;
    printf("Final sum:");
    for(j=65524; j<65536+8;j++) {printf("%d", finalSum[j]);}
    printf("\n"); */

    //Get the final bit, may have been missed due to rounding?
/*    if(bits % my_mpi_size > 0) {
      finalCarry = -1;
      MPI_Irecv(&finalCarry, 1, MPI_INT, my_mpi_size-1, 24, MPI_COMM_WORLD, &finalRecvReq);
      MPI_Wait(&finalRecvReq, &finalStatus);

      finalSum[bits-4] = inputBin1[bits-5] ^ inputBin2[bits-5] ^ finalCarry;
      printf("%d%d%d%d-ertsert\n", inputBin1[bits-4], inputBin1[bits-5],
        inputBin2[bits-4], inputBin2[bits-5]);
    } TODO fix last bit stuff!
    printf("%d, %d, %d--\n",
      finalSum[bits-4], finalSum[bits-5], finalSum[bits-6]);
    */

    printOutput(finalSum, my_output_file);

    if(testing_RunTime) { //Timing things?
      //fprintf(my_output_file, "\nTime:%lf\n", finish_time - start_time);
      printf("Run Time: %lf\n", finish_time - start_time);
    }

    fclose(my_output_file);

  }/* else if(my_mpi_rank == my_mpi_size - 1) {
    //ci[bits] = gi[bits] || (pi[bits] && gcj[bits/block_size]);
    tempSender = ci[(bits/my_mpi_size)-1];
    MPI_Isend(&tempSender, 1, MPI_INT, 0, 24, MPI_COMM_WORLD, &finalSendReq);
  }*/

  MPI_Finalize();

  //Free things
  //free(bin1);
  //free(bin2);
  free(hex1);
  free(hex2);
  //free(inputBin1);
  //free(inputBin2);

  return 0;
}




















