//Lannie Dalton Hough III
//Implementation of the Diamond-Square algorithm

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int randSeeded = 0;

inline int randomNum(int maxRand) {
    return rand() % (maxRand+1);
}

//Array param in diamondStep() and squareStep() used to pass arbitrary 2D array size
void diamondStep(void *array, int step, int size) {
    //Cast to 2D array
    float (*arr)[size][size] = (float (*)[size][size]) array;
    //Number of diamonds to calculate in this step
    //Step 1 is 1 diamond, step 2 is 4 diamonds, step 3 is 16 diamonds
    //+0.5 is to stop rounding errors
    int numDiamonds = (int) (pow(2, step-1) + 0.5);
    int numRows = (int) (sqrt(numDiamonds) + 0.5);
    printf("Step: %i\n", step); //1, 3, 5, etc.
    printf("Number diamonds: %i\n", numDiamonds); //1, 4, 16
    printf("NumRows: %i\n", numRows); //1, 2, 4
    int distBetween = size / numRows;
    for (int i = 0; i < numRows; i++) { //Across columns of diamonds: Left->Right
        for (int j = 0; j < numRows; j++) { //Down column of diamonds: Top->Bottom
            
            (*arr)[(i*distBetween) + (distBetween/2)][(j*distBetween) + (distBetween/2)] = 5555.55f;
        }
    }
    
}

void squareStep(void *array, int step, int size) {
    float (*arr)[size][size] = (float (*)[size][size]) array;
}

void diamondSquareGenHeightmap(int n, int maxRand, float c1, float c2, float c3, float c4) {
    if (randSeeded == 0) { //If random isn't seeded, seed rand
        srand(time(0));
        randSeeded = 1;
    }
    //+0.5 is to ensure no rounding errors, pow returns double
    //diamond-square algorithm works on 2^n + 1 size arrays
    int size = ((int) (pow(2, n) + 0.5)) + 1;
    printf("Size: %i\n", size);
    float array[size][size];

    //Total number of steps (diamond & square)
    //+0.5 is to ensure no rounding errors, whole number from log/log expression is expected
    //3x3 has 3 steps, 5x5 has 5 steps, 9x9 has 7 steps
    int numSteps = ((int) (log10((size-1) * (size-1)) / log10(2)) + 0.5) + 1; 
    printf("Number of steps: %i\n", numSteps);

    for (int step = 0; step < numSteps; step++) {

        if (step == 0) {
            //Initialize corners on first step
            array[0][0] = c1; //top left
            array[size-1][0] = c2; //top right
            array[0][size-1] = c3; //bottom left
            array[size-1][size-1] = c4; //bottom right
            continue;
        }

        int whichStep = step % 2; //0 is square step, 1 is diamond step
        if (whichStep == 0) {
            squareStep(array, step, size);
        } else {
            diamondStep(array, step, size);
        }
    }


    //Check contents of heightmap
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%f | ", array[i][j]);
        }
        printf("\n");
    }
}

int main() {
    diamondSquareGenHeightmap(3, 5, 0.5f, 0.5f, 0.5f, 0.5f);
    return 0;
}