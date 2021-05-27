#include <bits/stdc++.h>
#include "lp.h"
#include "ssimplex.h"
using namespace std;

void printMatrix(double** a, int row, int col) {
    printf(">>>>>>>>>>>>>>>>>>>>>>\n");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++)
            printf("%lf\t", a[i][j]);
        printf("\n");
    }
    printf("<<<<<<<<<<<<<<<<<<<<<<\n");
}

int main(){
    double C0[3]={-1,-14,-6};
    double b0[4]={4,2,3,6};
    double A0[4][3]={{1,1,1},{1,0,0},{0,0,1},{0,3,1}};
    double** res;
    printMatrix(res, 4, 3);
    flush(cout);
    SimpleLPtoMatrix(res, 3, 4, C0, (double **)A0, b0);
    printMatrix(res, 5, 8);
    return 0;
}
///* Variables */
// var x1 >= 0;
// var x2 >= 0;
// var x3 >= 0;
///* Object function */
// minimize z: -x1 - 14*x2 - 6*x3;
///* Constrains */
// s.t. con1: x1 + x2 + x3 <= 4;
// s.t. con2: x1  <= 2;
// s.t. con3: x3  <= 3;
// s.t. con4: 3*x2 + x3  <= 6;
// end;
/////////////////////////////////////
