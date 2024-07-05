#include <stdio.h>
  #define SIZE 4
  int main() {
    int a[SIZE];
    char *s_ptr = (char *)a;
    scanf("%s", s_ptr);
    for(int i = 0; i < SIZE - 1; i++){
      printf("a[%d] = %x\n", i, a[i]);
}
return 0; }
