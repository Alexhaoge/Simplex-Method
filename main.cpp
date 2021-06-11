#include <bits/stdc++.h>
#include <fstream>
#include "lp.h"
#include "ssimplex.h"
#include "bigm.h"
using namespace std;

void printMatrix(double* a, int row, int col) {
    cout << ">>>>>>>>>>>>>>>>>>>>>>" << endl;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++)
            cout << a[i*col + j] << "\t";
        cout << endl;
    }
    cout << "<<<<<<<<<<<<<<<<<<<<<<" << endl;
}

void testSimpleLP() {
    double C0[3]={-1,-14,-6};
    double b0[4]={4,2,3,6};
    double A0[12]={1,1,1,1,0,0,0,0,1,0,3,1};
    double* res;
    res = (double *)malloc(sizeof(double) * 5 * 8);
    flush(cout);
    SimpleLPtoMatrix(res, 3, 4, C0, A0, b0);
    printMatrix(res, 5, 8);
    SSimplex ss(res, 7, 4);
    ss.solve(true);
    ss.printAns();
    free(res);
}

void testGeneralLP() {
    ifstream f("E:\\JupyterLab\\Simplex-Method\\sample\\1.txt", ios::in);
    GeneralLP glp;
    f >> glp;
    cout << glp << endl;
    StandardLP slp(glp);
    cout << slp << endl;
    double *mat = (double *)malloc(sizeof(double) * (slp.getConstrainNum() + 1) * (slp.varNum + 1));
    slp.toMatrix(mat);
    printMatrix(mat, slp.getConstrainNum() + 1, slp.varNum + 1);
    Simplex_BigM sbm(mat, slp.varNum, slp.getConstrainNum());
    sbm.solve(true);
    sbm.printAns();
    free(mat);
}

int main(){
    cout << "TASK 1:" << endl;
    testSimpleLP();
    cout << endl << "TASK 2:" << endl;
    try {
        testGeneralLP();
    } catch (const std::exception& e) {
        std::cerr << e.what() << endl;
    }
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
