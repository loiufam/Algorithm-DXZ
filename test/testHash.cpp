#include <functional>
#include <cstdint>
#include <iostream>
#include <tuple>

using KeyTuple = std::tuple<int, uintptr_t, uintptr_t>;

// 定义ZDD节点
struct ZDDNode {
    int label;  // 节点标签
    ZDDNode* lo; // 如果当前位为0，则指向的子节点
    ZDDNode* hi; // 如果当前位为1，则指向的子节点
    bool terminal; // 是否是终端节点
    static ZDDNode* T; // 终端节点T
    static ZDDNode* F; // 终端节点F

    ZDDNode(int l, ZDDNode* low, ZDDNode* high, bool isTerminal)
        : label(l), lo(low), hi(high), terminal(isTerminal) {}

    // 析构函数，用于清理内存
    ~ZDDNode() {
        delete lo;
        delete hi;
    }
};

// 自定义哈希函数
struct UniqueKeyHash {
    std::size_t operator()(const std::tuple<int, uintptr_t, uintptr_t>& key) const {
        int r = std::get<0>(key);
        uintptr_t x = std::get<1>(key);
        uintptr_t y = std::get<2>(key);

        std::hash<int> hashInt;
        std::hash<uintptr_t> hashPtr;

        std::size_t h1 = hashInt(r);
        std::size_t h2 = hashPtr(x);
        std::size_t h3 = hashPtr(y);

        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

// 生成唯一键值
uint64_t generateUniqueKey(int r, ZDDNode* x, ZDDNode* y) {
    KeyTuple keyTuple = std::make_tuple(r, reinterpret_cast<uintptr_t>(x), reinterpret_cast<uintptr_t>(y));
    UniqueKeyHash hasher;
    return hasher(keyTuple);
}

int main() {
    // 示例
    ZDDNode* x = new ZDDNode(0,nullptr,nullptr,false); // 假设 ZDDNode 是一个有效的类型
    ZDDNode* y = new ZDDNode(0,nullptr,nullptr,true);
    int r = 1;

    // 生成唯一键值
    uint64_t uniqueKey = generateUniqueKey(r, x, y);
    std::cout << "Unique key: " << uniqueKey << std::endl;

    // 清理资源
    delete x;
    delete y;

    return 0;
}