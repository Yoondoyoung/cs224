#include <stdio.h>

int main()
{

    // This is our char being converted
    unsigned char c = 'p';

    // You'll also need an array to store the bit string,
    int arr[8];
    // either chars or ints will work

    /*
        Add your binary conversion code here
    */
    for (int i = 0; i < 8; i++) {
        if (c%2 == 1) {
            arr[i] = 1;
        }else {
            arr[i] = 0;
        }
        c = c / 2;
    }

    // Print out the resulting binary number
    for(int i = 0; i < 8; i++) {
        printf("Iteration %d : c = %d --> %d \n", i, c, arr[i]);
    }

    for(int i = 7; i >= 0; i--) {
        printf("%d",arr[i]);
    }

    return 0;
}    