#include "bigm.h"
#include <cmath>
#include <cstring>
#include <cstdlib>
#include "float.h"

BigM operator+(const BigM &a, const BigM &b) {
    return BigM(a.x + b.x, a.M + b.M);
}
BigM operator-(const BigM &a, const BigM &b) {
    return BigM(a.x - b.x, a.M - b.M);
}
BigM operator*(const BigM &a, const double &mul){
    return BigM(mul * a.x, mul * a.M);
}
bool operator>(const BigM &a, const BigM &b){
    return a.M == b.M ? a.x > b.x : a.M > b.M;
}

bool operator<=(const BigM &a, const double &b) {
    return fabs(a.M) < EPS ? a.x <= b : a.M < 0;
}

bool operator<(const BigM &a, const double &b) {
    return fabs(a.M) < EPS ? a.x < b : a.M < 0;
}

ostream& operator<<(ostream& out, BigM& bm) {
    out << bm.x;
    if (bm.M < 0)
        out << "-" << -bm.M;
    else
        out << "+" << bm.M;
    out << "M";    
    return out;
}

Simplex_BigM::Simplex_BigM(double* _mat, int var, int cons) {
    varNum = var, consNum = cons;
    mat = (double **)malloc(sizeof(double *) * consNum);
    C = (BigM *)malloc(sizeof(BigM)*(varNum+consNum+1));
    for (int j = 0; j < varNum + consNum; j++){
        C[j].M = 0;
        C[j].x = j < varNum ?  _mat[j]: 0;
    }
    C[varNum + consNum].x = 0, C[varNum + consNum].M = 0;
    for (int i = 1; i <= consNum; i++) {
        mat[i-1] = (double *)malloc(sizeof(double)*(varNum+consNum+1));
        for (int j = 0; j < varNum; j++)
            mat[i-1][j] = _mat[i*(varNum+1)+j];
        for (int j = varNum; j < varNum+consNum; j++)
            mat[i-1][j] = (j - varNum + 1 == i) ? 1 : 0;
        mat[i-1][varNum + consNum] = _mat[i*(varNum+1)+varNum];
    }
    for (int j = 0; j < varNum; j++) {
        for (int i = 0; i < consNum; i++)
            C[j].M -= mat[i][j];
    }
    for (int i = 0; i < varNum; i++)
        base.push_back(0);
    for (int i = 0; i < consNum; i++)
        base.push_back(i+1);
}
Simplex_BigM::~Simplex_BigM() {
    for (int i = 0; i < consNum; i++) free(mat[i]);
    free(mat);
    free(C);
}

void Simplex_BigM::printMatrix() {
    cout << ">>>>>>>>>>>>>>>>>>>>>>" << endl;
    for (int j = 0; j <= varNum + consNum; j++)
        cout << C[j] << "\t";
    cout << endl << "--------------------------------------" << endl;
    for (int i = 0; i < consNum; i++) {
        for (int j = 0; j <= varNum+consNum; j++)
            cout << mat[i][j] << "\t";
        cout << endl;
    }
    cout << "<<<<<<<<<<<<<<<<<<<<<<" << endl;
}

double Simplex_BigM::getAns(vector<double> &x) {
    x.clear();
    for (int i = 0; i < varNum; i++) {
        x.push_back(base[i] ? mat[base[i]-1][varNum + consNum] : 0);
    }
    return -C[varNum + consNum].x;
}

void Simplex_BigM::printAns() {
    vector<double> x;
    double ans = getAns(x);
    cout << "Minimum: " << ans << "\t";
    for (int i = 0; i < varNum; i++) {
        cout << "x" << i+1 << "=" << x[i] << "\t";
    }
    cout << endl;
}

bool Simplex_BigM::solve(bool verbose) {
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

int Simplex_BigM::pivot(int &in, int &out) {
    BigM min_in(DBL_MAX, DBL_MAX);
    in = -1;
    for (int j = 0; j < varNum + consNum; j++)
        if (!base[j] && min_in > C[j])
            min_in = C[j], in = j;
    if (!(min_in < 0)) return 0;
    double min_out = DBL_MAX;
    out = -1;
    for (int i = 0; i < consNum; i++)
        if (mat[i][in] > 0) {
            double tmp = mat[i][varNum + consNum] / mat[i][in];
            if (min_out > tmp)
                min_out = tmp, out = i + 1;
        }
    if (out == -1) return -1;
    for (int j = 0; j < varNum + consNum; j++)
        if (base[j] > 0 && base[j] == out) {
            out = j;
            break;
        }    
    return 1;
}

void Simplex_BigM::Gaussian(int &in, int &out) {
    int x = base[out] - 1;
    double scale = mat[x][in];
    for (int i = 0; i <= varNum + consNum; i++)
        mat[x][i] = mat[x][i] / scale;
    for (int i = 0; i < consNum; i++)
        if (i != x && mat[i][in] != 0){
            scale = mat[i][in];
            for (int j = 0; j <= varNum + consNum; j++) {
                mat[i][j] -= scale * mat[x][j];
            }
        }
    BigM scaleM = C[in];
    for (int j = 0; j <= consNum + varNum; j++)
        C[j] = C[j] - scaleM * mat[x][j];
    base[in] = base[out], base[out] = 0;
}
