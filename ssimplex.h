#include <vector>

#ifndef SSIMPLEX
#define SSIMPLEX
class SSimplex{
public:
    double** a;
    std::vector<int> base;
    int varNum, consNum;
    SSimplex(double** _a, int var, int cons);
    ~SSimplex();
    void printMatrix();
    double solve();
    double getAns(std::vector<double> &x);
    void printAns();
protected:
    bool pivot(int &in, int &out, int consID);
    void Gaussian(int &in, int &out);
};
#endif