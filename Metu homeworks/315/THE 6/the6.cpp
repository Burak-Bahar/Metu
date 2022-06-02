#include "the6.h"
// Don't add any additional includes
using namespace std;
/*
  N: Number of racers
  updates: Updates you have heard
*/
void adjlist( std::vector<int>* arr, std::vector<std::pair<int, int>> updates, int N, int M){
    for(int i = 0; i<N ; i++){
        for(int j=0 ; j < M ; j++){
            if( updates[j].first== i){
                arr[i].push_back(updates[j].second);
            }
        }
    }
   /* for(int i = 0; i<N ; i++){
        cout << i<< "  ";
        for(int j=0 ; j < arr[i].size() ; j++){
            cout << arr[i][j] << " ";
        }
        cout << endl;
    } */
    return;
}


void topologicalSortUtil(int N,  bool visited[], stack<int>& Stack, std::vector<int>* arr)
{
    // Mark the current node as visited.
    visited[N] = true;

    // Recur for all the vertices
    // adjacent to this vertex
    for (unsigned int i=0; i< arr[N].size(); i++){
        if (visited[arr[N][i]] == false){
            topologicalSortUtil(arr[N][i], visited, Stack, arr);
        }
    }
    // Push current vertex to stack
    // which stores result
    
    Stack.push(N);
}

vector<char> color;
vector<int> parent;
vector<int> rev;
int start=-1, end2;
bool dfs(int v, std::vector<int>* arr){
    color[v]= 1;
    for(int u : arr[v]){
        if(color[u]==0){
            parent[u] = v;
            if(dfs(u, arr)){
                return true;
            }
        }
        else if(color[u] == 1){
            end2 = v;
            start = u;
            return true;
        }
    }
    color[v]=2;
    return false;
}
bool cycle( std::vector<int>* arr, int N){
    color.assign(N, 0);
    parent.assign(N, -1);
    
    for(int i=0; i<N; i++){
        if(color[i]== 0 && dfs(i, arr) ){
            break;
        }
    }
    
    if(start == -1){
        return true;
    }
    else{
        vector<int> cycle;
        cycle.push_back(start);
        for(int i = end2; i != start; i = parent[i]){
            cycle.push_back(i);
        }
        cycle.push_back(start);
        
        
        int a =cycle.size();
        //reverse()
        for( int i=a-1; i>= 0; i--){
                rev.push_back(cycle[i]);
        }
    }
    return false;
}

std::pair<bool, std::vector<int>> RaceResult(int N, std::vector<std::pair<int, int>> updates) {
    int M = updates.size();
    std::vector<int> result;
    bool res;

    std::vector<int>* arr= new std::vector<int>[N];
    adjlist( arr, updates, N, M);
    stack<int> Stack;
    
    res = cycle(arr, N);
    if( res == false ){
        return {res, rev};
    }
    bool* visited = new bool[N];
    for (int i = 0; i < N; i++){
        visited[i] = false;}
    for (int j = 0; j < N; j++){
        if (visited[j] == false){
            topologicalSortUtil(j, visited, Stack, arr);
        }
    }
    // Print contents of stack
    while (Stack.empty() == false) {
        
        result.push_back(Stack.top());
        Stack.pop();
        
    }
  
  return {res, result}; // This is a dummy return value. YOU SHOULD CHANGE THIS!
}
