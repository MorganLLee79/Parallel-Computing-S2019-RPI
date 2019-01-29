#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> //C99 standard? Too far ahead?
#include <string.h>


/***** Data Structures (Provided) *********/

// EXAMPLE DATA STRUCTURE DESIGN AND LAYOUT FOR CLA
#define input_size 1024
#define block_size 8

//Do not touch these defines
#define digits (input_size+1)                 //1025
#define bits digits * 4                       //4100
#define ngroups bits/block_size               //512
#define nsections ngroups/block_size          //64
#define nsupersections nsections/block_size   //8

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

    if((/*i % 128 == 0 ||*/ i < 4) && false) { //Only part of the hex. && true for debuging/disabling
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

  bin1 = (int *) malloc( (bits) * sizeof(int));
  bin2 = (int *) malloc( (bits) * sizeof(int));

  hex1 = (char *) malloc( (digits+1) * sizeof(char));
  hex2 = (char *) malloc( (digits+1) * sizeof(char));


  //Read from file into hex1 and hex2
  scanf("%s", hex1 + sizeof(char));
  hex1[0] = '0';        //Add leading 0
  hex1[digits] = '\0';  //Add null terminator

  scanf("%s", hex2 + sizeof(char));
  hex2[0] = '0';
  hex2[digits] = '\0';

  //printf("TEST: %ld\n%s\n", strlen(hex2), hex2);
  //printf("Read numbers:\n%s\n%s\n", hex1, hex2);
  
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

    //if(i>digits-8) {
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
      printf("WARNING: Found incorrect byteTotal \'%d\' in convertToHexString().\n", byteTotal);
    }

    //Write the new char into our result string
    hexSum[i] = temp;

  }

  //printf("Ended at i:%d, bin index:%d; started at bin index:%d\n", i, currIndex-3, bits - 1);
  //printf("%d%d%d%d\n", inputBinary[currIndex-0], inputBinary[currIndex-1],
  //  inputBinary[currIndex-2], inputBinary[currIndex-3]);

  hexSum[i] = '\0'; //End the string.

  return hexSum;

}


// Takes in the number to be printed out
//  The given file will have the number in hexadecimal format
// Prints out to stdout.
void printOutput() {
  //int i;
  char *hexString;

  //Convert number to usable/printable string
  hexString = convertToHexString(sumi);

  //printf("Sum:\n");
  //for(i=0;i<bits;i++){  printf("%d", sumi[i]);}
  //printf("\n");

  //Print
  printf("%s\n", hexString);

  free(hexString);
}


/************** Program ******************/

//Calculate g_i and p_i for all 4096 bits i
void step1() {

  //gi = (int *) malloc( (bits) * sizeof(int));
  //pi = (int *) malloc( (bits) * sizeof(int));

  int i;

  //printf("TEST XOR: %d, %d, %d, %d\n", 0^0, 1^0,0^1,1^1);

  //xor things together
  for(i = 0; i < bits; i++){
    gi[i] = bin1[i] && bin2[i]; //g_i = a_i and b_i
    pi[i] = bin1[i] || bin2[i];
    //ci[i] = gi[i] || (pi[i] && ci[i-1]); //TODO properly use ci?
  }

  //printf("%d-%d-%d-%d--%d-\n", gi[i], pi[i], bin1[i], bin2[i], gi[bits-1]);
  //printf("step 1: %d, %d\n", gi[i-1], pi[i-1]);
}


//Calculate gg_j and gp_j for all 512 groups j using g_i and p_i
void step2() {

  //ggj = (int *) malloc( (bits) * sizeof(int));
  //gpj = (int *) malloc( (bits) * sizeof(int));

  int j;
  int i = 0;

  for(j=0; j < ngroups; j++) {
    ggj[j] = gi[i+7]
      || (gi[i+6] && pi[i+7])
      || (gi[i+5] && pi[i+7] && pi[i+6])
      || (gi[i+4] && pi[i+7] && pi[i+6] && pi[i+5]
      || (gi[i+3] && pi[i+7] && pi[i+6] && pi[i+5] && pi[i+4])
      || (gi[i+2] && pi[i+7] && pi[i+6] && pi[i+5] && pi[i+4] && pi[i+3])
      || (gi[i+1] && pi[i+7] && pi[i+6] && pi[i+5] && pi[i+4] && pi[i+3] && pi[i+2])
      || (gi[i]   && pi[i+7] && pi[i+6] && pi[i+5] && pi[i+4] && pi[i+3] && pi[i+2] && pi[i+1]));

    gpj[j] = pi[i+7] && pi[i+6] && pi[i+5] && pi[i+4] && pi[i+3] && pi[i+2] && pi[i+1] && pi[i];

    //Iterate/manage i as well.
    i = i + 8;

    if(i > bits) { printf("WARNING: step2, i surpassed bits! It is now: %d.\n", i); }
  }
}

//Calculate sg_k and sp_k for all 64 sections k using ggj and gpj (larger subsections)
void step3() {

  //sgk = (int *) malloc( (bits) * sizeof(int));
  //spk = (int *) malloc( (bits) * sizeof(int));

  int k;
  int j = 0;

  for(k=0; k < nsections; k++) {
    /*sgk[k] = ggj[j+3]
      || (ggj[j+2] && gpj[j+3])
      || (ggj[j+1] && gpj[j+3] && gpj[j+2])
      || (ggj[j]   && gpj[j+3] && gpj[j+2] && gpj[j+1]);

    spk[k] = gpj[j+3] && gpj[j+2] && gpj[j+1] && gpj[j];

    //Iterate/manage j as well.
    j = j + 4; */
    sgk[k] = ggj[j+7]
      || (ggj[j+6] && gpj[j+7])
      || (ggj[j+5] && gpj[j+7] && gpj[j+6])
      || (ggj[j+4] && gpj[j+7] && gpj[j+6] && gpj[j+5]
      || (ggj[j+3] && gpj[j+7] && gpj[j+6] && gpj[j+5] && gpj[j+4])
      || (ggj[j+2] && gpj[j+7] && gpj[j+6] && gpj[j+5] && gpj[j+4] && 
            gpj[j+3])
      || (ggj[j+1] && gpj[j+7] && gpj[j+6] && gpj[j+5] && gpj[j+4] && 
            gpj[j+3] && gpj[j+2])
      || (ggj[j]   && gpj[j+7] && gpj[j+6] && gpj[j+5] && gpj[j+4] && 
            gpj[j+3] && gpj[j+2] && gpj[j+1]));

    spk[j] = gpj[j+7] && gpj[j+6] && gpj[j+5] && gpj[j+4] && gpj[j+3] && gpj[j+2] && gpj[j+1] && gpj[j];

    //Iterate/manage j as well.
    j = j + 8;

    if(j > ngroups) { printf("WARNING: step3, j surpassed ngroups! It is now: %d.\n", j); }
  }
}


//Calculat ss_gl and sp_l for all 8 super sections l using sg_k and sp_k
void step4() {
  int l;
  int k = 0;

  for(l=0; l < nsupersections; l++) {
    /*ssgl[l] = sgk[k+3]
      || (sgk[k+2] && spk[k+3])
      || (sgk[k+1] && spk[k+3] && spk[k+2])
      || (sgk[k]   && spk[k+3] && spk[k+2] && spk[k+1]);

    sspl[l] = spk[k+3] && spk[k+2] && spk[k+1] && spk[k]; */

    ssgl[l] = sgk[k+7]
      || (sgk[k+6] && spk[k+7])
      || (sgk[k+5] && spk[k+7] && spk[k+6])
      || (sgk[k+4] && spk[k+7] && spk[k+6] && spk[k+5]
      || (sgk[k+3] && spk[k+7] && spk[k+6] && spk[k+5] && spk[k+4])
      || (sgk[k+2] && spk[k+7] && spk[k+6] && spk[k+5] && spk[k+4] && 
            spk[k+3])
      || (sgk[k+1] && spk[k+7] && spk[k+6] && spk[k+5] && spk[k+4] && 
            spk[k+3] && spk[k+2])
      || (sgk[k]   && spk[k+7] && spk[k+6] && spk[k+5] && spk[k+4] && 
            spk[k+3] && spk[k+2] && spk[k+1]));

    sspl[l] = spk[k+7] && spk[k+6] && spk[k+5] && spk[k+4] && 
              spk[k+3] && spk[k+2] && spk[k+1] && spk[k];


    //Iterate/manage k as well.
    k = k + 8;

    if(k > nsections) { printf("WARNING: step4, k surpassed nsections! It is now: %d.\n", k); }
  }
}


//Calculate ssc_l using ssg_l and ssp_l for all l super sections and 0 for ssc_-1
void step5() {
  //int l;

  //ssc_-1 = 0
  sscl[0] = ssgl[0] || (sspl[0] && 0);

  //And now, doing the other 7 super sections.
  sscl[1] = ssgl[1] || (sspl[1] && (ssgl[0] || (sspl[0] && 0)));
  sscl[2] = ssgl[2] || (sspl[2] && (ssgl[1] || (sspl[1] &&
    (ssgl[0] || (sspl[0] && 0)))));
  sscl[3] = ssgl[3] || (sspl[2] && (ssgl[2] || (sspl[2] && 
    (ssgl[1] || (sspl[1] &&
    (ssgl[0] || (sspl[0] && 0)))))));
  sscl[4] = ssgl[4] || (sspl[3] && (ssgl[3] || (sspl[2] && 
    (ssgl[2] || (sspl[2] && 
    (ssgl[1] || (sspl[1] &&
    (ssgl[0] || (sspl[0] && 0)))))))));
  sscl[5] = ssgl[5] || (sspl[4] && (ssgl[4] || (sspl[3] && 
    (ssgl[3] || (sspl[2] && 
    (ssgl[2] || (sspl[2] && 
    (ssgl[1] || (sspl[1] &&
    (ssgl[0] || (sspl[0] && 0)))))))))));
  sscl[6] = ssgl[6] || (sspl[6] && (ssgl[5] || (sspl[4] &&
    (ssgl[4] || (sspl[3] && 
    (ssgl[3] || (sspl[2] && 
    (ssgl[2] || (sspl[2] && 
    (ssgl[1] || (sspl[1] &&
    (ssgl[0] || (sspl[0] && 0)))))))))))));
  sscl[7] = ssgl[7] || (sspl[7] && (ssgl[6] || (sspl[6] && 
    (ssgl[5] || (sspl[4] &&
    (ssgl[4] || (sspl[3] && 
    (ssgl[3] || (sspl[2] && 
    (ssgl[2] || (sspl[2] && 
    (ssgl[1] || (sspl[1] &&
    (ssgl[0] || (sspl[0] && 0)))))))))))))));

  /* Not using for loop to partially simulate parallelism. Below should be effective the same
  for(l=1; l < nsupersections; l++) {

    sscl[l] = ssgl[l] || (sspl[l] && sscl[l-1]);
  }*/
}


//Calculate sc_k using sg_k and sp_k and correct ssc_l, l==k div 8 as
//  super sectional carry-in for all sections k
void step6() {
  int k;
  int x;

  for(k=0; k < nsections; k = k + 8) {
    
    sck[k] = sgk[k] || (spk[k] && sscl[k/8]);
    for(x=1; x<8;x++){  //Each group
      sck[k+x] = sgk[k+x] || (spk[k+x] && sck[k+x-1]);
    }

    //sck[k] = sgk[k] || (spk[k] && sck[k-1]);
    /*
    sck[k+0] = sgk[k+0] || (spk[k+0] && sscl[k/8]);
    sck[k+1] = sgk[k+1] || (spk[k+1] && (sgk[k+0] || (spk[k+0] && sscl[k/8])));
    sck[k+2] = sgk[k+2] || (spk[k+2] && (sgk[k+1] || (spk[k+1] &&
      (sgk[k+0] || (spk[k+0] && sscl[k/8])))));
    sck[k+3] = sgk[k+3] || (spk[k+2] && (sgk[k+2] || (spk[k+2] && 
      (sgk[k+1] || (spk[k+1] &&
      (sgk[k+0] || (spk[k+0] && sscl[k/8])))))));
    sck[k+4] = sgk[k+4] || (spk[k+3] && (sgk[k+3] || (spk[k+2] && 
      (sgk[k+2] || (spk[k+2] && 
      (sgk[k+1] || (spk[k+1] &&
      (sgk[k+0] || (spk[k+0] && sscl[k/8])))))))));
    sck[k+5] = sgk[k+5] || (spk[k+4] && (sgk[k+4] || (spk[k+3] && 
      (sgk[k+3] || (spk[k+2] && 
      (sgk[k+2] || (spk[k+2] && 
      (sgk[k+1] || (spk[k+1] &&
      (sgk[k+0] || (spk[k+0] && sscl[k/8])))))))))));
    sck[k+6] = sgk[k+6] || (spk[k+6] && (sgk[k+5] || (spk[k+4] &&
      (sgk[k+4] || (spk[k+3] && 
      (sgk[k+3] || (spk[k+2] && 
      (sgk[k+2] || (spk[k+2] && 
      (sgk[k+1] || (spk[k+1] &&
      (sgk[k+0] || (spk[k+0] && sscl[k/8])))))))))))));
    sck[k+7] = sgk[k+7] || (spk[k+7] && (sgk[k+6] || (spk[k+6] && 
      (sgk[k+5] || (spk[k+4] &&
      (sgk[k+4] || (spk[k+3] && 
      (sgk[k+3] || (spk[k+2] && 
      (sgk[k+2] || (spk[k+2] && 
      (sgk[k+1] || (spk[k+1] &&
      (sgk[k+0] || (spk[k+0] && sscl[k/8])))))))))))))));
    */

  }
}

//Calculate gc_j using gg_j, gp_j, and correct sc_k, k = j div 8 as sectional carry-in for all groups j
void step7() {
  int j;
  int x;

  for(j=0; j < ngroups; j = j + 8) {
    
    gcj[j] = ggj[j] || (gpj[j] && sck[j/8]);
    for(x=1; x<8;x++){  //Each group
      gcj[j+x] = ggj[j+x] || (gpj[j+x] && gcj[j+x-1]);
    }

    /*
    gcj[j+0] = ggj[j+0] || (gpj[j+0] && sck[j/8]);
    gcj[j+1] = ggj[j+1] || (gpj[j+1] && (ggj[j+0] || (gpj[j+0] && sck[j/8])));
    gcj[j+2] = ggj[j+2] || (gpj[j+2] && (ggj[j+1] || (gpj[j+1] &&
      (ggj[j+0] || (gpj[j+0] && sck[j/8])))));
    gcj[j+3] = ggj[j+3] || (gpj[j+2] && (ggj[j+2] || (gpj[j+2] && 
      (ggj[j+1] || (gpj[j+1] &&
      (ggj[j+0] || (gpj[j+0] && sck[j/8])))))));
    gcj[j+4] = ggj[j+4] || (gpj[j+3] && (ggj[j+3] || (gpj[j+2] && 
      (ggj[j+2] || (gpj[j+2] && 
      (ggj[j+1] || (gpj[j+1] &&
      (ggj[j+0] || (gpj[j+0] && sck[j/8])))))))));
    gcj[j+5] = ggj[j+5] || (gpj[j+4] && (ggj[j+4] || (gpj[j+3] && 
      (ggj[j+3] || (gpj[j+2] && 
      (ggj[j+2] || (gpj[j+2] && 
      (ggj[j+1] || (gpj[j+1] &&
      (ggj[j+0] || (gpj[j+0] && sck[j/8])))))))))));
    gcj[j+6] = ggj[j+6] || (gpj[j+6] && (ggj[j+5] || (gpj[j+4] &&
      (ggj[j+4] || (gpj[j+3] && 
      (ggj[j+3] || (gpj[j+2] && 
      (ggj[j+2] || (gpj[j+2] && 
      (ggj[j+1] || (gpj[j+1] &&
      (ggj[j+0] || (gpj[j+0] && sck[j/8])))))))))))));
    gcj[j+7] = ggj[j+7] || (gpj[j+7] && (ggj[j+6] || (gpj[j+6] && 
      (ggj[j+5] || (gpj[j+4] &&
      (ggj[j+4] || (gpj[j+3] && 
      (ggj[j+3] || (gpj[j+2] && 
      (ggj[j+2] || (gpj[j+2] && 
      (ggj[j+1] || (gpj[j+1] &&
      (ggj[j+0] || (gpj[j+0] && sck[j/8])))))))))))))));
    */    

  }


  //printf("Step 7: sck:%d, %d\n", sck[0], j);
}

//Calculate c_i using g_i, p_i, and correct gc_j, j = i div 8 as group carry-in for all bits i
void step8() {
  int i;
  int x;

  for(i=0; i < bits; i = i + 8) {

    ci[i] = gi[i] || (pi[i] && gcj[i/8]);

    
    for(x=1; x<8;x++){  //Each group
      ci[i+x] = gi[i+x] || (pi[i+x] && ci[i+x-1]);
    }
    
    
    /*
    ci[i+0] = gi[i+0] || (pi[i+0] && gcj[i/8]);
    ci[i+1] = gi[i+1] || (pi[i+1] && (gi[i+0] || (pi[i+0] && gcj[i/8])));
    ci[i+2] = gi[i+2] || (pi[i+2] && (gi[i+1] || (pi[i+1] &&
      (gi[i+0] || (pi[i+0] && gcj[i/8])))));
    ci[i+3] = gi[i+3] || (pi[i+2] && (gi[i+2] || (pi[i+2] && 
      (gi[i+1] || (pi[i+1] &&
      (gi[i+0] || (pi[i+0] && gcj[i/8])))))));
    ci[i+4] = gi[i+4] || (pi[i+3] && (gi[i+3] || (pi[i+2] && 
      (gi[i+2] || (pi[i+2] && 
      (gi[i+1] || (pi[i+1] &&
      (gi[i+0] || (pi[i+0] && gcj[i/8])))))))));
    ci[i+5] = gi[i+5] || (pi[i+4] && (gi[i+4] || (pi[i+3] && 
      (gi[i+3] || (pi[i+2] && 
      (gi[i+2] || (pi[i+2] && 
      (gi[i+1] || (pi[i+1] &&
      (gi[i+0] || (pi[i+0] && gcj[i/8])))))))))));
    ci[i+6] = gi[i+6] || (pi[i+6] && (gi[i+5] || (pi[i+4] &&
      (gi[i+4] || (pi[i+3] && 
      (gi[i+3] || (pi[i+2] && 
      (gi[i+2] || (pi[i+2] && 
      (gi[i+1] || (pi[i+1] &&
      (gi[i+0] || (pi[i+0] && gcj[i/8])))))))))))));
    ci[i+7] = gi[i+7] || (pi[i+7] && (gi[i+6] || (pi[i+6] && 
      (gi[i+5] || (pi[i+4] &&
      (gi[i+4] || (pi[i+3] && 
      (gi[i+3] || (pi[i+2] && 
      (gi[i+2] || (pi[i+2] && 
      (gi[i+1] || (pi[i+1] &&
      (gi[i+0] || (pi[i+0] && gcj[i/8])))))))))))))));
    //I started to regret not just making a sub loop at the end. This code is kinda gross >,<
    */
  }

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
  sumi[i] = bin1[i] ^ bin2[i] ^ 0;

  //This would actually be a parallel.
  for(i = 1; i < bits; i++) {
    sumi[i] = bin1[i] ^ bin2[i] ^ ci[i-1];
    //sumi[i] = gi[i] ^ pi[i] ^ ci[i-1];
  }

  /*printf("TESTING: sum:%d, %d, %d\n", sumi[i-1], bits, i);
  printf("%d%d%d%d%d%d%d%d %d%d%d%d%d%d%d%d\n", 
    sumi[i-1], sumi[i-2], sumi[i-3], sumi[i-4], sumi[i-5], sumi[i-6], sumi[i-7], sumi[i-8],
    sumi[i-9], sumi[i-10], sumi[i-11], sumi[i-12], sumi[i-13], sumi[i-14], sumi[i-15], sumi[i-16]);
  */

  //for(i=0;i<bits;i++){ printf("%d", sumi[i]);
  //} printf("\n");
}


//Master CLA routine
// Input/Output via global variables, as in provided data structures.
void cla() {

  step1();

  step2();

  step3();

  step4();

  step5();

  step6();
  
  step7();
  
  step8();
  
  step9();

  //Sum has now been set! It is a binary array, with most significant bit being at sumi[4095]

}


//Simple ripple carry tester. Mostly checking binary.
void simpleRippleCarryTest(){
  int i;
  //int newGi; going to actually try using what was generated earlier.
  //int newPi;

  int rippleSum[bits] = {0};
  int oldC = 0;

  for(i=0; i<bits+1;i++){

    if(i < 24 && false){  //Testing
      printf("Ripple: oldC=%d, gi=%d, pi=%d:b1=%d or b2=%d; result=%d\n", 
        oldC, gi[i], pi[i], bin1[i], bin2[i], gi[i] || (pi[i] && oldC));
    }

    rippleSum[i] = bin1[i] ^ bin2[i] ^ oldC;
    //rippleSum[i] = gi[i] ^ pi[i] ^ oldC;
    oldC = gi[i] || (pi[i] && oldC);

  }

  printf("Ripple Carry Test Results:\n%s\n\n", convertToHexString(rippleSum));

  if(false) {
    printf("\nRIPPLE\n");
    for(i=0;i<bits;i++){  printf("%d", bin1[i]);}
    printf("\n");
    for(i=0;i<bits;i++){  printf("%d", bin2[i]);}
    printf("\nRIPPLE\n");
  }
}

//Sanity checking some stuff
void relationsTests(){
  printf("And (&&): %d, %d, %d, %d.\n", 0 && 0, 0 && 1, 1 && 0, 1 && 1);
  printf("Or  (||): %d, %d, %d, %d.\n", 0 || 0, 0 || 1, 1 || 0, 1 || 1);
  printf("XOR (^ ): %d, %d, %d, %d.\n", 0 ^  0, 0 ^  1, 1 ^  0, 1 ^  1);
}


//Begin program run here
//Example Line: ./leeh17_hw1.c assignment1-testcase.txt
//Creates output file leeh17_hw1_output.txt
int main(int argc, char *argv[]){
  //printf("TESTING: Initial Program Open\n");

  readInput(argv[1]);

  //printf("\nInputs:\n%s\n\n%s\n\n", hex1, hex2);
  //printf("TESTING bin values: %d, %d\n", bin1[0], bin2[0]);

  //printf("Test convert to hex:\n%s\n%s\n", convertToHexString(bin1), convertToHexString(bin2));

  cla();
  //printf("TESTING bin values: %d, %d\n", bin1[0], bin2[0]);

  //simpleRippleCarryTest();
  //relationsTests();

  printOutput(); 
  //TODO free/empty things
  return 0; 
}




















