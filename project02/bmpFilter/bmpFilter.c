#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

#define BAD_NUMBER_ARGS 1
#define BAD_OPTION 2
#define FSEEK_ERROR 3
#define FREAD_ERROR 4
#define MALLOC_ERROR 5
#define FWRITE_ERROR 6

/**
 * Parses the command line.
 *
 * argc:      the number of items on the command line (and length of the
 *            argv array) including the executable
 * argv:      the array of arguments as strings (char* array)
 * grayscale: the integer value is set to TRUE if grayscale output indicated
 *            outherwise FALSE for threshold output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *isGrayscale, int*isScaleDown) {

  if (argc > 2) {
    fprintf(stderr, "Usage: %s [-g]\n", argv[0]);
    exit(BAD_NUMBER_ARGS);
  }

  *isScaleDown = FALSE;

  if (argc == 2) {
    if (strcmp(argv[1], "-g") == 0) {
      *isGrayscale = TRUE;

    } else if (strcmp(argv[1], "-s") == 0) {
      // set isscale here
      *isScaleDown = TRUE;

    }else {
      fprintf(stderr, "Unknown option: '%s'\n", argv[1]);
      fprintf(stderr, "Usage: %s [-g]\n", argv[0]);
      exit(BAD_OPTION);  
    }
  }

  return stdin;
}

unsigned getFileSizeInBytes(FILE* stream) {
  unsigned fileSizeInBytes = 0;
  
  rewind(stream);
  if (fseek(stream, 0L, SEEK_END) != 0) {
    exit(FSEEK_ERROR);
  }
  fileSizeInBytes = ftell(stream);

  return fileSizeInBytes;
}

void getBmpFileAsBytes(unsigned char* ptr, unsigned fileSizeInBytes, FILE* stream) {
  rewind(stream);
  if (fread(ptr, fileSizeInBytes, 1, stream) != 1) {
#ifdef DEBUG
    printf("feof() = %x\n", feof(stream));
    printf("ferror() = %x\n", ferror(stream));
#endif
    exit(FREAD_ERROR);
  }
}

//Scale down
void scaleDownPixelArray(unsigned char* originalPixelArray, unsigned char* newPixelArray, int width, int height) {
    int newWidth = width / 2;
    int newHeight = height / 2;
    int oldPadding = (4 - (width * 3) % 4) % 4;
    int newPadding = (4 - (newWidth * 3) % 4) % 4;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            int oldIndex = (y * 2 * (width * 3 + oldPadding)) + (x * 2 * 3);
            int newIndex = (y * (newWidth * 3 + newPadding)) + (x * 3);

            // Average the 2x2 block of pixels
            for (int c = 0; c < 3; c++) {
                int sum = originalPixelArray[oldIndex + c] +
                          originalPixelArray[oldIndex + 3 + c] +
                          originalPixelArray[oldIndex + (width * 3 + oldPadding) + c] +
                          originalPixelArray[oldIndex + (width * 3 + oldPadding) + 3 + c];
                newPixelArray[newIndex + c] = sum / 4;
            }
        }
    }
}


unsigned char getAverageIntensity(unsigned char blue, unsigned char green, unsigned char red) {
  unsigned char avg = (blue+green+red)/3;
  return avg;
}

void applyGrayscaleToPixel(unsigned char* pixel) {
  unsigned char avg = getAverageIntensity(pixel[0], pixel[1], pixel[2]);  
  pixel[0] = pixel[1] = pixel[2] = avg;
}

void applyThresholdToPixel(unsigned char* pixel) {
  unsigned char avg = getAverageIntensity(pixel[0], pixel[1], pixel[2]);
  pixel[0] = pixel[1] = pixel[2] = (avg > 127) ? 255 : 0;
}

void applyFilterToPixel(unsigned char* pixel, int isGrayscale) {
  if(isGrayscale){
    applyGrayscaleToPixel(pixel);
  }else{
    applyThresholdToPixel(pixel);
  }
}

void applyFilterToRow(unsigned char* row, int width, int isGrayscale) {
  for (int i = 0; i < width; i++) {
    applyFilterToPixel(row + i * 3, isGrayscale);
  }
}

void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale) {
  int padding = (4 - (width * 3) % 4) % 4;

#ifdef DEBUG
  printf("padding = %d\n", padding);
#endif  
  
  for (int i = 0; i < height; i++) {
    applyFilterToRow(pixelArray + (width * 3 + padding) * i, width, isGrayscale);
  }
}

void parseHeaderAndApplyFilter(unsigned char* bmpFileAsBytes, int isGrayscale, int isScaleDown) {
  int offsetFirstBytePixelArray = *(int*)&bmpFileAsBytes[10];
  int width = *(int*)&bmpFileAsBytes[18];
  int height = *(int*)&bmpFileAsBytes[22];
  unsigned char* pixelArray = bmpFileAsBytes + offsetFirstBytePixelArray;

#ifdef DEBUG
  printf("offsetFirstBytePixelArray = %u\n", offsetFirstBytePixelArray);
  printf("width = %u\n", width);
  printf("height = %u\n", height);
  printf("pixelArray = %p\n", pixelArray);
#endif

if (isScaleDown) {
        int newWidth = width / 2;
        int newHeight = height / 2;
        int newSizePixelArray = ((newWidth * 3 + 3) & ~3) * newHeight;
        unsigned char* newPixelArray = (unsigned char*)malloc(newSizePixelArray);

        scaleDownPixelArray(pixelArray, newPixelArray, width, height);

        // Update header information
        *(int*)&bmpFileAsBytes[18] = newWidth;
        *(int*)&bmpFileAsBytes[22] = newHeight;
        *(int*)&bmpFileAsBytes[2] = offsetFirstBytePixelArray + newSizePixelArray; // file size
        *(int*)&bmpFileAsBytes[34] = newSizePixelArray; // image size

        // Copy new pixel array back to bmpFileAsBytes
        memcpy(pixelArray, newPixelArray, newSizePixelArray);

        free(newPixelArray);
    } else {
        applyFilterToPixelArray(pixelArray, width, height, isGrayscale);
    }

}

int main(int argc, char **argv) {
  int grayscale = FALSE;
  int isScaleDown = FALSE;
  unsigned fileSizeInBytes = 0;
  unsigned char* bmpFileAsBytes = NULL;
  FILE *stream = NULL;
  
  stream = parseCommandLine(argc, argv, &grayscale, &isScaleDown);
  fileSizeInBytes = getFileSizeInBytes(stream);

#ifdef DEBUG
  printf("fileSizeInBytes = %u\n", fileSizeInBytes);
#endif

  bmpFileAsBytes = (unsigned char *)malloc(fileSizeInBytes);
  if (bmpFileAsBytes == NULL) {
    exit(MALLOC_ERROR);
  }
  getBmpFileAsBytes(bmpFileAsBytes, fileSizeInBytes, stream);

  parseHeaderAndApplyFilter(bmpFileAsBytes, grayscale, isScaleDown);

  // If scaled down, we need to write less bytes
  unsigned writeSize = isScaleDown ? *(int*)&bmpFileAsBytes[2] : fileSizeInBytes;
#ifndef DEBUG
  if (fwrite(bmpFileAsBytes, writeSize, 1, stdout) != 1) {
    exit(FWRITE_ERROR);
  }
#endif

  free(bmpFileAsBytes);
  return 0;
}
