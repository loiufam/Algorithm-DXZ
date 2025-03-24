#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <stack>
#include <memory>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <chrono>
#include "/opt/homebrew/Cellar/gperftools/2.15/include/gperftools/malloc_extension.h"
namespace fs = std::filesystem;


// 定义ZDD节点
struct ZDDNode {
    int label;
    ZDDNode* lo;
    ZDDNode* hi;
    bool isTerminal;

    ZDDNode(int label, ZDDNode* lo, ZDDNode* hi, bool isTerminal)
        : label(label), lo(lo), hi(hi), isTerminal(isTerminal) {}

};


//定义二维标记数组，方便删除恢复操作
struct MatrixWithFlags {
    std::vector<std::vector<int> > data; // 二维数组
    std::vector<bool> isDeletedCol;      // 列删除标志数组
    std::vector<bool> isDeletedRow;      // 行删除标志数组

    // 构造函数，直接接受二维数组
    MatrixWithFlags(const std::vector<std::vector<int> >& inputData)
        : data(inputData), 
          isDeletedCol(inputData[0].size(), false), 
          isDeletedRow(inputData.size(), false) {}

    // 删除指定的行
    void deleteRow(size_t row) {
        if (row < isDeletedRow.size()) {
            isDeletedRow[row] = true; // 标记为已删除
        }
    }

    // 恢复指定的行
    void restoreRow(size_t row) {
        if (row < isDeletedRow.size()) {
            isDeletedRow[row] = false; // 标记为未删除
        }
    }

    // 删除指定的列
    void deleteCol(size_t col) {
        if (col < isDeletedCol.size()) {
            isDeletedCol[col] = true; // 标记为已删除
        }
    }

    // 恢复指定的列
    void restoreCol(size_t col) {
        if (col < isDeletedCol.size()) {
            isDeletedCol[col] = false; // 标记为未删除
        }
    }
    // 清空所有数据，释放资源
    void clear() {
        data.clear();
        isDeletedCol.clear();
        isDeletedRow.clear();
    }
};