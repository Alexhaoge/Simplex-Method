#include <vector>
#include <map>
#include <utility>
#include <ostream>
using namespace std;

/**
 * NOTE: 暂时没想好怎么写一般线性规划到标准型的转化
**/
#ifndef LP
#define LP
// enum ConstrainType {
//     EQUAL=0, LEQ, GEQ
// };

// class Constraint {
// public:
//     map<int, double> coef;
//     double b;
//     ConstrainType type;
// };

// class GeneralLP {
// public:
//     vector<Constraint> constraints;
//     bool isObjMin;
//     int varNum;
//     map<int, double> objCoef;
// };

/**
 * StandardLP为标准型线性规划问题类
 */
class StandardLP {
public:
    int varNum;
    map<int, double> C;
    vector<map<int, double>> A;
    map<int, double> b;
    int getConstrainNum() const { return A.size(); }
    /**
     * toMatrix转化为二维数组形式
     * 第一行为目标函数系数C，最后一列为等式右侧常数项b
     * 其余部分为约束系数A，即
     * | C 0 |
     * | A b |
     */
    void toMatrix(double ***res);
    /**
     * toMatrix_BigM转化为用于大M法的两个二维数组，形状均如下
     * | C 0 |
     * | A b |
     * 第二个矩阵为M的系数，第一个为常数系数
     */
    void toMatrix_BigM(double ***res, double ***resM);
    ostream& operator<<(ostream& out);
};
#endif