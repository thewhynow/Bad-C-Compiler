#ifndef BST_H
#define BST_H
#include <stdlib.h>
#include <stdbool.h>
#include "../scope_info/scope_info.h"


#define T ScopeDeclaration*
#define K const char*


// returns a > b
typedef bool(*KeyGreaterThan_t)(K, K);
// returns a < b
typedef bool(*KeyLessThan_t)(K, K);

typedef struct TreeNode TreeNode;
struct TreeNode {
    TreeNode* left;
    TreeNode* right;

    T data;
    K key;
};

struct BinaryTree {
    KeyGreaterThan_t kGreater;
    KeyLessThan_t kLess;

    TreeNode* node;
};

typedef struct BinaryTree BinaryTree;

BinaryTree BTree_make(KeyGreaterThan_t keyGreater, KeyLessThan_t keyLess);

T* BTree_find(struct BinaryTree* this, K key);

T* BTree_insert(struct BinaryTree* this, K key, T val);

void BTree_destroy(struct BinaryTree* this);

#endif