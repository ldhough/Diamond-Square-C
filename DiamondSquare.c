//Lannie Dalton Hough III
//Implementation of the Diamond-Square algorithm

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int randSeeded = 0;

float randomNum(int maxRand, int timesMaxR) {
    int max = maxRand * timesMaxR;
    int ran = rand() % (max+1);
    //Sign decides whether random change in terrain should be up or down (pos or neg)
    float sign = (rand() % 2) ? -1.0f : 1.0f;
    return ((float) ran / (float) timesMaxR) * sign;
}

//array param in diamondStep() and squareStep() used to pass arbitrary 2D array size
void diamondStep(void *array, int step, int size, float magnitude, int maxR, int timesMaxR) {
    //Cast to 2D array
    float (*arr)[size][size] = (float (*)[size][size]) array;
    
    //Number of diamonds to calculate in this step
    //Step 1 is 1 diamond, step 2 is 4 diamonds, step 3 is 16 diamonds
    int numDiamonds = 1 << step-1;
    int numRows = (int) (sqrt(numDiamonds) + 0.5);
    
    //Distance between diamonds: For 1, 2, 3 if 1 and 3 are diamonds distBetween is 2
    //ex: For first step though there is only one diamond, distBetween assumes grid continues
    int distBetween = size / numRows;
    int db2 = distBetween / 2; //Used in expressions, describes offset of square points & offset of diamonds from edge
    
    for (int i = 0; i < numRows; i++) { //Across columns of diamonds: Left->Right
        for (int j = 0; j < numRows; j++) { //Down column of diamonds: Top->Bottom
            //i1 and j1 calculate center of diamond position
            int i1 = (i*distBetween) + db2;
            int j1 = (j*distBetween) + db2;
            float upLeft, upRight, bottomLeft, bottomRight;
            upLeft = (*arr)[i1-db2][j1-db2];
            upRight = (*arr)[i1+db2][j1-db2];
            bottomLeft = (*arr)[i1-db2][j1+db2];
            bottomRight = (*arr)[i1+db2][j1+db2];
            float avg = (upLeft + upRight + bottomLeft + bottomRight) / 4;
            (*arr)[i1][j1] = avg + (randomNum(maxR, timesMaxR) * magnitude);
        }
    }
    
}

void squareStep(void *array, int step, int size, int *prevRows, int *prevDist, float magnitude, int maxR, int timesMaxR) {
    float (*arr)[size][size] = (float (*)[size][size]) array;

    //Number of squares to calculate in this step
    //For first square step, numRows is 3, otherwise calculated from previous square step numRows
    //according to formula numRows = (prevRows) + (prevRows-1)
    *prevRows = (step == 2) ? 3 : (*prevRows) + ((*prevRows)-1);
    int *newDist = prevDist;
    
    *newDist = (step == 2) ? size - 1 : *prevDist / 2;
    int halfDist = *newDist / 2; //Used for offset to descriptor points that avg is taken from
    int numBigRows = *prevRows / 2; //If there are 5 rows with square points, 2 are big
    int numSmallRows = *prevRows - numBigRows; //If there are 5 rows w/ square points, 3 are big
    int sizeBigRow = numSmallRows;
    int sizeSmallRow = numBigRows;
    int smallOffset = (size / sizeSmallRow) / 2;
    
    //Being on a big or small col determines offset for start pos on that col
    for (int i = 0; i < *prevRows; i++) { //Across columns
        int smallOrBig = (i % 2) == 0; //If true, small, else big
        for (int j = 0; j < ((smallOrBig) ? sizeSmallRow : sizeBigRow); j++) { //i%2==0 then small col, else big col
            int jOffset = (smallOrBig) ? smallOffset : 0;
            //Use i1, j1 to calculate center of square position
            int i1, j1;
            i1 = i * smallOffset;
            j1 = (j * (*newDist)) + jOffset;
            float up, bottom, left, right;
            //Points on square step on edges of grid only have three descriptor points unless you wrap around
            up = (*arr)[i1][((j1 - halfDist) < 0) ? ((size - 1) - halfDist) : (j1 - halfDist)]; //just size - halfDist might be more intuitive but lands on an unsolved point
            bottom = (*arr)[i1][((j1 + halfDist) > (size-1)) ? (halfDist) : (j1 + halfDist)];
            left = (*arr)[((i1 - halfDist) < 0) ? ((size - 1) - halfDist) : (i1 - halfDist)][j1];
            right = (*arr)[((i1 + halfDist) > (size-1)) ? (halfDist) : (i1 + halfDist)][j1];
            float avg = (up + bottom + left + right) / 4;
            (*arr)[i1][j1] = avg + (randomNum(maxR, timesMaxR) * magnitude);
        }
    }
}

void diamondSquareGenHeightmap(void *arr, int size, int maxRand, int timesMaxR, float c1, float c2, float c3, float c4) {
    //Cast to 2D float array
    float (*array)[size][size] = (float (*)[size][size]) arr;

    if (randSeeded == 0) { //If random isn't seeded, seed rand
        srand(time(0));
        randSeeded = 1;
    }

    //Total number of steps (diamond & square)
    //+0.5 is to ensure no rounding errors, whole number from log/log expression is expected
    //3x3 has 3 steps, 5x5 has 5 steps, 9x9 has 7 steps
    int numSteps = ((int) (log10((size-1) * (size-1)) / log10(2)) + 0.5) + 1; 

    //Decreases at each step to smooth terrain generation
    float magnitude = 1.0f;
    float magChange = magnitude / (float) numSteps;

    int prevSquareStepRows = 0;
    int prevSquareStepDist = 0;
    for (int step = 0; step < numSteps; step++) {

        if (step == 0) {
            //Initialize corners on first step
            (*array)[0][0] = c1; //top left
            (*array)[size-1][0] = c2; //top right
            (*array)[0][size-1] = c3; //bottom left
            (*array)[size-1][size-1] = c4; //bottom right
            continue;
        }

        int whichStep = step % 2; //0 is square step, 1 is diamond step
        if (whichStep == 0) {
            //printf("Attempting square step\n");
            squareStep((*array), step, size, &prevSquareStepRows, &prevSquareStepDist, 1.0f, maxRand, timesMaxR);
        } else {
            //printf("Attempting diamond step\n");
            diamondStep((*array), step, size, 1.0f, maxRand, timesMaxR);
        }
        magnitude -= magChange;
    }
}

int main() {
    int n = 10;
    //diamond-square algorithm works on 2^n + 1 size arrays
    int size = (1 << n) + 1;
    float array[size][size];
    diamondSquareGenHeightmap(array, size, 1, 1000, 0.5f, 0.5f, 0.5f, 0.5f);

    //Check contents of heightmap
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%f | ", array[i][j]);
        }
        printf("\n");
    }
    printf("Size: %i\n", size);
    return 0;
}
