#include <stdio.h>

int main()
{
    //DEC , HEX
    unsigned char u = 51;
    unsigned char c = '3';
    // DEC , HEX
    unsigned char a = 55;
    unsigned char b = '7';
    // HEX, SYMBOL
    unsigned char x = 0x42;
    unsigned char y = 'B';

    // u and c
    if (u == c) {
        printf("They are the same1\n");
    }
    else {
        printf("They are different!\n");
    }
    // a and b
    if (a == b) {
        printf("They are the same2 \n");
    }else {
        printf("They are not the same2 \n");
    }
    // x and y
    if( x == y ) {
        printf("They are the same3\n");
    }else {
        printf("They are not the same3");
    }



    return 0;
}