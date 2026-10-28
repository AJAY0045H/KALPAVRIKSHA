//time complexity O(n^2)
//space complexity O(n^2)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void displayMatrix(int **matrix,int rowLength, int colLength){
    if(matrix == NULL){
        printf("The matrix is null cannot display.\n");
        return;
    }
    for(int row = 0 ; row < rowLength ; row++){
        for(int col = 0 ; col < colLength ; col++){
            printf("%d ",*(*(matrix + row)+col));
        }
        printf("\n");
    }
    printf("\n");
}

void generateMatrix(int ***matrix, int rowLength, int colLength){
    if(matrix == NULL) {
        printf("The matrix is null cannot generate matrix\n");
        return;
    }
    for(int row = 0 ; row < rowLength ; row++){
        for(int col = 0 ; col < colLength ; col++){
           *(*(*matrix+row)+col) = rand() % 256;
        }
    }
} 

void rotateMatrixClockwise(int **matrix, int rowLength, int colLength){
    if(matrix == NULL || *matrix == NULL){
        printf("Matrix is null cannot rotate it\n");
        exit(1);
    }
    for(int row = 0 ; row < rowLength; row++){
        for(int col = row + 1 ; col < colLength ; col++){
            int *address1 = *(matrix + row) + col;
            int *address2 = *(matrix + col) + row;

            int temp = *address1;
            *address1 = *address2;
            *address2 = temp;
        }
    }

    for(int row = 0 ; row < rowLength ; row++){
        int *left = *(matrix + row);
        int *right = *(matrix + row) + (colLength - 1);

        while(left < right){
            int temp = *left;
            *left = *right;
            *right = temp;
            left++;
            right--;
        }
    }

}
void freeMatrix(int **matrix, int size){
    if(matrix == NULL) return;
    for(int row = 0 ; row < size ; row++){
        free(*(matrix+row));
    }
    free(matrix);
}
void applySmoothingFilter(int **matrix, int rowLength, int colLength){
    int directions[][2] = {
        {-1,-1},
        {-1,0},
        {-1,1},
        {0,1},
        {1,1},
        {1,0},
        {1,-1},
        {0,-1},
        {0,0}
    }; 
    int *prevRow = malloc(colLength*sizeof(int));
   
    if(prevRow == NULL){
        printf("Memory allocation failed\n");
        freeMatrix(matrix,rowLength);
        exit(1);
    }
    
    for(int row = 0 ; row < rowLength ; row++){
        int left = 0;

        for(int col = 0 ; col < colLength ; col++){
            int org = *(*(matrix + row)+col);
            int count = 0;
            int sum = 0;//*(*(matrix+row)+col);
            for(int dirIndex = 0 ; dirIndex <= 8; dirIndex++){
                int dx = row + directions[dirIndex][0];
                int dy = col + directions[dirIndex][1];
                if(dx >= 0 && dy >= 0 && dx < rowLength && dy < colLength){
                    if(directions[dirIndex][0] == -1){
                        sum += *(prevRow + dy);
                    }
                    else if(directions[dirIndex][0] == 0 && directions[dirIndex][1] == -1){
                        sum += left;
                    }
                    else{
                        sum += *(*(matrix+dx)+dy);
                    }
                    count++;
                }
            }
            *(*(matrix + row) + col) = sum / count;
            if(col > 0){
                *(prevRow + col - 1) = left;
            }
            left = org;
        }
        *(prevRow + colLength - 1) = left;
    }
    free(prevRow);
}
int main(){
    int size;
    srand(time(NULL)); 

    while(1){
        printf("Enter matrix size (2 - 10):");
        if(scanf("%d",&size) != 1){
            int ch;
            while((ch = getchar()) != '\n' && ch != EOF);
            printf("Enter a valid Integer\n");
            continue;
        }
        else{
            if(size < 2 || size > 10){
                printf("The valid range is (2 - 10)\n");
                continue;
            }
            break;
        }
    }
    int **matrix = (int**) malloc(size * sizeof(int *));
    if(matrix == NULL){
        printf("Memory allocation failed.\n");
        return 1;
    }

    for(int row = 0 ; row < size ; row++){
        *(matrix + row) = (int *)malloc(size * sizeof(int));
        if(*(matrix + row) == NULL){
            printf("Memory allocation failed\n");
            freeMatrix(matrix,size);
            return 1;
        }
    }
    generateMatrix(&matrix, size, size);
    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrix,size,size);

    printf("Matrix after 90 degree clockwise Rotation:\n");
    rotateMatrixClockwise(matrix,size,size);
    displayMatrix(matrix,size,size);

    printf("Matrix after Applying %dx%d Smoothing Filter:\n",3,3);
    applySmoothingFilter(matrix,size,size);
    displayMatrix(matrix,size,size);
    freeMatrix(matrix,size);
}