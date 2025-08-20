#ifndef DANCINGLINKS_H
#define DANCINGLINKS_H

#include<iostream> 
#include<bitset> 
#include<unordered_map>
#include<unordered_set>
#include<set>
#include<queue>
using namespace std;  

struct Node  
{  
    Node* left, *right, *up, *down;  
    int col, row;  
    Node(){  
        left = NULL; right = NULL;  
        up = NULL; down = NULL;  
        col = 0; row = 0;  
    }  
    Node( int r, int c )  
    {  
        left = NULL; right = NULL;  
        up = NULL; down  = NULL;  
        col = c; row = r;  
    }  
};  
  
struct ColunmHeader : public Node  
{  
    int size;  
    ColunmHeader(){  
        size = 0;  
    }  
};

// 定义ZDD节点
struct ZDDNode {
    int label;
    ZDDNode* lo;
    ZDDNode* hi;
    bool isTerminal;

    ZDDNode(int label, ZDDNode* lo, ZDDNode* hi, bool isTerminal = false)
        : label(label), lo(lo), hi(hi), isTerminal(isTerminal) {}
    
    // 等价节点比较
    bool operator==(const ZDDNode& other) const {
        return label == other.label && lo == other.lo && hi == other.hi && isTerminal == other.isTerminal;
    }

    // 拷贝构造函数
    ZDDNode(const ZDDNode& other) : label(other.label), isTerminal(other.isTerminal), lo(other.lo), hi(other.hi) {}

};

// 辅助函数用于复制节点
inline ZDDNode* copyNode(ZDDNode* original) {
    if (original == nullptr) {
        return nullptr;
    }
    // 使用拷贝构造函数创建新节点
    ZDDNode* newNode = new ZDDNode(*original);
    // 如果 low 或 high 不是 nullptr，则递归复制它们
    if (newNode->lo != nullptr) {
        newNode->lo = copyNode(newNode->lo);
    }
    if (newNode->hi != nullptr) {
        newNode->hi = copyNode(newNode->hi);
    }
    return newNode;
}

// 自定义比较函数，使得 set 按降序存储
struct Greater {
    bool operator()(int a, int b) const {
        return a > b;
    }
};

class DancingLinks
{
public:
    int countNum;
    int countSolution;
    DancingLinks(int rows, int cols, const std::vector<std::vector<int>>& matrix);
    ~DancingLinks();
    void insert(int r, int c);
    void cover(int c);
    void uncover(int c);
    void columnToVector(std::vector<bool>& vec);
    size_t hashFunction(int r, ZDDNode* x, ZDDNode* y);
    ZDDNode* unique(int r, ZDDNode* x, ZDDNode* y);
    ZDDNode* search();
    void printZDD(ZDDNode* node);
    void printTable();
    void printCache();
    void printColumnHeaders();
    void printRowNodes();
    void printRemainingColumns();
    string getColumnState() const;

private:
    Node* RowIndex;
    ColunmHeader* root;
    int ROWS;
    int COLS;
    ColunmHeader* ColIndex;
    ZDDNode* T;
    ZDDNode* F;
    std::unordered_map<size_t, ZDDNode*> Z;
    std::unordered_map<std::string, ZDDNode*> C;
};

#endif