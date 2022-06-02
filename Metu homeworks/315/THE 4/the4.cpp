#include "the4.h"



int recursive_sln(int i, int*& arr, int &number_of_calls){ //direct recursive
    number_of_calls+=1;

    //your code here
    int x,y,z;
    if(i==0) return arr[0];
    if(i==1) return fmax(arr[0], arr[1]);
    if(i==2) {
        int m;
        m =fmax(arr[0], arr[1]);
        m= fmax(m, arr[2]);
        return m;
    }
    x= recursive_sln(i-3, arr, number_of_calls)+arr[i];
    z=recursive_sln(i-1, arr, number_of_calls);
    int m=0;
    m =fmax(x, z);

    return m; // this is a dummy return value. YOU SHOULD CHANGE THIS!
}

int M2(int i, int*& arr, int*& mem){
    int x, y,z;
    int m;
    if(i>=3){
        if( mem[i-3]==-1){
            x = mem[i]= M2(i-3, arr, mem)+ arr[i];
        }
        else{
            x = mem[i-3]+ arr[i];
        }
        if( mem[i-1]==-1){
            y= M2(i-1, arr, mem);
        }
        else{
            y = mem[i-1];
        }
        if( mem[i-2]==-1){
            z  = M2(i-2, arr, mem);
        }
        else{
            z = mem[i-2];
        }
        
        m =fmax(x, y);
        m= fmax(m, z);
        mem[i]=m;
    }
    return mem[i];
    
}

int memoization_sln(int i, int*& arr, int*& mem){ //memoization

    //your code here
    int sum;
    if(i==0) return arr[0];
    if(i==1) return fmax(arr[0], arr[1]);
    if(i==2) {
        int m;
        m =fmax(arr[0], arr[1]);
        m= fmax(m, arr[2]);
        return m;
    }
    mem[0]= arr[0];
    mem[1]= fmax(arr[0], arr[1]);
    mem[2]= fmax(mem[1], arr[2]);
    sum = M2(i , arr, mem);
    return sum; // this is a dummy return value. YOU SHOULD CHANGE THIS!
}



int dp_sln(int size, int*& arr, int*& mem){ //dynamic programming
    int tem;
    //your code here
    if(size==1) return arr[0];
    if(size==2) return fmax(arr[0], arr[1]);
    if(size==3) {
        int m;
        m =fmax(arr[0], arr[1]);
        m= fmax(m, arr[2]);
        return m;
    }
    if(size > 3){
        mem[0]= arr[0];
        mem[1]= fmax(arr[0], arr[1]);
        mem[2]= fmax(mem[1], arr[2]);
        for(int i=3; i< size;i++){
            tem= fmax( mem[i-3]+ arr[i], mem[i-1]);
            mem[i] = fmax(tem , mem[i-2]);
        }
    }

    return mem[size-1]; // this is a dummy return value. YOU SHOULD CHANGE THIS!
}













