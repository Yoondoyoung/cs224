#include <stdio.h>

int main() {
    int a = 0x00555942;
    char c[] = "BYU";
    int arr[8];
    int x = "abc";
    printf("String : %s\n", c);

    printf("addr of a : %p\n", &a);
    printf("addr of c : %p\n", &c);

    printf("size : %d\n", sizeof(c));

    for(int i = 0; i < 8; i++){
        if(x%2 == 1){
            arr[i] = 1;
        }else{
            arr[i] = 0;
        }
        x = x/2;
    }

    for(int i = 0; i < 8; i++){
        printf("%d", arr[i]);
    }


    return 0;
}