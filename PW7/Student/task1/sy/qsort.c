#include "lib.h"
int a[1000];
int partition(int p, int r){
    int x = a[r];
    int i = p;
    int j = p;
    int tmp;
    while(j < r){
        if(x > a[j]){
            tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
            i = i + 1;
        }
        j = j + 1;
    }
    tmp = a[i];
    a[i] = a[r];
    a[r] = tmp;
    return i;
}
void qsort(int p, int r){
    int q;
    if(p < r){
        q = partition(p, r);
        qsort(p, q - 1);
        qsort(q + 1, r);
    }
    return;
}
int main(){
    int n;
    n = getarray(a);
    qsort(0, n - 1);
    putarray(n ,a);
    return 0;
}
