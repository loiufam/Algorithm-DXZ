#include "../include/ZDD.h"

void printMatrix(const MatrixWithFlags& A){
    int row = A.data.size();
    int col = A.data[0].size();
    for (int i = 0; i < row; ++i) {
        if (!A.isDeletedRow[i]) { // 检查行是否被删除
            for (int j = 0; j < col; ++j) {
                if (!A.isDeletedCol[j]) { // 检查列是否被删除
                    std::cout << A.data[i][j] << " ";
                }
            }
            std::cout << std::endl;
        }
    }
}


int main(){
    // 初始化输入矩阵X
    std::vector<std::vector<int> > X = {
        {1, 1, 1, 0, 1, 0},
        {1, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 1},
        {0, 0, 1, 1, 0, 1},
        {0, 0, 1, 0, 1, 0}
    };

    //构造输入矩阵的二维标记数据结构
    MatrixWithFlags matrix(X);

    matrix.deleteRow(1);
    matrix.deleteCol(2);

    //打印矩阵
    printMatrix(matrix);

    return 0;
}