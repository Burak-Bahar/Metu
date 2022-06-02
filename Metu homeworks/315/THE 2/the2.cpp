#include "the2.h"
#include <iostream>
//You may write your own helper functions here
double diff(unsigned short *arr, unsigned short *arrr){
    double i = arrr - arr;
    if(i < 0){
        return i+ (2*(-i));
    }
    else return i;
}
void swap1(unsigned short *arr, unsigned short *arrr ,long &swap, double & avg_dist,  double & max_dist){
    double distance;
    unsigned short tem = *arr;
    *arr = *arrr;
    *arrr = tem;
    distance = diff(arr, arrr);
    avg_dist = avg_dist*(swap);
    swap++;
    avg_dist = avg_dist+distance;
    avg_dist = avg_dist/swap;
    if(distance > max_dist){ max_dist = distance; }
}

 
int partition(unsigned short *arr, int size, long &swap, double & avg_dist,  double & max_dist){
    unsigned short x= arr[size-1];
    int i= -1;
    for(int j =0; j <= size-2 ; j++){
        if(arr[j] >= x){
            i++;
            swap1( &arr[i], &arr[j], swap , avg_dist, max_dist);
        }
    }
    swap1( &arr[i+1], &arr[size-1], swap , avg_dist, max_dist);
    return i+1;
}

int hoar(unsigned short *arr, int size, long &swap, double & avg_dist,  double & max_dist){
    unsigned short x = arr[(size-1)/2];
    int i = -1;
    int j = size;
    while(true){
        do{
            j--;
        }while(arr[j]<x);
        do{
            i++;
        }while(arr[i]>x);
        if(i<j){
            swap1(&arr[i], &arr[j], swap , avg_dist, max_dist);
        }
        else {return j;}
    }
    
}

void quickSort(unsigned short* arr, long &swap, double & avg_dist, double & max_dist, bool hoare, int size)
{
    //Your code here
    int P;
    if(size<=1){return;}
    
	if (hoare == 0){

	        P = partition(arr , size, swap , avg_dist, max_dist);
	        quickSort(arr ,swap , avg_dist, max_dist, 0, P);
	        quickSort(&arr[P+1] ,swap , avg_dist, max_dist, 0, size-P-1);}
        
	
	else{
	        P = hoar(arr , size, swap , avg_dist, max_dist);
	        quickSort(arr ,swap , avg_dist, max_dist, 1, P+1);
	        quickSort(&arr[P+1] ,swap , avg_dist, max_dist, 1, size-P-1);
	}
}

void swap2(unsigned short *arr, unsigned short *arrr, long &swap){
    unsigned short tem = *arr;
    *arr = *arrr;
    *arrr = tem;
    swap++;
}

struct returner{
    int L, R;
};
int mini(int a, int b){
    if(a<b) {return a;}
    else {return b;}
}
returner part3way(unsigned short *arr, int size, long &swap, long &comparison ){
    int i=0;
    int j =0;
    int p= size-1;
    returner al;
    while(i<p){
        if(arr[i] > arr[size-1]){
            comparison++;
            swap2(&arr[i], &arr[j], swap);
            i++; j++; 
        }
        else if(arr[i]== arr[size-1]){
            p--;
            comparison = comparison+2;
            swap2(&arr[i], &arr[p], swap);
        }
        else { i++; comparison= comparison+2;}
    }
    int m= mini(p-j, size-p);
    for (int a=0; a< m; a++){
        swap2(&arr[j+a], &arr[size-m+a], swap);
    }
    al.L = j;
    al.R = p-j;
    return al;
}

void quickSort3(unsigned short *arr, long &swap, long &comparison, int size) {
	
    //Your code here
    returner la;
	int P;
    if(size<=1){return;}
    else{
        la = part3way(arr, size, swap, comparison);
        quickSort3(arr, swap, comparison, la.L);
        quickSort3(&arr[size-(la.R)], swap, comparison, la.R);
    }
	
	
	
	
}