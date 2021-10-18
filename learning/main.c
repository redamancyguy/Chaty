#include "string.h"
#include "stdio.h"
#define L 7
long GetPlace(const long *array,long length,long target){
    long left = 0,right = length,temp;
    while(left<=right){
        temp = (left+right)/2;
        if(array[temp] - target < 0){
            left = temp +1;
        }else if(array[temp] - target > 0){
            right = temp-1;
        }else{
            return temp;
        }
    }
    while(target < array[temp] && temp > 0){
        temp--;
    }
    while(target > array[temp] && temp < length){
        temp++;
    }
//    if(temp == 0){
//        if(array[temp] < target){
//            temp++;
//        }
//    }
    return temp;
}
int main(){
    long a[L] = {1,5,9,45,99,999,433432};
    printf("%ld\n", GetPlace(a,L,0));
    printf("%ld\n", GetPlace(a,L,4));
    printf("%ld\n", GetPlace(a,L,6));
    printf("%ld\n", GetPlace(a,L,55));
    printf("%ld\n", GetPlace(a,L,100));
    printf("%ld\n", GetPlace(a,L,989));
    printf("%ld\n", GetPlace(a,L,111119189));
}