#include <vector>
#include <map>
#include <iostream>
using namespace std;

/**
 * NOTE: 暂时没想好怎么写一般线性规划到标准型的转化
**/
#ifndef LP
#define LP
enum ConstrainType { EQUAL=0, LEQ, GEQ };

class Constraint {
public:
    map<int, double> coef;
    double b;
    ConstrainType type;
    /***
     * 必须一行一个约束条件，以'\n'分隔
     * 未实现对标号的重映射，认为变量个数=最大标号
     * 所以使用x1和x100两个变量会被认为一共有100个变量
     */
    friend istream& operator>>(istream& in, Constraint& cons);
    friend ostream& operator<<(ostream& out, Constraint& cons);
};

class GeneralLP {
public:
    vector<Constraint> constraints;
    bool isObjMin;
    int varNum;
    map<int, double> C;
    /***
     * 1. 目标函数需要以min或者max开头，不要写s.t.，不能有常数
     * 2. 变量名应形如x1,x2... 以字母x开头后接正整数
     * 3. 必须一行一个约束条件，以'\n'分隔
     * 未实现对标号的重映射，认为变量个数=最大标号
     * 所以使用x1和x100两个变量会被认为一共有100个变量
     */
    friend istream& operator>>(istream& in, GeneralLP& glp);
    friend ostream& operator<<(ostream& out, GeneralLP& glp);
};

/**
 * StandardLP为标准型线性规划问题类
 */
class StandardLP {
public:
    int varNum;
    map<int, double> C;
    vector<map<int, double> > A;
    vector<double> b;
    StandardLP(GeneralLP glp);
    int getConstrainNum() const { return A.size(); }
    /**
     * toMatrix转化为二维数组形式
     * 第一行为目标函数系数C，最后一列为等式右侧常数项b
     * 其余部分为约束系数A
     * NOTE: 函数内部不会为res分配二维数组内存
     * | C 0 |
     * | A b |
     */
    void toMatrix(double* res);
    
    /**
     * 静态方法，将输出的AbC数组合并为一个二维数组
     * AbC对应问题 min <c,x> st Ax=b x>=0
     * NOTE: 函数内部不会为res分配二维数组内存
     * varNum: 自变量个数
     * consNum: A的行数，及小于等于号的个数
     */
    static void toMatrix(double* res, int varNum, int consNum, double* C, double* A, double* b);
    
    double restoreAns(vector<double> &originX, double ans, const vector<double> &standardX);

    /* 定义输出*/
    friend ostream& operator<<(ostream& out, StandardLP& slp);

protected:
    // TODO: 或许把这里设计成一个adapter类会更好
    int originVarNum;
    double cBias;
    map<int, double> xBias;
    map<int, int> xPrime;
};

/**
 * 静态方法，将输出的AbC数组合并为一个二维数组
 * AbC对应问题 min <c,x> st Ax<=b x>=0 
 * 所以这里会自动在尾部添加与A行数相同的consNum个辅助变量变为标准型
 * 且最后consNum个变量为初始的基变量
 * NOTE: 函数内部不会为res分配二维数组内存
 * varNum: 自变量个数
 * consNum: A的行数，及小于等于号的个数
 */
void SimpleLPtoMatrix(double* res, int varNum, int consNum, double* C, double* A, double* b);

#endif
