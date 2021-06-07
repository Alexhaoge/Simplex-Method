#include <vector>
#ifndef BIGM
#define BIGM
#define EPS 1e-6
class BigM{
public:
    double x, M;
    BigM(double _x, double _M): x(_x), M(_M) {}
    friend BigM operator+(const BigM &a, const BigM &b);
    friend BigM operator-(const BigM &a, const BigM &b);
    friend BigM operator*(const BigM &a, const double &mul);
    friend bool operator>(const BigM &a, const BigM &b);
    friend bool operator<=(const BigM &a, const double &b);
    BigM operator/(double scale) { x /= scale, M /= scale; }
};

class Simplex_BigM{
public:
    BigM* C;
    double** mat;
    std::vector<int> base;
    int varNum, consNum;
    // 输入矩阵为标准型的包含ABC的(cons + 1) x (var + 1)维矩阵
    Simplex_BigM(double* _mat, int var, int cons);
    ~Simplex_BigM();
    void printMatrix();
    double getAns(std::vector<double> &x);
    void printAns();
    bool solve(bool verbose=false);
protected:
    int pivot(int &in, int &out);
    void Gaussian(int &in, int &out);
};
#endif