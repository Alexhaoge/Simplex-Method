#include <vector>

#ifndef SSIMPLEX
#define SSIMPLEX
class SSimplex{
public:
    double** mat;
    std::vector<int> base;
    int varNum, consNum;
    SSimplex(double* _mat, int var, int cons);
    ~SSimplex();
    void printMatrix();
    double getAns(std::vector<double> &x);
    void printAns();
    /**默认最后consNum个变量为基变量 */
    bool solve(bool verbose=false);
protected:
    int pivot(int &in, int &out);
    void Gaussian(int &in, int &out);
};
#endif