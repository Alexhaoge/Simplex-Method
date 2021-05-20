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
    vector<double> b;
    int getConstrainNum() const { return A.size(); }
    /**
     * toMatrix转化为二维数组形式
     * 第一行为目标函数系数C，最后一列为等式右侧常数项b
     * 其余部分为约束系数A
     * NOTE: 函数内部会为res分配二维数组内存且不会释放，注意内存管理避免泄露
     * | C 0 |
     * | A b |
     */
    void toMatrix(double** &res);
    /**
     * 静态方法，将输出的AbC数组合并为一个二维数组
     * AbC对应问题 min <c,x> st Ax=b x>=0
     * NOTE: 函数内部会为res分配二维数组内存且不会释放，注意内存管理避免泄露
     * varNum: 自变量个数
     * consNum: A的行数，及小于等于号的个数
     */
    static void toMatrix(double** &res, int varNum, int consNum, double* C, double** A, double* b);
    /**
     * toMatrix_BigM转化为用于大M法的两个二维数组，形状均如下
     * | C 0 |
     * | A b |
     * 第二个矩阵为M的系数，第一个为常数系数
     */
    // void toMatrix_BigM(double** &res, double** &resM);
    /* 定义输出*/
    ostream& operator<<(ostream& out);
};

/**
 * 静态方法，将输出的AbC数组合并为一个二维数组
 * AbC对应问题 min <c,x> st Ax<=b x>=0 
 * 所以这里会自动在尾部添加与A行数相同的consNum个辅助变量变为标准型
 * 且最后consNum个变量为初始的基变量
 * NOTE: 函数内部会为res分配二维数组内存且不会释放，注意内存管理避免泄露
 * varNum: 自变量个数
 * consNum: A的行数，及小于等于号的个数
 */
void SimpleLPtoMatrix(double** &res, int varNum, int consNum, double* C, double** A, double* b);

#endif