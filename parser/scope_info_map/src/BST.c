#include "../include/BST.h"



BinaryTree BTree_nodeFind(BinaryTree* this, K key);
void BTree_nodeDestroy(TreeNode* node);
TreeNode* BTree_makeNode(K key, T val);

BinaryTree BTree_make(KeyGreaterThan_t keyGreater, KeyLessThan_t keyLess){
    return (BinaryTree){.kGreater = keyGreater, .kLess = keyLess, .node = NULL};
}

T* BTree_find(BinaryTree* this, K key){
    BinaryTree node = BTree_nodeFind(this, key);
    return (node.node == NULL) ? NULL : &node.node->data;
}

BinaryTree BTree_nodeFind(BinaryTree* this, K key){
    if (this->kGreater(this->node->key, key)){
        return BTree_nodeFind(&(BinaryTree){.kGreater = this->kGreater, .kLess = this->kLess, .node = this->node->left}, key);
    } else
    if (this->kLess(this->node->key, key)){
        return BTree_nodeFind(&(BinaryTree){.kGreater = this->kGreater, .kLess = this->kLess, .node = this->node->right}, key);
    } else
    if (this->node != NULL){
        return *this;
    }
    else
        return (BinaryTree){.kGreater = this->kGreater, .kLess = this->kLess, .node = NULL};
}

TreeNode* BTree_makeNode(K key, T val){
    TreeNode* new = malloc(sizeof(TreeNode));
    new->data = val;
    new->key = key;
    new->left = new->right = NULL;

    return new;
}

// assumes the key does not already exist in the tree
T* BTree_insert(BinaryTree* this, K key, T val){
    TreeNode* tracker = this->node;

    if (tracker == NULL){
        this->node = BTree_makeNode(key, val);
        this->node->left = NULL;
        this->node->right = NULL;
        return &this->node->data;
    }

    for (; tracker->left != NULL && tracker->right != NULL;){
        if (this->kGreater(key, tracker->key))
            tracker = tracker->right;    
        else // since we are assuming the key does not exist, we do not need to check for that case
            tracker = tracker->left;
    }


    if (tracker->left == NULL && tracker->right == NULL){
        if (this->kGreater(key, tracker->key)){
            tracker->right = BTree_makeNode(key, val);
            tracker = tracker->right;
        }
        else{
            tracker->left = BTree_makeNode(key, val);
            tracker = tracker->left;
        }
    } else
    if (this->kGreater(key, tracker->key)){
        if (tracker->right == NULL){
            tracker->right = BTree_makeNode(key, val);
            tracker = tracker->right;
        } 
        else
            return BTree_insert(&(BinaryTree){.kGreater = this->kGreater, .kLess = this->kLess, .node = tracker->right}, key, val);
    } 
    else {
        if (tracker->left == NULL){
            tracker->left = BTree_makeNode(key,  val);
            tracker = tracker->left;
        }
        else
            return BTree_insert(&(BinaryTree){.kGreater = this->kGreater, .kLess = this->kLess, .node = tracker->left}, key, val);
    }

    return &tracker->data;
}

void BTree_destroy(BinaryTree* this){
    BTree_nodeDestroy(this->node);
}

void BTree_nodeDestroy(TreeNode* node){
    if (node->left != NULL)
        BTree_nodeDestroy(node->left); else
    if (node->right != NULL)
        BTree_nodeDestroy(node->right);
    
    free(node);
}