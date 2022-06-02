#include "the3.h"

// do not add extra libraries here
using namespace std;

/*
    arr       : array to be sorted, in order to get points this array should contain be in sorted state before returning
    ascending : true for ascending, false for descending 
    n         : number of elements in the array
    l         : the number of characters used in counting sort at each time
    
    you can use ceil function from cmath
    
*/
int us(int a){
    int b=1;
    while(a>0){
        b = b *26;
        a--;
    } 
    return b;
}
int wordint(std::string word, int position, int end, int srt){
    int i=0;
    for(int j = 0; j< end-srt ;j++){
        i = i+ int(word[end-j])*(us(j));
    }
    return i;
}
int countsorta(std::string arr[], int n, int w_size,int srt, int l, int& it){
    int end= srt +l;
    long k = us(l);
    std::string B[n];
    int* C = new int[k];
    for(int i=0; i< k; i++ ){
        C[i]=0; it++;
    }
    for(int j = 0; j<n ; j++){
        C[wordint(arr[j],w_size,end,srt)]= C[wordint(arr[j],w_size,end,srt)]+1;
        it++;
    }
    for(int i=1; i < k; i++){
        C[i] = C[i] + C[i-1];
        it++;
    }
    
    for(int j=n-1; j>=1 ;j--){
        B[C[wordint(arr[j],w_size,l,srt)]]= arr[j];
        C[wordint(arr[j],w_size,l,srt)]= C[wordint(arr[j],w_size,l,srt)]-1;
        it++;
    }
    for(int j=0 ;j< n ;j++){
        B[j]=arr[j];
    }
    
    return it;
}
int radixSort(std::string arr[], bool ascending, int n, int l){
    int size =arr[0].size(); 
    int srt = size-l;
    if(n == 0){
        return 0;
    }
    int iter =0;
    std::string B[n];
    int pro=0;
    if(ascending){
        for(int i=size/l; i>=0 ;i--, srt-=l){
        countsorta(arr,n,size,srt,l,iter);}
    }
    else{
        for(int i=size/l ; i>0 ;i--){
       // countsortd(arr,n,w);}
    }}

    return iter;    
    
}
