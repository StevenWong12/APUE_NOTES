#include "stdio.h"
#include "unistd.h"
#include"apue.h"
int main(void){
    long *arr = calloc(5, sizeof(long));
    long *arr2 = calloc(5, sizeof(long*));
    printf("%ld\n", arr[0]);
    if(arr2 == NULL)
        printf("is null\n");
    else
        printf("isn't null");
}