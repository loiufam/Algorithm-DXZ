#include "../include/DancingLinks.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <chrono>

namespace fs = std::filesystem;

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

// 将原始数据处理成 0-1 矩阵 处理第一个数据集
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

void extractCR(const std::string& line, int& c, int& r){
    std::istringstream iss(line);
    std::string token;

    iss >> c;
    iss >> r;
}

bool startsWith(const std::string& str, char ch) {
    return !str.empty() && str[0] == ch;
}

//处理第二个数据集
std::vector<std::vector<int>> proFileToMat(const fs::path& filename, int& r, int& c){ 
    std::ifstream file(filename.string());
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件");
    }

    std::string line;
    std::getline(file, line);  // 读取第一行

    int n, m;
    extractCR(line, n, m);
    r = m;
    c = n;

    // 动态分配二维向量
    std::vector<std::vector<int>> matrix(r, std::vector<int>(c, 0));

    // 提取文件名
    fs::path fileName = filename.filename();
    if(startsWith(fileName.string(), 's')){
        std::cout<<"该文件名以s开头"<<std::endl;
        int row = 0;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            // std::string token;
            // iss >> token;   //跳过第一个字符
            int col;
            while (iss >> col) {
                if (col > 0 && col <= n) {  // 确保列索引在有效范围内
                    matrix[row][col - 1] = 1;  // 由于数组索引从0开始，所以减1
                }
            }
            ++row;
        }
        file.close();
    }else
    {
        std::cout<<"该文件名不以s开头"<<std::endl;
        int row = 0;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;   //跳过第一个字符
            iss >> token;  //跳过第二个字符
            int col;
            while (iss >> col) {
                if (col > 0 && col <= n) {  // 确保列索引在有效范围内
                    matrix[row][col - 1] = 1;  // 由于数组索引从0开始，所以减1
                }
            }
            ++row;
        }
        file.close();
    }
    return matrix;
}

//输出ZDD路径
void findPaths(ZDDNode* node, std::string path, std::vector<std::string>& paths) {
    if (!node) return; // 如果节点为空，直接返回

    // 将当前节点的值添加到路径中
    path += std::to_string(node->label);

    // 如果是叶子节点，保存路径
    if (node->isTerminal) {
        if(node->label == -1){ //T
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

vector<vector<int>> TranFileToMatrix(const std::string& filename, int& r, int& c) { 
    std::ifstream file(filename);
    if(!file.is_open()) {
        throw std::runtime_error("无法打开文件");
    }

    std::string line;
    std::getline(file, line);  // 读取第一行
    std::istringstream iss(line);
    int n, m;
    iss >> n >> m;  // 假设第一行格式为 "n m"
    c = n;
    r = m;

    vector<vector<int>> tmp_matrix(r, vector<int>(c, 0));

    int row = 0;
    while (std::getline(file, line))
    {
        std::istringstream rowStream(line);
        int col, count;

        // 读取该行“1”的个数
        rowStream >> count;

        for (int i = 0; i < count; ++i) {
            rowStream >> col;  // 读取列索引
            if (col > 0 && col <= n) {  // 确保列索引在有效范围内
               tmp_matrix[row][col - 1] = 1;  // 由于数组索引从0开始，所以减1
            }
        }
        row++;
    }
    
    file.close();
    return tmp_matrix;
}

// 主函数
int main() {

    try {
        // 文件夹路径
        const std::string folderPath1 = "/Users/luoyaohui/VSCodeProjects/code_projects/algorithmLab/DLX/exact_cover_benchmark";
        const std::string folderPath2 = "/Users/luoyaohui/VSCodeProjects/code_projects/algorithmLab/DLX/set_partitioning_benchmarks";
        const std::string folderd3x = "../../SDX/data/dataset_d3x";
        //遍历文件夹
        for (const auto& entry : fs::directory_iterator(folderd3x)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt"){
                
                int r;
                int c;
                std::cout << "当前文件: " << entry.path().filename().string() << std::endl;
                // std::vector<std::vector<int>> X = processFileToMatrix(entry.path().string(), r, c);
                std::vector<std::vector<int>> X = TranFileToMatrix(entry.path().string(), r, c);
 
                {
              
                DancingLinks dlm(r, c, X);
                //dlm.printColumnHeaders();
                //dlm.printRowNodes();
                
                auto start = std::chrono::high_resolution_clock::now();
                ZDDNode* z = dlm.search(); 
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

                std::cout << "Search Compulation Time: " << duration.count() << " seconds." << std::endl;


                
                //统计ZDD节点个数(使用缓存时统计)
                // std::cout << "节点数量: " << dlm.countNum << std::endl;

                //std::vector<std::string> paths = binaryTreePaths(z);
                // std::cout<<"solution nums: "<< dlm.countSolution<<std::endl; // 不采用缓存时统计
                // for (const auto& path : paths) {
                //     std::cout << path << std::endl;
                // }
                std::cout<<std::endl;
                }
            }    
        }
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
    return 0;
}
