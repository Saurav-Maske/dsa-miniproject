#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_SIZE 50

// Structure to represent a point
typedef struct {
    int row, col;
} Point;

// Node for adjacency list
typedef struct Node {
    Point dest;
    int weight;
    struct Node* next;
} Node;

// Graph structure
typedef struct {
    char maze[MAX_SIZE][MAX_SIZE];  // Stores symbols ('S', 'E', '#', or weight)
    int weights[MAX_SIZE][MAX_SIZE]; // Stores numerical weights
    Node* adj[MAX_SIZE][MAX_SIZE];
    int rows, cols;
    Point start, end;
} Graph;

// Priority Queue Node
typedef struct {
    Point point;
    int cost;
} PQNode;

// Min-Heap Priority Queue for Dijkstra
typedef struct {
    PQNode heap[MAX_SIZE * MAX_SIZE];
    int size;
} MinHeap;

// Directions: up, right, down, left
int dr[] = {-1, 0, 1, 0};
int dc[] = {0, 1, 0, -1};

// Function to initialize graph
void initGraph(Graph* graph, int rows, int cols) {
    graph->rows = rows;
    graph->cols = cols;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            graph->adj[i][j] = NULL, graph->weights[i][j] = -1;
}

// Function to create adjacency list node
Node* createNode(int row, int col, int weight) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->dest.row = row;
    newNode->dest.col = col;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// Function to add an edge to the graph
void addEdge(Graph* graph, int srcRow, int srcCol, int destRow, int destCol, int weight) {
    Node* newNode = createNode(destRow, destCol, weight);
    newNode->next = graph->adj[srcRow][srcCol];
    graph->adj[srcRow][srcCol] = newNode;
}

// Function to read the maze and construct the graph
void readGraph(Graph* graph) {
    printf("Enter the number of rows and columns: ");
    scanf("%d %d", &graph->rows, &graph->cols);
    initGraph(graph, graph->rows, graph->cols);

    printf("\nEnter the maze:\n");
    printf("Use '#' for walls, 'S' for start, 'E' for end, and '1' for paths (will ask for weight).\n\n");

    getchar(); 
	// Consume newline
    for (int i = 0; i < graph->rows; i++) {
        for (int j = 0; j < graph->cols; j++) {
            scanf(" %c", &graph->maze[i][j]);

            if (graph->maze[i][j] == 'S') {
                graph->start.row = i;
                graph->start.col = j;
                graph->weights[i][j] = 0; 
				 // Start has zero weight
            } else if (graph->maze[i][j] == 'E') {
                graph->end.row = i;
                graph->end.col = j;
                graph->weights[i][j] = 0; 
				 // End has zero weight
            } else if (graph->maze[i][j] == '1') {
                printf("Enter weight for position (%d, %d): ", i, j);
                int weight;
                scanf("%d", &weight);
                graph->weights[i][j] = weight;
                graph->maze[i][j] = weight + '0';
				 // Store weight as character
            } else {
                graph->weights[i][j] = (graph->maze[i][j] == '#') ? INT_MAX : 1;
            }
        }
    }

    // Build adjacency list graph
    for (int i = 0; i < graph->rows; i++) {
        for (int j = 0; j < graph->cols; j++) {
            if (graph->maze[i][j] != '#') {
                for (int d = 0; d < 4; d++) {
                    int newRow = i + dr[d], newCol = j + dc[d];
                    if (newRow >= 0 && newRow < graph->rows && newCol >= 0 && newCol < graph->cols && graph->maze[newRow][newCol] != '#') {
                        int weight = graph->weights[newRow][newCol];
                        addEdge(graph, i, j, newRow, newCol, weight);
                    }
                }
            }
        }
    }
}

// Function to display the weighted maze
void displayMaze(Graph* graph) {
    printf("\nMaze (%d x %d):\n", graph->rows, graph->cols);
    for (int i = 0; i < graph->rows; i++) {
        for (int j = 0; j < graph->cols; j++) {
            printf("%c ", graph->maze[i][j]);
        }
        printf("\n");
    }
}

// Dijkstra's Algorithm to find the shortest path
bool findShortestPath(Graph* graph, int* totalCost) {
    int dist[MAX_SIZE][MAX_SIZE];
    Point prev[MAX_SIZE][MAX_SIZE];
    bool visited[MAX_SIZE][MAX_SIZE] = {false};

    for (int i = 0; i < graph->rows; i++)
        for (int j = 0; j < graph->cols; j++)
            dist[i][j] = INT_MAX, prev[i][j].row = prev[i][j].col = -1;

    dist[graph->start.row][graph->start.col] = 0;
    
    MinHeap heap = {0};
    heap.heap[heap.size++] = (PQNode){graph->start, 0};

    while (heap.size > 0) {
        PQNode node = heap.heap[--heap.size];
        Point u = node.point;
        if (visited[u.row][u.col]) continue;
        visited[u.row][u.col] = true;

        if (u.row == graph->end.row && u.col == graph->end.col) {
            *totalCost = dist[u.row][u.col];

            // Mark shortest path
            Point path = graph->end;
            while (!(path.row == graph->start.row && path.col == graph->start.col)) {
                if (graph->maze[path.row][path.col] != 'S' && graph->maze[path.row][path.col] != 'E') {
                    graph->maze[path.row][path.col] = '*';
                }
                path = prev[path.row][path.col];
            }
            return true;
        }

        for (Node* neighbor = graph->adj[u.row][u.col]; neighbor; neighbor = neighbor->next) {
            int newDist = dist[u.row][u.col] + neighbor->weight;
            if (newDist < dist[neighbor->dest.row][neighbor->dest.col]) {
                dist[neighbor->dest.row][neighbor->dest.col] = newDist;
                prev[neighbor->dest.row][neighbor->dest.col] = u;
                heap.heap[heap.size++] = (PQNode){neighbor->dest, newDist};
            }
        }
    }
    return false;
}


int main() {
    Graph graph;
    int totalCost = 0;

    printf("Maze Solver \n");
    readGraph(&graph);

    printf("\nOriginal Weighted Maze:");
    displayMaze(&graph);

    if (findShortestPath(&graph, &totalCost)) {
        printf("\nShortest Path Found! Total Cost: %d\n", totalCost);
        printf("\nMaze with Shortest Path:");
        displayMaze(&graph);
    } else {
        printf("\nNo path exists from start to end.\n");
    }

    return 0;
}
