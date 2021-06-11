#include "ssimplex.h"
#include <float.h>
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
    for (int i = 1; i <= consNum; i++)
        base.push_back(i);
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

bool SSimplex::solve(bool verbose) {
    int in, out;
    if (verbose) printMatrix();
    while (1) {
        int res = pivot(in, out);
        if (res == 0) break;
        else if (res == -1) return false;
        if (verbose)
            cout << "in: " << in << ", out: " << out << endl;
        Gaussian(in, out);
        if (verbose)    printMatrix();
    }
    return true;
}

int SSimplex::pivot(int &in, int &out) {
    double min_in = DBL_MAX;
    in = -1;
    for (int j = 0; j < varNum; j++)
        if (!base[j] && min_in > mat[0][j])
            min_in = mat[0][j], in = j;
    if (min_in >= 0) return 0;
    double min_out = DBL_MAX;
    out = -1;
    for (int i = 1; i <= consNum; i++)
        if (mat[i][in] > 0) {
            double tmp = mat[i][varNum] / mat[i][in];
            if (min_out > tmp)
                min_out = tmp, out = i;
        }
    if (out == -1) return -1;
    for (int j = 0; j < varNum; j++)
        if (base[j] > 0 && base[j] == out) {
            out = j;
            break;
        }
    return 1;
}

void SSimplex::Gaussian(int &in, int &out) {
    int x = base[out];
    double scale = mat[x][in];
    for (int i = 0; i <= varNum; i++)
        mat[x][i] /= scale;
    for (int i = 0; i <= consNum; i++)
        if (i != x && mat[i][in] != 0){
            scale = mat[i][in];
            for (int j = 0; j <= varNum; j++) {
                mat[i][j] -= scale * mat[x][j];
            }
        }
    base[out] = 0, base[in] = x;
}
