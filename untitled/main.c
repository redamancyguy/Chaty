#include <stdio.h>

int narcissistic( int number );
void PrintN( int m, int n );

int main()
{
    int m, n;

    scanf("%d %d", &m, &n);
    if ( narcissistic(m) ) printf("%d is a narcissistic number\n", m);
    PrintN(m, n);
    if ( narcissistic(n) ) printf("%d is a narcissistic number\n", n);

    return 0;
}



int narcissistic( int number ){
    int temp = number;
    int sum = 0;
    while(number != 0){
        sum += (number%10)*(number%10)*(number%10);
        number/=10;
    }
    return sum == temp;
}
void PrintN( int m, int n ){
    for(int i=m+1;i<=n;i++){
        if(narcissistic(i)){
            printf("%d\n",i);
        }
    }
}