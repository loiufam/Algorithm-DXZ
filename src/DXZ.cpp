#include "../include/ZDD.h"

std::string vectorToString(const std::vector<bool>& vec) {
    std::string binary_string;
    for (bool b : vec) {
        binary_string += (b ? '1' : '0');
    }
    return binary_string;
}

// 选择包含最少1的列
int select_column(const MatrixWithFlags& A) {
    int col = A.isDeletedCol.size();
    int row = A.isDeletedRow.size();
    int minOnes = row + 1; // 初始化为比任何可能值都大的数
    int columnIndex = -1;
    for (size_t j = 0; j < col; ++j) {
        if (A.isDeletedCol[j]) continue; // 选未删除的列
        int count = 0;
        for (size_t i = 0; i < row; ++i) {
            if (A.isDeletedRow[i]) continue; // 选未删除行
            if (A.data[i][j] == 1) {
                ++count;
            }
        }
        if (count > 0 && count < minOnes) {
            minOnes = count;
            columnIndex = j;
        }
    }
    return columnIndex;
}

// 生成唯一键值
std::size_t generate_hash(int r, int x_label, int y_label) {
    return std::hash<int>()(r) ^ (std::hash<int>()(x_label) << 1) ^ (std::hash<int>()(y_label) << 2);
}
//递归查找终端节点
ZDDNode* findTerminalNode(const ZDDNode* x, const ZDDNode* y) { //在y中找x
    if (!y) {
        return nullptr;
    }

    if (y->isTerminal && y->label == x->label) {
        return const_cast<ZDDNode*>(y);
    }

    ZDDNode* foundInLo = findTerminalNode(x, y->lo);
    if (foundInLo) {
        return foundInLo;
    }

    ZDDNode* foundInHi = findTerminalNode(x, y->hi);
    if (foundInHi) {
        return foundInHi;
    }

    return nullptr;
}
// 创建唯一ZDD节点
ZDDNode* Unique(int r, const ZDDNode* x, const ZDDNode* y, std::unordered_map<size_t, ZDDNode*>& Z) {
    //std::cout<<"unique 节点label："<< r << " " << x->label << " " << y->label<< std::endl;
    std::size_t key = generate_hash(r, x->label ,  y->label);
    //std::cout<<"生成key："<< key << std::endl;
    if (Z.find(key) == Z.end()) {
        //如果x，y指向的都不是终端节点，直接插入ZDD中
        if(!x->isTerminal && !y->isTerminal){
            Z[key] = new ZDDNode(r, const_cast<ZDDNode*>(x) , const_cast<ZDDNode*>(y) , false);
        }
        else if (x->isTerminal && !y->isTerminal) //x是终端节点，在y中递归寻找等于x的终端节点
        {
            Z[key] = new ZDDNode(r, findTerminalNode(x, y), const_cast<ZDDNode*>(y), false);
        }
        else if (!x->isTerminal && !y->isTerminal)
        {
            Z[key] = new ZDDNode(r, const_cast<ZDDNode*>(x), findTerminalNode(y, x), false);
        }
        else if(x->isTerminal && y->isTerminal){
            Z[key] = new ZDDNode(r, const_cast<ZDDNode*>(x) , const_cast<ZDDNode*>(y) , false);
        }
    }
    return Z[key];
}

// ZDD节点表
std::unordered_map<size_t, ZDDNode*> Z;
// 缓存
std::unordered_map<std::string, ZDDNode*> C;

// 初始化静态成员
ZDDNode* T = new ZDDNode(-1, nullptr, nullptr, true);
ZDDNode* F = new ZDDNode(-2, nullptr, nullptr, true);

// 搜索函数
ZDDNode* Search(MatrixWithFlags& matrix) {

    //查找是否有未删除的列
    auto it = std::find(matrix.isDeletedCol.begin(), matrix.isDeletedCol.end(), false);

    if(it == matrix.isDeletedCol.end()){
        return T;
    }

    // 列状态转化为字符串作为键
    std::string colKey = vectorToString(matrix.isDeletedCol); 

    // 查找缓存
    auto iterator = C.find(colKey);
    if (iterator != C.end()) {
        return (iterator->second); // 返回一个ZDDNode*
    }

    // 选择一列
    int c = select_column(matrix);

    if (c == -1) { // 如果没有可用的列，返回 F
        return F;
    }

    ZDDNode* x = F;
    std::stack<int> rStack;
    std::stack<int> cStack;

    for (int r = matrix.isDeletedRow.size() - 1; r >= 0; --r) {
        if (matrix.isDeletedRow[r]) continue; // 忽略被删除的行
        if (matrix.data[r][c] == 1) {
            for (int j = 0; j < matrix.isDeletedCol.size(); ++j) {
                if (matrix.isDeletedCol[j]) continue; // 忽略被删除的列
                if (matrix.data[r][j] == 1) {
                    matrix.deleteCol(j); // 删除第j列
                    cStack.push(j);
                    for (int i = 0; i < matrix.isDeletedRow.size(); ++i) {
                        if (matrix.isDeletedRow[i]) continue; // 忽略被删除的行
                        if (matrix.data[i][j] == 1) {
                            matrix.deleteRow(i); // 删除第i行
                            rStack.push(i);
                        }
                    }
                }
            }
            ZDDNode* y = Search(matrix);
            if (y && y->label != -2) {
                x = Unique(r, x, y, Z);
            }
        }
        //恢复删除的行和列
        while (!rStack.empty())
        {
            matrix.restoreRow(rStack.top()); 
            rStack.pop();
        }

        while (!cStack.empty())
        {
            matrix.restoreCol(cStack.top()); 
            cStack.pop();
        }
    }       
    // 存储结果到缓存
    C[colKey] = x;

    return x;
}

//输出ZDD路径
void findPaths(const ZDDNode* node, std::string path, std::vector<std::string>& paths) {
    if (!node) return; // 如果节点为空，直接返回

    // 将当前节点的值添加到路径中
    path += std::to_string(node->label);

    // 如果是叶子节点，保存路径
    if (node->isTerminal) {
        if(node == T){
            paths.push_back(path);
        }
        return;
    } else {
        // 递归遍历左子树
        findPaths(node->lo, path + " -> lo: ", paths);
        // 递归遍历右子树
        findPaths(node->hi, path + " -> hi: ", paths);
    }
}

std::vector<std::string> binaryTreePaths(ZDDNode* root) {
    std::vector<std::string> paths;
    findPaths(root, "", paths);
    return paths;
}

// 释放内存的函数
void releaseMemory() {
    std::unordered_set<ZDDNode*> deletedNodes;

    // 释放 Z 中的所有节点
    for (auto& pair : Z) {
        ZDDNode* node = pair.second;
        if (node != nullptr && deletedNodes.find(node) == deletedNodes.end()) {
            delete node;
            deletedNodes.insert(node);
        }
    }
    Z.clear();

    // 释放 C 中的所有节点
    for (auto& pair : C) {
        ZDDNode* node = pair.second;
        if (node != nullptr && deletedNodes.find(node) == deletedNodes.end()) {
            delete node;
            deletedNodes.insert(node);
        }
    }
    C.clear();
}

// 从字符串中提取 n 和 m 的值
void extractNM(const std::string& line, int& n, int& m) {
    std::istringstream iss(line);
    std::string token;

    // 读取 "c"
    iss >> token;  // 读取 "c"

    // 读取 "n" 和其值
    iss >> token;  // 读取 "n"
    if (token == "n") {
        iss >> token;  // 读取 "="
        iss >> n;      // 读取 n 的值
    } else {
        throw std::runtime_error("格式错误: 未找到 'n'");
    }

    // 读取 "m" 和其值
    iss >> token;  // 读取 ","
    iss >> token;  // 读取 "m"
    if (token == "m") {
        iss >> token;  // 读取 "="
        iss >> m;      // 读取 m 的值
    } else {
        throw std::runtime_error("格式错误: 未找到 'm'");
    }
}

// 将原始数据处理成 0-1 矩阵
std::vector<std::vector<int>> processFileToMatrix(const std::string& filename, int& r, int& c) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件");
    }

    std::string line;
    std::getline(file, line);  // 读取第一行

    int n, m;
    extractNM(line, n, m);  // 读取第一行的 n 和 m
    r = m;
    c = n;

    std::getline(file, line);  // 跳过第二行

    // 动态分配二维向量
    std::vector<std::vector<int>> matrix(r, std::vector<int>(c, 0));

    int row = 0;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] != 's') continue;  // 忽略不是以 's' 开头的行
        std::istringstream iss(line.substr(2));  // 跳过前两个字符's '
        int col;
        while (iss >> col) {
            if (col > 0 && col <= n) {  // 确保列索引在有效范围内
                matrix[row][col - 1] = 1;  
            }
        }
        ++row;
    }

    file.close();
    return matrix;
}

// 递归函数，用于统计从给定节点开始的所有节点数量
int countNodesRecursive(ZDDNode* node, std::unordered_set<ZDDNode*>& visited) {
    if (node == nullptr) {
        return 0;
    }

    // 如果节点已经访问过，直接返回0
    if (visited.find(node) != visited.end()) {
        return 0;
    }

    // 标记当前节点为已访问
    visited.insert(node);

    // 递归统计 low 和 high 子树的节点数量
    int count = 1; // 当前节点
    count += countNodesRecursive(node->lo, visited);
    count += countNodesRecursive(node->hi, visited);

    return count;
}

// 公共接口函数，用于统计从给定节点开始的所有节点数量
int countNodes(ZDDNode* root) {
    std::unordered_set<ZDDNode*> visited;
    return countNodesRecursive(root, visited);
}

void printMemoryUsage() {
    size_t heap_size = 0;
    MallocExtension::instance()->GetNumericProperty("generic.current_allocated_bytes", &heap_size);
    
    // 转换为 MB
    double heap_size_mb = static_cast<double>(heap_size) / (1024 * 1024);
    
    //std::cout << "Current allocated bytes: " << heap_size << " B" << std::endl;
    std::cout << "Current allocated memory: " << heap_size_mb << " MB" << std::endl;
}

// 主函数
int main() {

    try {
        // 文件夹路径
        const std::string folderPath = "/Users/luoyaohui/VSCodeProjects/code_projects/algorithmLab/DLX/exact_cover_benchmark";
        //遍历文件夹
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt"){
                std::string filePath = entry.path().string();
                int r, c;
                std::vector<std::vector<int>> X = processFileToMatrix(filePath, r, c);
                
                // std::vector<std::vector<int>> X = {
                //     {1, 1, 1, 0, 1, 0},
                //     {1, 1, 0, 0, 0, 0},
                //     {0, 0, 0, 1, 0, 1},
                //     {0, 0, 1, 1, 0, 1},
                //     {0, 0, 1, 0, 1, 0},
                // };

                //构造输入矩阵的二维标记数据结构
                MatrixWithFlags matrix(X);
                std::cout<<entry.path().filename().string()<< ": "<< matrix.isDeletedCol.size() <<" "<< matrix.isDeletedRow.size() << "\n";

                // 打印矩阵
                // for (int i = 0; i < r; ++i) {
                //     for (int j = 0; j < c; ++j) {
                //         std::cout << matrix.data[i][j] << " ";
                // }
                // std::cout << std::endl;
                // }

                auto start = std::chrono::high_resolution_clock::now();
                ZDDNode* z = Search(matrix); 
                auto end = std::chrono::high_resolution_clock::now();

                printMemoryUsage(); 
                auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

                std::cout << "Search Compulation Time: " << duration.count() << " seconds." << std::endl;

                // 输出ZDD路径
                std::vector<std::string> paths = binaryTreePaths(z);
                
                // for (const std::string& path : paths) {
                //     std::cout << path << std::endl;
                // }
                std::cout<<"path num: "<< paths.size()<< "\n";

                //统计ZDD节点个数
                int count = countNodes(z);
                std::cout << "节点数量: " << count << std::endl;

                //releaseMemory();
                matrix.clear();
                Z.clear();
                C.clear();
            }    
        }
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
    return 0;
}