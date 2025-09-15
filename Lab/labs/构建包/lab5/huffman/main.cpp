#include "util.hpp"




TreeNode* huffman(vector<TreeNode*>& tree) {
    int n = tree.size();
    // 创建一个小顶堆
    priority_queue<TreeNode*, vector<TreeNode*>, TreeNode> q;
    // 把 tree 放入 q 中
    for (int i = 0; i < n; i++) {
        q.push(tree[i]);
    }

    // 请注意，非叶子结点的 symbol 不需要赋值
    // 请在这里完成你的代码

    return q.top();
}

/**
 * @brief 销毁树结构
 * 
 * @param root 
 */
void destory(TreeNode* root) {
    if (root == NULL)
        return;
    destory(root->left);
    destory(root->right);
    delete root;
}

/**
 * @brief 对 symbols 中的字符，根据 freqs 中的频率构建哈夫曼树，返回树的根结点
 * 
 * @param symbols 
 * @param freqs 
 * @return TreeNode* 
 */
TreeNode* huffman(string symbols, vector<double> freqs) {
    vector<TreeNode *> tree;
    for (size_t i = 0; i < freqs.size(); i++) {
        tree.push_back(new TreeNode(symbols[i], freqs[i]));
    }
    return huffman(tree);
}

int main() {
    string symbols = "algorithm";
    vector<double> freqs = { 17, 19, 20, 5, 9, 34, 78, 45, 12 };

    // 构建哈夫曼树
    TreeNode *root = huffman(symbols, freqs);
    // 打印哈夫曼树结构
    print_tree(root);
    // 输出每个字符的 Huffman 编码
    out_symbol_code(root);
    
    destory(root);
    return 0;
}
