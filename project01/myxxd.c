#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hex = 16;
int binary = 6;


/**
 * Parses the command line.
 *
 * argc: the number of items on the command line (and length of the
 *       argv array) including the executable
 * argv: the array of arguments as strings (char* array)
 * bits: the integer value is set to TRUE if bits output indicated
 *       outherwise FALSE for hex output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *bits) {
  if (argc > 2) {
    printf("Usage: %s [-b|-bits]\n", argv[0]);
    exit(1);
  }

  if (argc == 2 &&
      (strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "-bits") == 0)) {
    *bits = 1;
  } else {
    *bits = 0;
  }

  return stdin;
}

/**
 * Writes data to stdout in hexadecimal.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsHex(unsigned char *data, size_t size) {
  for (int i = 0; i < hex; i++) {
    if (i < hex - 1 && !(i % 2)) {
        printf(" ");
    }

    if (i < (int)size) {
      printf("%02x", *(data + i));
    } else {
      printf("%*c", 2, ' ');
    }
  }
}

/**
 * Writes data to stdout as characters.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsChars(unsigned char *data, size_t size) {
  for (int i = 0; i < (int) size; i++) {
    unsigned char currentChar = *(data + i);

    if (currentChar > 126 || currentChar < 32) {
        currentChar = '.';
    }

    printf("%c", currentChar);
  }
}

void readAndPrintInputAsHex(FILE *input) {
  unsigned char data[hex];
  int numBytesRead = fread(data, 1, hex, input);
  unsigned int offset = 0;
  while (numBytesRead != 0) {
    printf("%08x:", offset);
    offset += numBytesRead;
    printDataAsHex(data, numBytesRead);
    printf("  ");
    printDataAsChars(data, numBytesRead);
    printf("\n");
    numBytesRead = fread(data, 1, hex, input);
  }
}
/**
 *
 * Printing data as binary 
 * 
 **/
void printDataAsBinary(unsigned char *data, size_t size) {
  for (int i = 0; i < binary; i++) {
    printf(" ");

    if (i < (int)size) {
      char byteString[8];
      unsigned char currentChar = *(data + i);

      for (int i = 7; i >= 0; i--) {
        byteString[i] = currentChar % 2 == 1 ? '1' : '0';
        currentChar /= 2;
      }

      printf("%s", byteString);
    } else {
      // 8 spaces where a byte would go
      printf("%*c", 8, ' ');
    }
  }
}

/**
 * Bits output for xxd.
 *
 * See myxxd.md for details.
 *
 * input: input stream
 **/
void readAndPrintInputAsBits(FILE *input) {
  unsigned char data[binary];
  int numBytesRead = fread(data, 1, binary, input);
  unsigned int offset = 0;
  while (numBytesRead != 0) {
    printf("%08x:", offset);
    offset += numBytesRead;
    printDataAsBinary(data, numBytesRead);
    printf("  ");
    printDataAsChars(data, numBytesRead);
    printf("\n");
    numBytesRead = fread(data, 1, binary, input);
  }
}




int main(int argc, char **argv) {
  int bits = 0;
  FILE *input = parseCommandLine(argc, argv, &bits);

  if (bits == 0) {
    readAndPrintInputAsHex(input);
  } else {
    readAndPrintInputAsBits(input);
  }
  return 0;
}
