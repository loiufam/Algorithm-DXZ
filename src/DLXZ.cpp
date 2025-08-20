#include "DancingLinks.h"

// 构造函数
DancingLinks::DancingLinks( int rows , int cols, const std::vector< std::vector<int>>& matrix)
    : ROWS(rows), COLS(cols), countNum(0), countSolution(0){
    T = new ZDDNode(-1, nullptr, nullptr, true);  // 初始化 T
    F = new ZDDNode(-2, nullptr, nullptr, true);  // 初始化 F

    ColIndex = new ColunmHeader[cols + 1]; //存储列（item），第一个是root
    RowIndex = new Node[rows]; //每个Node指向一个option
    root = &ColIndex[0];
    ColIndex[0].left = &ColIndex[COLS];  
    ColIndex[0].right = &ColIndex[1];  
    ColIndex[COLS].right = &ColIndex[0];  
    ColIndex[COLS].left = &ColIndex[COLS-1];  
    for( int i=1; i<cols; i++ )  
    {  
        ColIndex[i].left = &ColIndex[i-1];  
        ColIndex[i].right = &ColIndex[i+1];  
    }  
  
    for ( int i=0; i<=cols; i++ )  
    {  
        ColIndex[i].up = &ColIndex[i];  
        ColIndex[i].down = &ColIndex[i];  
        ColIndex[i].col = i;  
    }  
    ColIndex[0].down = &RowIndex[0];  
      
    for( int i = 0; i < rows; i++ ){
        for( int j = 0; j < cols ; j++ ) {  
            if(matrix[i][j] == 1){
                insert(  i , j+1 );  //行数与原矩阵相同，而列数加1
                //printf("插入元素坐标（%d, %d）\n", i, j);
            }
        }
    }
}

// 析构函数
DancingLinks::~DancingLinks()
{
    for( int i=1; i<=COLS; i++ )  
    {  
        Node* cur = ColIndex[i].down;  
        Node* del = cur->down;  
        while( cur != &ColIndex[i] )  
        {  
            delete cur;  
            cur = del;  
            del = cur->down;  
        }  
    }  
    delete[] RowIndex;  
    delete[] ColIndex; 

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

void DancingLinks::insert(int r, int c)
{
    Node* cur = &ColIndex[c];  
    ColIndex[c].size++;  
    Node* newNode = new Node( r, c );  
    while( cur->down != &ColIndex[c] && cur->down->row < r )  
        cur = cur->down;  
    newNode->down = cur->down;  
    newNode->up = cur;  
    cur->down->up = newNode;  
    cur->down = newNode;  
    if( RowIndex[r].right == NULL )  
    {  
        RowIndex[r].right = newNode;  
        newNode->left = newNode;  
        newNode->right = newNode;  
    }  
    else  
    {  
        Node* rowHead = RowIndex[r].right;  
        cur = rowHead;  
        while( cur->right != rowHead && cur->right->col < c )  
            cur = cur->right;  
        newNode->right = cur->right;  
        newNode->left = cur;  
        cur->right->left = newNode;  
        cur->right = newNode;  
    }
}

void DancingLinks::cover( int c )  //将c列移出列表，并修改其他受影响列的size
{  
    ColunmHeader* col = &ColIndex[c];  
    col->right->left = col->left;  
    col->left->right = col->right;  
    Node* curR, *curC;  
    curC = col->down;  
    while( curC != col )  //遍历要覆盖列下的元素
    {  
        Node* noteR = curC;  
        curR = noteR->right;  
        while( curR != noteR )  
        {  
            curR->down->up = curR->up;  
            curR->up->down = curR->down;  
            ColIndex[curR->col].size--;  
            curR = curR->right;  
        }  
        curC = curC->down;  
    }  
}  

void DancingLinks::uncover( int c )  
{  
    Node* curR, *curC;  
    ColunmHeader* col = &ColIndex[c];  
    curC = col->up;  
    while( curC != col )  
    {  
        Node* noteR = curC;  
        curR = curC->left;  
        while( curR != noteR )  
        {  
            ColIndex[curR->col].size++;  
            curR->down->up = curR;  
            curR->up->down = curR;  
            curR = curR->left;  
        }  
        curC = curC->up;  
    }  
    col->right->left = col;  
    col->left->right = col;  
}


void DancingLinks::columnToVector(std::vector<bool>& vec)
{
    ColunmHeader* cur = root;
    while(cur->right != root){
        //vec[cur->right->col-1] = true;
        vec[cur->right->col - 1] = true;
        cur = (ColunmHeader*)cur->right;
    }
}


bool areSubtreesEqual(ZDDNode* s, ZDDNode* t) {
    // 如果两个节点都是空，则它们是相同的
    if (!s && !t) return true;

    // 如果只有一个节点为空，则它们不同
    if (!s || !t) return false;

    // 比较当前节点的标签
    if (s->label != t->label) return false;

    // 递归比较左子树
    if (!areSubtreesEqual(s->lo, t->lo)) return false;

    // 递归比较右子树
    if (!areSubtreesEqual(s->hi, t->hi)) return false;

    // 如果所有条件都满足，则子树是相同的
    return true;
}

// 判断 t 是否是 s 的子树，并返回子树在父树中的根节点
ZDDNode* findSubtree(ZDDNode* s, ZDDNode* t) {
    if(!s) return nullptr;

    // 检查根节点是否相同
    if (areSubtreesEqual(s, t)) {
        return s; // 找到了子树，返回根节点
    }

    // 在左子树中查找
    ZDDNode* leftResult = findSubtree(s->lo, t);
    if (leftResult) {
        return leftResult; // 在左子树中找到了子树
    }

    // 在右子树中查找
    ZDDNode* rightResult = findSubtree(s->hi, t);
    if (rightResult) {
        return rightResult; // 在右子树中找到了子树
    }

    return nullptr; // 没有找到子树
}

void DancingLinks::printZDD(ZDDNode* node) {
    if (!node) return;

    std::queue<ZDDNode*> q;
    q.push(node);

    std::set<int> visited;
    while(!q.empty()) {
        ZDDNode* current = q.front();
        q.pop();

        if(!current->isTerminal && visited.find(current->label) == visited.end()){ //如果不是终端节点
            std::cout << "Label: " << current->label << " { ";
            std::cout << "Lo: " << (current->lo ? std::to_string(current->lo->label) : "null") 
                      << " Hi: " << (current->hi ? std::to_string(current->hi->label) : "null") << " }"<< std::endl;
            visited.insert(current->label);
        }

        if (current->lo) q.push(current->lo);
        if (current->hi) q.push(current->hi); 
    }
}

size_t DancingLinks::hashFunction(int r, ZDDNode* x, ZDDNode* y)
{
    return std::hash<int>()(r) ^ (std::hash<int>()(x->label) << 1) ^ (std::hash<int>()(y->label) << 2);
}

ZDDNode* DancingLinks::unique(int r, ZDDNode* x, ZDDNode* y)
{
    countNum++; //计数器加1
    //std::cout<< "第" << countNum << "次 " << "labe: " << r << std::endl;
    if (x == y) {
        return x;
    }
    
    std::size_t key = hashFunction(r, x, y);
    if (Z.find(key) != Z.end()) {
        return Z[key];
    }

    Z[key] = new ZDDNode(r, x, y); 
    return Z[key];  

    // if (Z.find(key) == Z.end()) {  //如果没有找到解
    //     ZDDNode* lo = x;
    //     ZDDNode* hi = y;

    //     //先检查下x和y是否都为终端节点
    //     if(x->isTerminal && y->isTerminal){
    //        Z[key] = new ZDDNode(r, lo, hi); 
    //        return Z[key];
    //     }

    //     //如果x，y都为分支节点,x指向的是已找到的解, 并且x，y都存在Z中
    //     if(!x->isTerminal && !y->isTerminal){
    //         //如果y存在缓存, 说明也存在Z中（DXZ）
    //         if (C.find(getColumnState()) != C.end()) {
    //             hi = C[getColumnState()];    
    //         }

            
    //        Z[key] = new ZDDNode(r, lo, hi); 
    //        return Z[key]; 
    //     }
        
    //     // 如果x或y是终端节点，尝试在另一个节点中找到匹配的终端节点
    //     if (x->isTerminal) {
    //         lo = F;
    //     } else if (y->isTerminal) { // 此时说明r覆盖了当前矩阵的所有列
    //         hi = T;
    //     }

    //     // 创建新的ZDDNode
    //     Z[key] = new ZDDNode(r, lo, hi);
    // }
    // return Z[key];
}

std::string DancingLinks::getColumnState() const {
    std::string columnState(COLS, '0'); // 初始化为全0字符串
    ColunmHeader* cur = (ColunmHeader*)root->right;
    while (cur != root) {
        columnState[cur->col - 1] = '1'; // 将能遍历到的列设置为1
        cur = (ColunmHeader*)cur->right;
    }
    return columnState;
}

ZDDNode* DancingLinks::search()
{
    if(root->right == root){
        // countSolution++;
        return T;
    }

    std::string columnState = getColumnState();
    //std::cout<< columnState << std::endl;
    // 查找缓存
    if (C.find(columnState) != C.end()) {
        // countSolution++;
        return C[columnState]; 
    }

    ColunmHeader* choose = (ColunmHeader*)root->right;  
    ColunmHeader* cur = choose;
    while( cur != root )  
    {   //选择元素最少的列
        if( choose->size > cur->size )  
            choose = cur;  
        cur = (ColunmHeader*)cur->right;  
    }  

    if( choose->size <= 0 ){
        return F;  
    } 
    ZDDNode* x = F;


    cover(choose->col);  //将选中列移出列表
    Node* curC = choose->up;  //curC用来遍历选中列的所有非零行(从下往上)
    while(curC != choose)  //相当于for r such that A[r, c]=1 do
    {
        //printColumnHeaders();
        Node* noteR = curC;  
        Node* curR = curC->right;  
        while( curR != noteR )  
        {  
            cover( curR->col );  
            curR = curR->right;  
        }
        ZDDNode* y = search();
        if(y->label != -2){
            x = unique(curC->row, x, y);
        }
       
        //printColumnHeaders();
        curR = noteR->left;  
        while( curR != noteR )  
        {  
            uncover( curR->col );  
            curR = curR->left;  
        }  

        curC = curC->up;
    }
    uncover(choose->col);  //回溯
    C[columnState] = x; // 结果存入缓存，实际上在Z中
    return x;
}

void DancingLinks::printTable(){
    std::cout << "Table:" << std::endl;
    for (const auto& pair : Z) {
        //size_t key = pair.first;
        ZDDNode* node = pair.second;
        // node
        std::cout << "Node in Z: " << std::endl;
        printZDD(node);
    }
}

void DancingLinks::printCache(){
    std::cout << "Cache:" << std::endl;
    for (const auto& pair : C) {
        std::string key = pair.first;
        ZDDNode* node = pair.second;
        // key
        std::cout << "Key: " << key << std::endl;
        // node
        std::cout << "Node in C: " << std::endl;
        printZDD(node);
    }
}


void DancingLinks::printColumnHeaders(){
    std::cout << "Column Headers:" << std::endl;
    ColunmHeader* current = static_cast<ColunmHeader*>(root->right);
    while (current != root) {
        std::cout << "Col: " << current->col << " Size: " << current->size << " ";
        Node* cur = current->down; 
        std::cout<<"rows: (";
        do{
            std::cout<< cur->row << " ";
            cur = cur->down;
        }while(cur!=current);
        std::cout<<")";
        current = static_cast<ColunmHeader*>(current->right);
    }
    std::cout << std::endl;
}


void DancingLinks::printRowNodes(){
    std::cout << "Row Nodes:" << std::endl;
    for (int i = 0; i < ROWS; ++i) {
        Node* current = RowIndex[i].right;
        if(current != nullptr){
            Node* Note = current;
            std::cout << "Row: " << current->row << " Col: " << current->col << " ";
            current = current->right;
            while (current != Note ) {
                std::cout << "Row: " << current->row << " Col: " << current->col << " ";
                current = current->right;
            }
            std::cout << std::endl;
        }
        else
            break;
    }
}


void DancingLinks::printRemainingColumns() {
        if(root->right == root){
          std::cout << "所有列已被覆盖" << std::endl;  
          return;
        }
        std::cout << "剩余列及其大小：" << std::endl;
        Node* cur = root->right;
        while (cur != root) {
            ColunmHeader* header = static_cast<ColunmHeader*>(cur);
            std::cout << "列: " << header->col << ", 大小: " << header->size << " 行：";
            Node* note = cur->down;
            while(note != cur){
                std::cout<< note->row <<" ";
                note = note->down;
            }
            std::cout<<std::endl;
            cur = cur->right;
        }
        return;
}

