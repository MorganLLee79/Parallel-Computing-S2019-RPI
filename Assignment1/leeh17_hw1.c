#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> //C99 standard? Too far ahead?
#include <string.h>


/***** Data Structures (Provided) *********/

// EXAMPLE DATA STRUCTURE DESIGN AND LAYOUT FOR CLA
#define input_size 1024
#define block_size 8

//Do not touch these defines
#define digits (input_size+1)
#define bits digits * 4
#define ngroups bits/block_size
#define nsections ngroups/block_size
#define nsupersections nsections/block_size

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
int* bin1 = NULL;
int* bin2 = NULL;

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

  //Iterate through the string
  for(i = 0; i < digits; i++) {

    //Read the current digit, converting to a binary value
    // Does this with simple if cases for each potential expected char.a
    if(inputString[i] == 'F' ){  // F = 1111
      result[j] = 1;  result[j-1] = 1;  result[j-2] = 1;  result[j-3] = 1;

    } else if(inputString[i] == 'E') { // E = 1110
      result[j] = 1;  result[j-1] = 1;  result[j-2] = 1;  result[j-3] = 0;

    } else if(inputString[i] == 'D') { // 0 = 0000
      result[j] = 1;  result[j-1] = 1;  result[j-2] = 0;  result[j-3] = 1;

    } else if(inputString[i] == 'C') { // 0 = 0000
      result[j] = 1;  result[j-1] = 1;  result[j-2] = 0;  result[j-3] = 0;

    } else if(inputString[i] == 'B') { // 0 = 0000
      result[j] = 1;  result[j-1] = 0;  result[j-2] = 1;  result[j-3] = 1;

    } else if(inputString[i] == 'A') { // 0 = 0000
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
      result[j] = 0;  result[j-1] = 4;  result[j-2] = 0;  result[j-3] = 0;

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

    if((/*i % 128 == 0 ||*/ i < 4) && true) { //Only part of the hex. && true for debuging/disabling
      printf("Converted %c to %d%d%d%d.\n", inputString[i], 
          result[j], result[j-1], result[j-2], result[j-3]);
    }

    j = j - 4; //Iterate down; 4095 is most significant, 0 is least

  }

  printf("Finished converting to number, final i:%d final j: %d\n", i, j);

  //return result;
}


// Begin reading in input files. Parses them as well
//Input:  The file path
void readInput(char *inputFilePath) {

  //int bin1[bits];
  //int bin2[bits];
  bin1 = (int *) malloc( (bits) * sizeof(int));
  bin2 = (int *) malloc( (bits) * sizeof(int));

  //char hex1[digits+1];
  //char hex2[digits+1];
  hex1 = (char *) malloc( (digits+1) * sizeof(char));
  hex2 = (char *) malloc( (digits+1) * sizeof(char));

  FILE *fp;
  //char buffer[digits+1];

  //Read from file into hex1 and hex2
  printf("Opening file at \"%s\".\n", inputFilePath);
  fp = fopen(inputFilePath, "r");
  fscanf(fp, "%s", hex1 + sizeof(char));
  hex1[0] = '0';  //Add leading 0
  //fscanf(fp, "%s", buffer + sizeof(char));  //Expecting single "words", one number per line
  //buffer[0] = '0';
  //strcpy(hex1, buffer);

  fscanf(fp, "%s", hex2 + sizeof(char));
  hex2[0] = '0';
  //fscanf(fp, "%s", buffer + sizeof(char));
  //buffer[0] = '0';
  //strcpy(hex2, buffer);

  fclose(fp);
  printf("Closing file.\n");

  //printf("TEST: %ld\n%s\n", strlen(hex2), hex2);
  //printf("Read numbers:\n%s\n%s\n", hex1, hex2);
  
  //Convert the two hexadecimal strings into usable numbers, saving to globals bin1/2
  convertToNumber(hex1, bin1);
  convertToNumber(hex2, bin2);

  printf("bin1: %d%d%d%d \nbin2: %d%d%d%d\n", bin1[4095], bin1[4094], bin1[4093], bin1[4092],
    bin2[4095], bin2[4094], bin2[4093], bin2[4092]);
  
}


//char* convertToHexString(int* inputNumber){
  /*int i;  //To traverse 512 bytes
  int j;  //Traverse 8 bits in byte
  int k;  //The sum of the bits, turned into base 10

  //Go through each byte in the hw1Num
  for(i = 0; i < 512; i = i + 1){
    for(j = 0; j<8; j = j + 1) {

    take j to power, add to k; multiple times 2^i

    }
  } */
/*
  //Uh, this isn't part of CLA, hopefully it's fine to just use
  char hexOutput[1025];
  char tempBuffer;
  int i;  //Will need to traverse so that most significant byte (value[0]) is at the front

  for(i = 0; i < 512; i++) {
    //TODO convert integer, probably
    sprintf(tempBuffer, "%x", inputNumber.value[i]);

    hexOutput
  }
  
  return hexOutput;
}
*/

// Takes in the number to be printed out and the file path and name to print to.
//  The given file will have the number in hexadecimal format
//Input:  The number we're printing. TODO: Format
//        The file path/name string to print to. Ex: /here/folder/output.txt
/*void printOutput(char *filePath) {

  FILE *fp;
  char *hexString;
    
  //Make the new file
  fp = fopen(*filePath, "w+");

  //Convert number to usable/printable string
  hexString = convertToHexString(inputNumber);
  
  
  //Print/write to the file
  fprintf(fp, hexString);
  //Alternate: fputs("String", fp); //Just prints to given output stream, no formatting

  fclose(fp);

}
*/

/************** Program ******************/

//Calculate g_i and p_i for all 4096 bits i
void step1() {

  //gi = (int *) malloc( (bits) * sizeof(int));
  //pi = (int *) malloc( (bits) * sizeof(int));

  int i;

  //printf("TEST XOR: %d, %d, %d, %d\n", 0^0, 1^0,0^1,1^1);

  //xor things together
  for(i = 0; i < bits; i++){
    gi[i] = bin1[i] ^  bin2[i]; //g_i = a_i xor b_i
    pi[i] = bin1[i] || bin2[i];
    //ci[i] = gi[i] || (pi[i] && ci[i-1]); //TODO properly use ci?
  }

  //printf("%d-%d-%d-%d--%d-\n", gi[i], pi[i], bin1[i], bin2[i], gi[bits-1]);

}


//Calculate gg_j and gp_j for all 512 groups j using g_i and p_i
void step2() {

  //ggj = (int *) malloc( (bits) * sizeof(int));
  //gpj = (int *) malloc( (bits) * sizeof(int));

  int j;
  int i = 0;

  for(j=0; j < ngroups; j++) {
    ggj[j] = gi[i+3]
      || (gi[i+2] && pi[i+3])
      || (gi[i+1] && pi[i+3] && pi[i+2])
      || (gi[i]   && pi[i+3] && pi[i+2] && pi[i+1]);

    gpj[j] = pi[i+3] && pi[i+2] && pi[i+1] && pi[i];

    //Iterate/manage i as well.
    i = i + 4;

    if(i >= bits) { printf("WARNING: step2, i surpassed bits! It is now: %d.\n", i); }
  }
}

//Calculate sg_k and sp_k for all 64 sections k using ggj and gpj (larger subsections)
void step3() {

  //sgk = (int *) malloc( (bits) * sizeof(int));
  //spk = (int *) malloc( (bits) * sizeof(int));

  int k;
  int j = 0;

  for(k=0; k < nsections; k++) {
    sgk[k] = ggj[j+3]
      || (ggj[j+2] && gpj[j+3])
      || (ggj[j+1] && gpj[j+3] && gpj[j+2])
      || (ggj[j]   && gpj[j+3] && gpj[j+2] && gpj[j+1]);

    spk[k] = gpj[j+3] && gpj[j+2] && gpj[j+1] && gpj[j];

    //Iterate/manage j as well.
    j = j + 4;

    if(j >= ngroups) { printf("WARNING: step3, j surpassed ngroups! It is now: %d.\n", j); }
  }
}


//Calculat ss_gl and sp_l for all 8 super sections l using sg_k and sp_k
void step4() {
  int l;
  int k = 0;

  for(l=0; l < nsupersections; l++) {
    ssgl[l] = sgk[k+3]
      || (sgk[k+2] && spk[k+3])
      || (sgk[k+1] && spk[k+3] && spk[k+2])
      || (sgk[k]   && spk[k+3] && spk[k+2] && spk[k+1]);

    sspl[l] = spk[k+3] && spk[k+2] && spk[k+1] && spk[k];

    //Iterate/manage k as well.
    k = k + 4;

    if(k >= nsections) { printf("WARNING: step4, k surpassed nsections! It is now: %d.\n", j); }
  }
}


//Calculate ssc_l using ssg_l and ssp_l for all l super sections and 0 for ssc_-1
void step5() {
  int l;
  int k = 0;
tg5re4ghr
  for(l=0; l < nsupersections; l++) {
    ssgl[l] = sgk[k+3]
      || (sgk[k+2] && spk[k+3])
      || (sgk[k+1] && spk[k+3] && spk[k+2])
      || (sgk[k]   && spk[k+3] && spk[k+2] && spk[k+1]);

    sspl[l] = spk[k+3] && spk[k+2] && spk[k+1] && spk[k];

    //Iterate/manage k as well.
    k = k + 4;

    if(k >= nsections) { printf("WARNING: step5, k surpassed nsections! It is now: %d.\n", j); }
  }
}


//Calculate sc_k using sg_k and sp_k and correct ssc_l, l==k div 8 as
//  super sectional carry-in for all sections k


//Calculate gc_j using gg_j, gp_j, and correct sc_k, k = j div 8 as sectional carry-in for all groups j


//Calculate c_i using g_i, p_i, and correct gc_j, j = i div 8 as group carry-in for all bits i


//Calculate sum_i using a_i * b_i * c_i-1 for all i where * is xor



//Master CLA routine
// Input/Output via global variables, as in provided data structures.
void cla() {

  step1();

}





//Begin program run here
//Example Line: ./leeh17_hw1.c assignment1-testcase.txt
//Creates output file leeh17_hw1_output.txt
int main(int argc, char *argv[]){
  printf("TESTING: Initial Program Open\n");

  readInput(argv[1]);

  cla();

  //printOutput(, "leeh17_hw_output.txt");
 
  //TODO Empty things
  return 1; 
}




















