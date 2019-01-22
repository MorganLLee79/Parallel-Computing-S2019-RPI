#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> //C99 standard? Too far ahead?


/********** I/O and Setup **********/

//Data Structure to save a number
struct hw1Num {
  //2d array of booleans, sub-arrays = 8 bit blocks (aka bytes)
  bool value[512][8]  //4096 bits = 512 bytes of 8
  //!!! Make sure value [0] is most significant byte

  //Potentially replace with just using value[512][8]
};


//Convert the given hex string into a usable number.
//Input:  A hexadecimal string representing an integer
//Return: A number __TODO define type
//         -1 = bug number
struct hw1Num convertToNumber(char *inputString) {

  int result;

  result = -1;  //Initial "bug" value



    
  return result;
}


// Begin reading in input files. Parses them as well
//Input:  The file path
//Return: The two numbers in the input file as an array of length 2.
struct hw1Num[2] readInput(char *inputFilePath) {

  FILE *fp;
  char hexString1[1024];  //Know numbers will be 1024 characters long.
  char hexString2[1024];

  struct hw1Num number1;
  struct hw1Num number2;
  

  fp = fopen(inputFilePath, "r");
  fscanf(fp, "%s", hexString1);  //Expecting single "words", one number per line
  fscanf(fp, "%s", hexString2);

  fclose(fp);

  
  //Convert the two hexadecimal strings into usable numbers
  number1 = convertToNumber(hexString1);
  number2 = convertToNumber(hexString2);
  
}


char* convertToHexString(struct hw1Num inputNumber){
  /*int i;  //To traverse 512 bytes
  int j;  //Traverse 8 bits in byte
  int k;  //The sum of the bits, turned into base 10

  //Go through each byte in the hw1Num
  for(i = 0; i < 512; i = i + 1){
    for(j = 0; j<8; j = j + 1) {

    take j to power, add to k; multiple times 2^i

    }
  } */

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


// Takes in the number to be printed out and the file path and name to print to.
//  The given file will have the number in hexadecimal format
//Input:  The number we're printing. TODO: Format
//        The file path/name string to print to. Ex: /here/folder/output.txt
void printOutput(struct hw1Num inputNumber, char *filePath) {

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


/************** Program ******************/

//Calculate g_i and p_i for all 4096 bits i
bool[512] or * step1(struct hw1Num input1, struct hw1Num input2) {



}


//Calculate gg_j and gp_j for all 512 groups j using g_i and p_i


//Calculate sg_k and sp_k for all 64 sections k using ggj and gpj (larger subsections)


//Calculat ss_gl and sp_l for all 8 super sections l using sg_k and sp_k



//Calculate ssc_l using ssg_l and ssp_l for all l super sections and 0 for ssc_-1



//Calculate sc_k using sg_k and sp_k and correct ssc_l, l==k div 8 as
//  super sectional carry-in for all sections k


//Calculate gc_j using gg_j, gp_j, and correct sc_k, k = j div 8 as sectional carry-in for all groups j


//Calculate c_i using g_i, p_i, and correct gc_j, j = i div 8 as group carry-in for all bits i


//Calculate sum_i using a_i * b_i * c_i-1 for all i where * is xor



//Master CLA routine
struct hw1Num cla(input1, input2) {

}






//Begin program run here
//Example Line: ./leeh17_hw1.c assignment1-testcase.txt
//Creates output file leeh17_hw1_output.txt
void main(int argc, char *argv[]){
  printf("TESTING: Initial Program Open\n");

  


  printOutput(sum__, "leeh17_hw_output.txt");
  
}
