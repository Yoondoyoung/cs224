#include <stdio.h>

void bubble_sort(int arr[], int size) {
    int i, j;
    int swapped;
    
    for (i = 0; i < size - 1; i++) {
        swapped = 0;
        for (j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap arr[j] and arr[j+1]
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }
        
        // If no swapping occurred, array is already sorted
        if (swapped == 0)
            break;
    }
}
int main()
{
    int size; 
    float average = 0;
    printf("How long is the list?");
    scanf("%d", &size);
    int list[size];
    for(int i = 0; i < size; i++){
        int num;
        printf("Put your numbers : \n");
        scanf("%d", &num);
        list[i] = num;
    }

    //print list and caculate average
    for(int i=0; i < size; i++){
        printf("list : %d \n", list[i]);
        average += list[i]; 
    }
    average = average/size;
    
    printf("Average : %f \n", average);
    
    bubble_sort(list, size);
    
    //print sorted array
    for(int i=0; i < size; i++){
        printf("list : %d \n", list[i]);
    }

    return 0;
}

