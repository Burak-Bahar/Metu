#include "the1.h"


//You can add your own helper functions
void swap1(int *arr1, int* arr2, long & swap){
    int tem;
    tem =*arr1;
	*arr1 = *arr2;
	*arr2= tem;
	swap++;
}




int sillySort(int* arr, long &comparison, long & swap , int size) 
{

    int num_of_calls=1;
    int tem;
	if(size < 4){
	    if(size < 2) {return 1;} 
	    else if(size == 2){
	        comparison++;
	        if(arr[0]>arr[1]){
	            tem =arr[0];
	            arr[0] = arr[1];
	            arr[1]= tem;
	            swap++;
	        } 
	        return 1;
	    }
	    else{
	        comparison++;
	        if(arr[0]>arr[1]){
	            comparison++;
	            if(arr[0]> arr[2]){
	                swap1(&arr[0],&arr[2], swap);
	                comparison++;
	                if(arr[0]>arr[1]) {swap1(&arr[0],&arr[1], swap);}
	            }
	            else{ swap1(&arr[0],&arr[1], swap);}
	        }
	        
	        else if(arr[0]>arr[2]){  comparison++; swap1(&arr[0],&arr[2], swap);}
	       
	        else if(arr[1]>arr[2]) {comparison++; swap1(&arr[1],&arr[2], swap);}
	        return 1;
	    }
	}
	else{
	    num_of_calls = num_of_calls + sillySort(arr, comparison, swap, size/2);
	    num_of_calls = num_of_calls + sillySort(&arr[size/4], comparison, swap, size/2);
	    num_of_calls = num_of_calls + sillySort(&arr[size/2], comparison, swap, size/2);
	    num_of_calls = num_of_calls + sillySort(arr, comparison, swap, size/2);
	    num_of_calls = num_of_calls + sillySort(&arr[size/4], comparison, swap, size/2);
	    num_of_calls = num_of_calls + sillySort(arr, comparison, swap, size/2);
	}
	//Your code here
	
	return num_of_calls;
}

void merge(int *arr1, int *arr2, int size,long &comparison , int *hs){
    int k=0,i=0,j=0;
        while(i < size && j < size ){
            comparison++;
            if(arr1[i]<= arr2[j]){
                hs[k]= arr1[i];
                i++;
                k++;
            }
            else{
                hs[k]= arr2[j];
                j++;
                k++;
            }
        }
        while (i < size){
            hs[k] = arr1[i];
            k++;i++;
        }
        while (j < size){
            hs[k] = arr2[j];
            k++; j++;
        }
}
    
int crossMergeSort(int *arr, long &comparison, int size)
{
    long swap=0;
    int *h1= new int(size*2);
	int num_of_calls=1;	
	if(size ==1) {return 1;} 
	else if(size == 2){
	   comparison++;
	   if(arr[0]>arr[1]){
	    swap1(&arr[0],&arr[1], swap);
	   }
	   return 1;
	}
	else {
	    num_of_calls = num_of_calls + crossMergeSort(arr, comparison, size/4);
	    num_of_calls = num_of_calls + crossMergeSort(&arr[size/4], comparison, size/4);
	    num_of_calls = num_of_calls + crossMergeSort(&arr[size/2], comparison,  size/4);
	    num_of_calls = num_of_calls + crossMergeSort(&arr[3*(size/4)], comparison,  size/4);
	    merge(arr, &arr[size/2], size/4 , comparison, h1);
	    merge(&arr[size/4], &arr[3*(size/4)], size/4, comparison, &h1[size]);
	    merge(h1, &h1[size], size/2, comparison ,arr);
	}
	
	// Your code here
	
	return num_of_calls;
	
}




/*int crossHelper(int arr, long &comparison, int size, int& start_index, int temp_arr){
    int numcalls=1;
    if(size==1){
        return 1;
    }
    if(size==2){
        comparison++;
        if(arr[0] > arr[1]){
            int tmp = arr[1];
            arr[1] = arr[0];
            arr[0] = tmp;
        }
        return 1;
    }
    int qtr = size >> 2;
    int hlf = size >> 1;
    int tqtr = qtr+hlf;
    int this_start = start_index;
    start_index += size;
    numcalls+=crossHelper(arr,comparison,qtr,start_index,temp_arr);
    numcalls+=crossHelper(arr+qtr,comparison,qtr,start_index,temp_arr);
    numcalls+=crossHelper(arr+hlf,comparison,qtr,start_index,temp_arr);
    numcalls+=crossHelper(arr+tqtr,comparison,qtr,start_index,temp_arr);
    merge(arr,arr+hlf,temp_arr+this_start,qtr,comparison);
    merge(arr+qtr,arr+tqtr,temp_arr+this_start+hlf,qtr,comparison);
    merge(temp_arr+this_start,temp_arr+this_start+hlf,arr,hlf,comparison);
    return numcalls;
}
int crossMergeSort(int *arr, long &comparison, int size)
{
	
	int num_of_calls;
	int start_index = 0;
	int temp[nineteenth_pw];
	comparison = 0;
	return crossHelper(arr,comparison,size,start_index,temp);
	// Your code here
	
	//return num_of_calls;
	
}
    */
    
    
    