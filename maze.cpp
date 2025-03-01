#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_SIZE 50

// Structure to represent a point in the maze
typedef struct {
    int row;
    int col;
} Point;

// Maze structure
typedef struct {
    char grid[MAX_SIZE][MAX_SIZE];
    int weights[MAX_SIZE][MAX_SIZE];
    int rows;
    int cols;
    Point start;
    Point end;
} Maze;

// Direction arrays for 4-directional movement
int dr[] = {-1, 0, 1, 0};
int dc[] = {0, 1, 0, -1};

// Function to check if a position is valid within the maze
bool isValid(Maze *maze, int row, int col) {
    return (row >= 0 && row < maze->rows && 
            col >= 0 && col < maze->cols && 
            maze->grid[row][col] != '#');
}

// Function to read a maze from user input
void readMaze(Maze *maze) {
    printf("Enter the number of rows and columns: ");
    scanf("%d %d", &maze->rows, &maze->cols);
    
    printf("\nEnter the maze matrix:\n");
    printf("Use '#' for walls, 'S' for start, 'E' for end, and '1' for paths (will ask for weight).\n\n");

    getchar(); 
	// To clear the newline character from the buffer

    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            scanf(" %c", &maze->grid[i][j]);
            
            if (maze->grid[i][j] == 'S') {
                maze->start.row = i;
                maze->start.col = j;
                maze->weights[i][j] = 0;  
				// Start has zero weight  
            } else if (maze->grid[i][j] == 'E') {
                maze->end.row = i;
                maze->end.col = j;
                maze->weights[i][j] = 0;  
				// End has zero weight  
            } else if (maze->grid[i][j] == '#') {
                maze->weights[i][j] = INT_MAX;  
				// Wall has infinite weight
            } else if (maze->grid[i][j] == '1') {
                printf("Enter weight for position (%d, %d): ", i, j);
                scanf("%d", &maze->weights[i][j]);
            } else {
                maze->weights[i][j] = 1;  
				// Default weight for other paths
            }
        }
    }
}

// Function to display the maze
void displayMaze(Maze *maze) {
    printf("\nMaze (%d x %d):\n", maze->rows, maze->cols);
    
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            printf("%c ", maze->grid[i][j]);
        }
        printf("\n");
    }
}

// Dijkstra's algorithm to find the shortest path in the maze
bool findShortestPath(Maze *maze, int *totalCost) {
    int dist[MAX_SIZE][MAX_SIZE];
    Point prev[MAX_SIZE][MAX_SIZE];
    bool visited[MAX_SIZE][MAX_SIZE];
    
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            dist[i][j] = INT_MAX;
            visited[i][j] = false;
            prev[i][j].row = prev[i][j].col = -1;
        }
    }
    
    dist[maze->start.row][maze->start.col] = 0;
    
    for (int count = 0; count < maze->rows * maze->cols; count++) {
        int minDist = INT_MAX;
        Point current;
        
        for (int i = 0; i < maze->rows; i++) {
            for (int j = 0; j < maze->cols; j++) {
                if (!visited[i][j] && dist[i][j] < minDist) {
                    minDist = dist[i][j];
                    current.row = i;
                    current.col = j;
                }
            }
        }
        
        if (minDist == INT_MAX) break;
        
        visited[current.row][current.col] = true;
        
        if (current.row == maze->end.row && current.col == maze->end.col) {
            *totalCost = dist[maze->end.row][maze->end.col];  
			// Store the total path cost
            
            Point path = maze->end;
            while (path.row != maze->start.row || path.col != maze->start.col) {
                if (maze->grid[path.row][path.col] != 'E') {
                    maze->grid[path.row][path.col] = '*';
                }
                path = prev[path.row][path.col];
            }
            return true;
        }
        
        for (int i = 0; i < 4; i++) {
            int newRow = current.row + dr[i];
            int newCol = current.col + dc[i];
            
            if (isValid(maze, newRow, newCol)) {
                int weight = maze->weights[newRow][newCol];
                
                if (!visited[newRow][newCol] && dist[current.row][current.col] + weight < dist[newRow][newCol]) {
                    dist[newRow][newCol] = dist[current.row][current.col] + weight;
                    prev[newRow][newCol] = current;
                }
            }
        }
    }
    
    return false;
}


int main() {
    Maze maze;
    int totalCost = 0; 

    
    printf("Maze Solver using Dijkstra's Algorithm with User-Defined Weighted Paths\n");
    printf("--------------------------------------------------------\n\n");
    
    readMaze(&maze);
    
    printf("\nOriginal Maze:");
    displayMaze(&maze);
    
    if (findShortestPath(&maze, &totalCost)) {
        printf("\nShortest Path:");
        displayMaze(&maze);
        
        printf("\nTotal Path Cost: %d\n", totalCost);
		 // Displaying the total path cost
        
        printf("\nLegend:\n");
        printf("# - Wall\n");
        printf("S - Start\n");
        printf("E - End\n");
        printf("* - Shortest path\n");
        printf("1 - Path with user-defined weight\n");
    } else {
        printf("\nNo path exists from start to end.\n");
    }
    
    return 0;
}
