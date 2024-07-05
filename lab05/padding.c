#include <stdio.h>

int getPadding(int width)
{
    // COMPUTE THE PADDING HERE AND RETURN IT
    
    // Each pixel is 3 bytes (24 bits for RGB)
    int bytesPerRow = width * 3;
    
    // Calculate padding to make the row a multiple of 4 bytes
    int padding = (4 - (bytesPerRow % 4)) % 4;
    
    return padding;
}

int main()
{
    // The test widths, in pixels
    int w[] = {20, 400, 273, 750, 1200, 6, 7, 8};
    // The correct paddings, in bytes
    int p[] = {0, 0, 1, 2, 0, 2, 3, 0};

    int examples = 8;
    int numberWrong = 0;

    for(int i=0; i<examples; i++){
        int padding = getPadding(w[i]);
        if(padding != p[i]){
            numberWrong += 1;
            printf(" Incorrect for width %d\n Correct padding : %d\n You computed: %d\n", w[i], p[i], padding);
        }
    }

    if(numberWrong){
        printf("Your solution is incorrect.  You missed %d of %d\n", numberWrong, examples);
    }
    else{
        printf("Congratulations, you got them all correct!\n");
    }

    return 0;
}
