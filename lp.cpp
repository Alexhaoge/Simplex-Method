#include "lp.h"
#include <cstdlib>

StandardLP::StandardLP(GeneralLP glp) {
    C.clear(), A.clear(), b.clear();
    C = glp.C;
    
}

void StandardLP::toMatrix(double* res){
    int consNum = A.size();
    for (int i = 0; i < varNum; i++) {
        res[i] = C[i];
        for (int j = 0; j < consNum; j++) {
            res[(j + 1)*(varNum + 1) + i] = A[j][i];
        }
    }
    res[varNum] = 0;
    for (int i = 1; i <= consNum; i++) {
        res[i * (varNum + 1) + varNum] = b[i];
    }
}

void StandardLP::toMatrix(double* res, int varNum, int consNum, double* C, double* A, double* b) {
    for (int i = 0; i < varNum; i++) {
        res[i] = C[i];
        for (int j = 0; j < consNum; j++) {
            res[(j + 1)*(varNum + 1) + i] = A[j * varNum + i];
        }
    }
    res[varNum] = 0;
    for (int i = 1; i <= consNum; i++) {
        res[i * (varNum + 1) + varNum] = b[i];
    }
}

ostream& StandardLP::operator<<(ostream& out) {
    out << "min";
    for (int i = 0; i < varNum; i++) {
        if (C[i] < 0) {
            out << "-" << -C[i] << "x" << i+1 << "\t";
        } else {
            if (i) out << "+";
            out << C[i] << "x" << i+1 << "\t";
        }
    }
    out << " s.t." << endl;
    for (int i = 0, consNum = A.size(); i < consNum; i++) {
        for (int j = 0; j < varNum; j++) {
            if (A[i][j] < 0) {
                out << "-" << -A[i][j] << "x" << i+1 << "\t";
            } else {
                if (i) out << "+";
                out << A[i][j] << "x" << i+1 << "\t";
            }
        }
        out << "=" << b[i] << endl;
    }
    out << "x1,...,x" << varNum << ">=0" << endl;
    return out;
}

void SimpleLPtoMatrix(double* res, int varNum, int consNum, double* C, double* A, double* b) {
    for (int i = 0; i < varNum; i++) {
        res[i] = C[i];
        for (int j = 0; j < consNum; j++) {
            res[(j + 1)*(varNum + 1) + i] = A[j * varNum + i];
        }
    }
    for (int i = varNum; i <= varNum + consNum; i++) {
        res[i] = 0;
        for (int j = 0; j < consNum; j++) {
            res[(j + 1)*(varNum + 1) + i] = (j == (i - varNum)) ? 1 : 0;
        }
    }
    res[varNum + consNum] = 0;
    for (int i = 1; i <= consNum; i++) {
        res[i * (varNum + 1) + varNum + consNum] = b[i];
    }
}
