#include "ssimplex.h"
#include <cstring>
#include <iostream>

// 默认最后consNum个变量为基变量
SSimplex::SSimplex(double** _a, int var, int cons) {
    varNum = var, consNum = cons;
    a = (double **)malloc(sizeof(double *) * (consNum + 1));
    for (int i = 0, _sz=sizeof(double)*(varNum+1); i <= consNum; i++) {
        a[i] = (double *)malloc(_sz);
        memcpy(a[i], _a[i], _sz);
    }
    a[0][varNum] = 0;
    for (int i = 0; i < varNum - consNum; i++)
        base.push_back(0);
    for (int i = varNum - consNum; i < varNum; i++)
        base.push_back(i - varNum + 1);
}
SSimplex::~SSimplex() {
    for (int i = 0; i <= consNum; i++) free(a[i]);
    free(a);
}

void SSimplex::printMatrix() {
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    for (int i = 0; i <= consNum; i++) {
        for (int j = 0; j <= varNum; j++)
            std::cout << a[i][j] << "\t";
        std::cout << std::endl;
    }
    std::cout << "<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
}

double SSimplex::getAns(std::vector<double> &x) {
    x.clear();
    for (int i = 0; i < varNum; i++) {
        x.push_back(base[i] ? a[base[i]][varNum] : 0);
    }
    return a[0][varNum];
}

void SSimplex::printAns() {
    std::vector<double> x;
    double ans = getAns(x);
    std::cout << "Minimum: " << ans << "\t";
    for (int i = 0; i < varNum; i++) {
        std::cout << "x" << i+1 << "=" << x[i] << "\t";
    }
    std::cout << std::endl;
}

double SSimplex::solve() {

    return a[0][varNum];
}

bool SSimplex::pivot(int &in, int &out, int consID) {
    
}

void SSimplex::Gaussian(int &in, int &out) {

}