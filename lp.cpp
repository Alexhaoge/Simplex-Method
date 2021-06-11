#include "lp.h"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <stdexcept>

istream& operator>>(istream& in, Constraint& cons) {
    char c; cons.b = 0;
    double tmp = 0; // 记录系数
    int decimalBit = 0; // 记录小数点位数
    int varIdx; // 记录变量标号
    bool hasDecimal = false; // 是否有小数点
    bool isNegative = false; // 当前项是否为负
    bool hasPastOpt = false; // 判断当前位置是否在等号或不等号右侧
    bool isVar = false; // 是否为变量，否则为b
    bool omitCoef = true; // x前系数是否被忽略
    while (in.get(c)){
    	if (c == '\n' || c == '\r') {
            if (!hasPastOpt) throw logic_error("A constraint must have '=', '<=' or '>='");
            if (!isVar)
                cons.b += isNegative ? -tmp : tmp;
            break;
        } else if(c == '-') {
            if (!isVar) {
                cons.b += (isNegative ^ (!hasPastOpt)) ? -tmp : tmp;
                tmp = 0;
            }
            isNegative = omitCoef = true;
            hasDecimal = isVar = false;
        } else if (c == '+') {
            if (!isVar) {
                cons.b += (isNegative ^ (!hasPastOpt)) ? -tmp : tmp;
                tmp = 0;
            }
            isNegative = hasDecimal = isVar = false;
            omitCoef = true;
        } else if (c >= '0' && c <= '9') {
            if (hasDecimal) tmp += (c - '0') / pow(10, ++decimalBit);
            else tmp = tmp * 10 + (c - '0');
            omitCoef = false;
        } else if (c == '.') {
            if (hasDecimal) throw logic_error("Found decimal dot point('.') in one number");
            hasDecimal = true, decimalBit = 0;
        } else if (c == 'x') {
            in >> varIdx;
            isVar = true;
            if (varIdx < 0) throw logic_error("Invalid variable index (must be positive integer)");
            if (omitCoef) tmp = 1;
            cons.coef[varIdx-1] += (isNegative ^ hasPastOpt) ? -tmp : tmp;
            hasDecimal = isNegative = isVar = false;
            tmp = 0;
            omitCoef = true;
        } else if (c == '=') {
            if (!isVar) {
                cons.b += (isNegative ^ (!hasPastOpt)) ? -tmp : tmp;
                tmp = 0;
            }
            if (hasPastOpt) throw logic_error("Found 2 of ('=', '<=', '>=') in one contraint");
            hasPastOpt = true, cons.type = EQUAL;
            omitCoef = true;
        } else if (c == '<' || c == '>') {
            if (!isVar) {
                cons.b += (isNegative ^ (!hasPastOpt)) ? -tmp : tmp;
                tmp = 0;
            }
            if (hasPastOpt) throw logic_error("Found 2 of ('=', '<=', '>=') in one contraint");
            char c2; in >> c2;
            if (c2 != '=')
                throw logic_error("Invalid constraint type, only '=', '<=', '>=' is allowed");
            omitCoef = hasPastOpt = true, cons.type = c == '<' ? LEQ : GEQ;
        }
    }
    return in;
}

ostream& operator<<(ostream& out, Constraint& cons) {
    map<int, double>::iterator it = cons.coef.begin();
    out << it->second << "x" << it->first+1 << "\t";
    for (it++; it != cons.coef.end(); it++)
    	if (it->second < 0) {
    		out << "-" << -it->second << "x" << it->first+1 << "\t";
		} else {
			if (it != cons.coef.begin())	out << "+";
			out << it->second << "x" << it->first+1 << "\t";
		}
    switch (cons.type) {
        case EQUAL: out << "="; break;
        case LEQ: out << "<="; break;
        case GEQ: out << ">="; break;
    }
    out << "\t" << cons.b;
    return out;
}

istream& operator>>(istream& in, GeneralLP& glp) {
    string minmax; in >> minmax;
    if (!minmax.compare("min")) glp.isObjMin = true;
    else if (!minmax.compare("max")) glp.isObjMin = false;
    else throw invalid_argument("Object must be 'min' or 'max'!");
    char c; double tmp = 0;
    int decimalBit = 0; // 记录小数点位数
    int varIdx; // 记录变量标号
    bool hasDecimal = false; // 是否有小数点
    bool isNegative = false; // 当前项是否为负
    bool omitCoef = true; // x前系数是否被忽略
    while (in.get(c)) {
        if (c == '\n' || c == '\r') {
            if (tmp != 0) throw logic_error("Object should not contain constant");
            break;
        } if(c == '-') {
            isNegative = true, hasDecimal = false, omitCoef = true;
        } else if (c == '+') {
            isNegative = false, hasDecimal = false, omitCoef = true;
        } else if (c >= '0' && c <= '9'){
            if (hasDecimal) tmp += (c - '0') / pow(10, ++decimalBit);
            else tmp = tmp * 10 + (c - '0');
            omitCoef = false;
        } else if (c == '.') {
            if (hasDecimal) throw logic_error("Found decimal dot point('.') in one number");
            hasDecimal = true, decimalBit = 0;
        } else if (c == 'x') {
            in >> varIdx;
            if (varIdx <= 0) throw out_of_range("Invalid variable index (must be positive integer)");
            if (omitCoef) tmp = 1;
            glp.C[varIdx-1] += isNegative ? -tmp : tmp;
            hasDecimal = isNegative = false, tmp = 0;
            omitCoef = true;
        }
    }
    while (true) {
        Constraint cons; in >> cons;
        glp.constraints.push_back(cons);
        if (in.eof()) break;
    }
    glp.varNum = (--glp.C.end())->first;
    for (int i = 0, _sz = glp.constraints.size(); i < _sz; i++)
        glp.varNum = max(glp.varNum, (--glp.constraints[i].coef.end())->first);
    glp.varNum++;
    return in;
}

ostream& operator<<(ostream& out, GeneralLP& glp) {
    out << "min ";
    for (int i = 0; i < glp.varNum; i++){
        if (glp.C[i] < 0) {
            out << "-" << -glp.C[i] << "x" << i+1 << "\t";
        } else {
            if (i) out << "+";
            out << glp.C[i] << "x" << i+1 << "\t";
        }
    }
    out << "s.t." << endl;
    for (int i = 0, consNum = glp.constraints.size(); i < consNum; i++)
        out << glp.constraints[i] << endl;
    return out;
}

StandardLP::StandardLP(GeneralLP glp) {
    C.clear(), A.clear(), b.clear();
    xBias.clear(), cBias = 0, xPrime.clear();
    varNum = originVarNum = glp.varNum;
    bool *isXRange = (bool *)malloc(sizeof(bool)*glp.constraints.size());
    memset(isXRange, 0, sizeof(bool) * glp.constraints.size());
    for (int i = 0, _sz = glp.constraints.size(); i < _sz; i++){
        Constraint con = glp.constraints[i];
        if (con.coef.size() == 1) {
            int xIdx = con.coef.begin()->first;
            double xcoef = con.coef.begin()->second;
            if (xcoef < 0){
                if (con.type == GEQ) con.type = LEQ;
                else if (con.type == LEQ) con.type = GEQ;
            }
            if (con.type == GEQ) {
                xBias[xIdx] = max(xBias[xIdx], con.b / xcoef);
                isXRange[i] = true;
            }
        }
    }
    // 找到无约束变量，设为xi - x'
    for (int i = 0; i < originVarNum; i++)
        if (xBias.find(i) == xBias.end())
            xPrime[i] = varNum++;
    // 重写目标函数
    cBias = 0;
    for (int i = 0; i < originVarNum; i++) {
        C[i] = glp.isObjMin ? glp.C[i] : -glp.C[i];
        if (xBias.find(i) == xBias.end())
            C[xPrime[i]] = -C[i];
        else
            cBias += C[i] * xBias[i];
    }
    // 重写新的约束条件
    for (int i = 0, _sz = glp.constraints.size(); i < _sz; i++) 
        if (!isXRange[i]) {
            Constraint con = glp.constraints[i];
            map<int, double> Ai; Ai.clear();
            double bi = con.b;
            for (int j = 0; j < originVarNum; j++) {
                Ai[j] = con.coef[j];
                if (xBias.find(j) == xBias.end())
                    Ai[xPrime[j]] = -Ai[j];
                else
                    bi -= Ai[j] * xBias[j];
            }
            if (bi < 0) {
                bi = -bi;
                for (int j = 0; j < originVarNum; j++) {
                    Ai[j] = -Ai[j];
                    if (xPrime.find(j) == xPrime.end())
                        Ai[xPrime[j]] = -Ai[xPrime[j]];
                }
                if (con.type == LEQ) con.type = GEQ;
                else if (con.type == GEQ) con.type = LEQ;
            }
            if (con.type == LEQ) Ai[varNum++] = 1;
            else if (con.type == GEQ) Ai[varNum++] = -1;
            A.push_back(Ai);
            b.push_back(bi);
        }
    free(isXRange);
}

void StandardLP::toMatrix(double* res){
    int consNum = A.size();
    for (int i = 0; i < varNum; i++) {
        map<int, double>::const_iterator tmp = C.find(i);
        res[i] = (tmp == C.end()) ? 0 : tmp->second;
        for (int j = 0; j < consNum; j++) {
            tmp = A[j].find(i);
            res[(j + 1)*(varNum + 1) + i] = (tmp == A[j].end()) ? 0 : tmp->second;
        }
    }
    res[varNum] = 0;
    for (int i = 1; i <= consNum; i++) {
        res[i * (varNum + 1) + varNum] = b[i-1];
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
        res[i * (varNum + 1) + varNum] = b[i-1];
    }
}

double StandardLP::restoreAns(vector<double> &originX, double ans, const vector<double> &standardX) {
    originX.clear();
    for (int i = 0; i < originVarNum; i++) {
        if (xPrime.find(i) == xPrime.end()) {
            originX.push_back(standardX[i]);
            if (xBias.find(i) != xBias.end())
                originX[i] += xBias[i];
        } else {
            originX.push_back(standardX[i] - standardX[xPrime[i]]);
        }
    }
    return ans + cBias;
}

ostream& operator<<(ostream& out, StandardLP& slp) {
    out << "min ";
    for (int i = 0; i < slp.varNum; i++) {
        if (slp.C[i] < 0) {
            out << "-" << -slp.C[i] << "x" << i+1 << "\t";
        } else {
            if (i) out << "+";
            out << slp.C[i] << "x" << i+1 << "\t";
        }
    }
    out << " s.t." << endl;
    for (int i = 0, consNum = slp.A.size(); i < consNum; i++) {
        for (int j = 0; j < slp.varNum; j++) {
            if (slp.A[i][j] < 0) {
                out << "-" << -slp.A[i][j] << "x" << j+1 << "\t";
            } else {
                if (j) out << "+";
                out << slp.A[i][j] << "x" << j+1 << "\t";
            }
        }
        out << "=" << slp.b[i] << endl;
    }
    out << "x1,...,x" << slp.varNum << ">=0" << endl;
    return out;
}

void SimpleLPtoMatrix(double* res, int varNum, int consNum, double* C, double* A, double* b) {
    for (int i = 0; i < varNum; i++) {
        res[i] = C[i];
        for (int j = 0; j < consNum; j++) {
            res[(j + 1)*(varNum + consNum + 1) + i] = A[j * varNum + i];
        }
    }
    for (int i = varNum; i <= varNum + consNum; i++) {
        res[i] = 0;
        for (int j = 0; j < consNum; j++) {
            res[(j + 1)*(varNum + consNum + 1) + i] = (j == (i - varNum)) ? 1 : 0;
        }
    }
    res[varNum + consNum] = 0;
    for (int i = 1; i <= consNum; i++) {
        res[i * (varNum + consNum + 1) + varNum + consNum] = b[i-1];
    }
}
