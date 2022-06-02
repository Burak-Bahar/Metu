#include "the5.h"
// do not add extra libraries here

/*
W: the width of the field
L: the length of the field
partitions: 2D Boolean array for partitions, if a partition of w x l exists then partitions[x][y] is true.
numberOfCalls/numberOfIterations: method specific control parameter
*/

/* recursive */
int f_1( int W,int L ,bool** partitions,int* numberOfCalls, int a, int b, int we, int le){
    int wl=900000,lw=900000,mini;
    int waste= W*L;
    (*numberOfCalls)++;
    if(partitions[W][L]==1){return 0;}
    else{
        while(0 <= a &&  a < we){
            a++;
            wl= std::min(wl, f_1(W-a, L, partitions, numberOfCalls, 0, 0,(W-a)/2,le)+  f_1(a, L , partitions, numberOfCalls, 0, 0,a/2,le));
            }
        while(0<= b && b < le){
            b++;
            lw = std::min(lw,f_1(W, L-b, partitions, numberOfCalls, 0, 0,we,(L-b)/2 )+ f_1(W, b , partitions, numberOfCalls, 0, 0,we,b/2));
            
            }
        }   
    mini= std::min(waste, std::min(lw, wl));
        
    return mini;
}
int f_2( int W,int L ,bool** partitions,int* numberOfCalls, int a, int b, int we, int le, int** memory){
    int wl=900000, wl1=900000, wl2=900000;
    int lw=900000, lw1=900000, lw2=900000;
    int mini;
    int waste= W*L;
    (*numberOfCalls)++;
    if(partitions[W][L]==1){return 0;}
    if(memory[W][L]!=-1){return memory[W][L];}
        while(0 <= a &&  a < we){
            a++;

                wl1 =f_2(W-a, L, partitions, numberOfCalls, 0, 0,(W-a)/2,le, memory);
               // memory[W-a][L]= wl1;

                wl2 =f_2(a, L , partitions, numberOfCalls, 0, 0,a/2,le, memory);
                //memory[a][L]= wl2;
            wl=  std::min(wl, wl1+wl2);
        }
        while(0<= b && b < le){
            b++;
                lw1 =f_2(W, L-b, partitions, numberOfCalls, 0, 0,we,(L-b)/2, memory);
               // memory[W][L-b]= lw1;
                lw2 =f_2(W, b , partitions, numberOfCalls, 0, 0,we,b/2, memory);
                //memory[W][b]= lw2;
            lw = std::min(lw, lw1+ lw2);
            
            }
           
    mini= std::min(waste, std::min(lw, wl));
     memory[W][L]=mini;   
    return mini;
}

int recursiveMethod(int W, int L, bool** partitions, int* numberOfCalls){
    int a =0;
    int b =0;
    int waste;
    waste = f_1(W, L ,partitions, numberOfCalls, a, b, W/2, L/2);
	return waste; // this is a dummy return value. YOU SHOULD CHANGE THIS!
}

int memoizationMethod(int W, int L, bool** partitions, int* numberOfCalls){
    int a =0;
    int b =0;
    int waste;
    int** memory = new int*[600];
	for(int i = 0; i < 600; i++){
		memory[i] = new int[600];
		for (int j = 0; j < 600; j++){
			memory[i][j] = -1;
		}
	}
    waste = f_2(W, L ,partitions, numberOfCalls, a, b, W/2, L/2, memory);
    (*numberOfCalls)++;
	return waste; // this is a dummy return value. YOU SHOULD CHANGE THIS!
}

int bottomUpMethod(int W, int L, bool** partitions, int* numberOfIterations){
    int i=0, j=0;
    int i_mini=360000, j_mini=360000;
    int memory[600][600];
    if(partitions[W][L]==1){return 0;}
    for(int i=0; i<= W; i++){
        for(int j=0; j<= L ;j++){
            if(partitions[i][j]==1){
                memory[i][j]=0;
            }
            else{
                memory[i][j]=i*j;
            }
        }
    }
    for(int i=1; i<= W; i++){
        for(int j=1; j<= L; j++){
            if(memory[i][j]== i*j){
                i_mini= i*j;
                j_mini= i*j;
                for(int c= i; c>0; c--){
                    i_mini = std::min(i_mini, memory[i-c][j]+memory[c][j]);
                    (*numberOfIterations)++;
                }
                for(int d =j; d>0; d--){
                    j_mini = std::min(j_mini, memory[i][j-d]+memory[i][d]);
                    (*numberOfIterations)++;
                }
                memory[i][j]= std::min(j_mini, i_mini);
            }
        }
    }
	return memory[W][L]; // this is a dummy return value. YOU SHOULD CHANGE THIS!
}
