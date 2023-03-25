#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPaths(Node * root){
    // if the node is a leaf, return true
    if (!root->left && !root->right) {
        return true;
    }
    
    // Recursively check the left and right subtrees
    int leftPathLength = 0, rightPathLength = 0;
    if (root->left) {
        if (!equalPaths(root->left)) {
            return false;
        }
				// add one to the height acconting for the current node
        leftPathLength = getHeight(root->left) + 1;
    }
    if (root->right) {
        if (!equalPaths(root->right)) {
            return false;
        }
        rightPathLength = getHeight(root->right) + 1;
    }
    
    // Check if the left and right subtrees have the same path length
    return (leftPathLength == rightPathLength);
}

int getHeight(Node *node) {
    // If the node is null, then it has a depth of 0
    if (node == nullptr) {
        return 0;
    }
    //  find the max depth of the left and right subtrees add 1 for the current node
    int leftDepth = getHeight(node->left);
    int rightDepth = getHeight(node->right);
		//find the number to add
		int addNum = leftDepth;
		if (leftDepth < rightDepth){
			// return whichever is bigger
			addNum = rightDepth;
		}
    return 1 + addNum;
}