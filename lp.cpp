#include "lp.h"

void StandardLP::toMatrix(double** &res){
    res = (double **)malloc(sizeof(double *) * (A.size() + 1));
    for (int i = 0; i <= A.size(); i++) {
        res[i] = (double *)malloc(sizeof(double) * (varNum + 1));
    }
    for (int i = 0; i < varNum; i++) {
        res[0][i] = C[i];
        for (int j = 0; j < A.size(); j++) {
            res[j + 1][i] = A[j][i];
        }
    }
    res[0][varNum] = 0;
    for (int i = 1; i <= A.size(); i++) {
        res[i][varNum] = b[i];
    }
}

void StandardLP::toMatrix(double** &res, int varNum, int consNum, double* C, double** A, double* b) {
    res = (double **)malloc(sizeof(double *) * (consNum + 1));
    for (int i = 0; i <= consNum; i++) {
        res[i] = (double *)malloc(sizeof(double) * (varNum + 1));
    }
    for (int i = 0; i < varNum; i++) {
        res[0][i] = C[i];
        for (int j = 0; j < consNum; j++) {
            res[j + 1][i] = A[j][i];
        }
    }
    res[0][varNum] = 0;
    for (int i = 1; i <= consNum; i++) {
        res[i][varNum] = b[i];
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
    for (int i = 0; i < A.size(); i++) {
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

void SimpleLPtoMatrix(double** &res, int varNum, int consNum, double* C, double** A, double* b) {
    res = (double **)malloc(sizeof(double *) * (consNum + 1));
    for (int i = 0; i <= consNum; i++) {
        res[i] = (double *)malloc(sizeof(double) * (varNum + consNum + 1));
    }
    for (int i = 0; i < varNum; i++) {
        res[0][i] = C[i];
        for (int j = 0; j < consNum; j++) {
            res[j + 1][i] = A[j][i];
        }
    }
    for (int i = varNum; i <= varNum + consNum; i++) {
        res[0][i] = 0;
        for (int j = 0; j < consNum; j++) {
            res[j + 1][i] = (j == (i - varNum)) ? 1 : 0;
        }
    }
    res[0][varNum + consNum] = 0;
    for (int i = 1; i <= consNum; i++) {
        res[i][varNum + consNum] = b[i];
    }
}