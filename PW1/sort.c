#include <stdio.h>
// bubble 
void sort(int n, int *num){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n - i - 1; j++){
            if(num[j] > num[j + 1]){
                int temp = num[j];
                num[j] = num[j + 1];
                num[j + 1] = temp;
            }
        }
    }
    return;
}
int main(){
    int n;
    int num[10000];
    scanf("%d", &n);
    for(int i = 0; i < n; i++){
        scanf("%d", num + i);
    }
    sort(n, num);
    for(int i = 0; i < n; i++){
        printf("%d ", num[i]);
    }
    printf("\n");
    return 0;
}