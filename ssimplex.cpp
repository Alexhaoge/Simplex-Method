#include "ssimplex.h"
#include <cstring>
#include <iostream>
#include <cstdlib>
using namespace std;

// 默认最后consNum个变量为基变量
SSimplex::SSimplex(double* _mat, int var, int cons) {
    varNum = var, consNum = cons;
    mat = (double **)malloc(sizeof(double *) * (consNum + 1));
    for (int i = 0, _sz=sizeof(double)*(varNum+1); i <= consNum; i++) {
        mat[i] = (double *)malloc(_sz);
        memcpy(mat[i], &_mat[i*(varNum+1)], _sz);
    }
    mat[0][varNum] = 0;
    for (int i = 0; i < varNum - consNum; i++)
        base.push_back(0);
    for (int i = varNum - consNum; i < varNum; i++)
        base.push_back(i - varNum + 1);
}
SSimplex::~SSimplex() {
    for (int i = 0; i <= consNum; i++) free(mat[i]);
    free(mat);
}

void SSimplex::printMatrix() {
    cout << ">>>>>>>>>>>>>>>>>>>>>>" << endl;
    for (int i = 0; i <= consNum; i++) {
        for (int j = 0; j <= varNum; j++)
            cout << mat[i][j] << "\t";
        cout << endl;
    }
    cout << "<<<<<<<<<<<<<<<<<<<<<<" << endl;
}

double SSimplex::getAns(vector<double> &x) {
    x.clear();
    for (int i = 0; i < varNum; i++) {
        x.push_back(base[i] ? mat[base[i]][varNum] : 0);
    }
    return mat[0][varNum];
}

void SSimplex::printAns() {
    vector<double> x;
    double ans = getAns(x);
    cout << "Minimum: " << ans << "\t";
    for (int i = 0; i < varNum; i++) {
        cout << "x" << i+1 << "=" << x[i] << "\t";
    }
    cout << endl;
}

double SSimplex::solve(bool verbose) {
    int in, out, consID;
    if (verbose) printMatrix();
    while (pivot(in, out, consID)) {
        if (verbose) {
            cout << "in: " << in << ", out: " << out << ", constraint ID: " << consID << endl;
        }
        Gaussian(in, out);
    }
    return mat[0][varNum];
}

bool SSimplex::pivot(int &in, int &out, int &consID) {
    return true;
}

void SSimplex::Gaussian(int &in, int &out) {
    
}
