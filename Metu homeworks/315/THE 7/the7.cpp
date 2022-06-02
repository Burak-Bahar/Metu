#include "the7.h"
using namespace std;

int minDistance(int dist[], bool sptSet[], int V)
{
  
    // Initialize min value
    int min = INT_MAX, min_index;

    for (int v = 0; v < V; v++)
        if (sptSet[v] == false && dist[v] <= min){
            min = dist[v]; 
            min_index = v;
        }

    return min_index;
}

void printPath(int parent[], int j,std::vector<int>& path)
{
    // Base Case : If j is source
    if (parent[j]==-1)
        return;
    printPath(parent, parent[j],path);
    path.push_back(j);
    
        
}
void FindRoute(int n, std::vector<Road> roads, int s, int d, int x, int y) {
  // You can change these variables. These are here for demo only.
  std::vector<int> path;
  int cost = INT_MAX;
  int size = roads.size();
  int graph[n][n] = {0};
  int i, j, mini;
    for (i = 0; i < n; i++){
        for(int a= 0; a < size ; a++){
            graph[roads[a].endpoints.first][roads[a].endpoints.second] = roads[a].time ;
            graph[roads[a].endpoints.second][roads[a].endpoints.first] = roads[a].time ;
            if(roads[a].endpoints.first == i){
                graph[i][roads[a].endpoints.second] = roads[a].time ;
            }
             if(roads[a].endpoints.second == i){
                graph[i][roads[a].endpoints.first] = roads[a].time ;
            }
        for(j=0; j< n ; j++){
            if( graph[i][j] == 0){
                graph[i][j] = INT_MAX;
            }
        }
        graph[i][i]= 0;
        }
    }
    
    int dist[n]; 
    int parents[n];
    int parentx_y[n];
    int parenty_x[n];

    bool sptSet[n]; 

    for (int i = 0; i < n; i++){
        parents[i] = -1; dist[i] = INT_MAX;
        sptSet[i] = false;
    }

    // Distance of source vertex from itself is always 0
    dist[s] = 0;
 
 
    // Find shortest path for all vertices
    for (int count = 0; count < n - 1; count++) {

        int u = minDistance(dist, sptSet, n);

        sptSet[u] = true;


        for (int v = 0; v < n; v++){
            if (!sptSet[v] && (dist[u] + graph[u][v]) < dist[v] && graph[u][v] != INT_MAX && (dist[u] + graph[u][v]) >= 0 && dist[u]>=0 && graph[u][v] >= 0 && dist[u]!= INT_MAX)
                parents[v] = u, dist[v] = dist[u] + graph[u][v];
        }
    }
    int s_x = dist[x];
    int s_y = dist[y];

    for (int i = 0; i < n; i++){
        parentx_y[i] = -1; dist[i] = INT_MAX;
        sptSet[i] = false;
    }

    // Distance of source vertex from itself is always 0
    dist[x] = 0;
 
 
    // Find shortest path for all vertices
    for (int count = 0; count < n - 1; count++) {

        int u = minDistance(dist, sptSet, n);

        sptSet[u] = true;


        for (int v = 0; v < n; v++){
            if (!sptSet[v] && (dist[u] + graph[u][v]) < dist[v] && graph[u][v] != INT_MAX && (dist[u] + graph[u][v]) >= 0 && dist[u]>=0 && graph[u][v] >= 0 && dist[u]!= INT_MAX)
                parentx_y[v] = u, dist[v] = dist[u] + graph[u][v];
            
        }
    }
    int x_y = dist[y];
    int x_d = dist[d];
    for (int i = 0; i < n; i++){
        parenty_x[i] = -1; dist[i] = INT_MAX;
        sptSet[i] = false;
    }
    dist[y] = 0;
 
 
    // Find shortest path for all vertices
    for (int count = 0; count < n - 1; count++) {

        int u = minDistance(dist, sptSet, n);

        sptSet[u] = true;


        for (int v = 0; v < n; v++){
            if (!sptSet[v] && (dist[u] + graph[u][v]) < dist[v] && graph[u][v] != INT_MAX && (dist[u] + graph[u][v]) >= 0 && dist[u]>=0 && graph[u][v] >= 0 && dist[u]!= INT_MAX)
                parenty_x[v] = u, dist[v] = dist[u] + graph[u][v];
            
        }
    }
    int y_x = dist[x];
    int y_d = dist[d];

    
    path.push_back(s);
    cost= s_x + x_y + y_d;
    mini = s_y + y_x + x_d;
    if(mini < cost){
        cost = mini;
        printPath(parents, y, path);
        printPath(parenty_x, x, path);
        printPath(parentx_y, d, path);

    }
    else{
        printPath(parents, x, path);
        printPath(parentx_y, y, path);
        printPath(parenty_x, d, path);

    }

 
  // Your output should be like this. You can change this as long as you keep
  // the output format. PrintRange function helps you print elements of
  // containers with iteratos (e.g., std::vector).
  std::cout << cost << " ";
  PrintRange(path.begin(), path.end());
  std::cout << std::endl;
}