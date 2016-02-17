/*
 * Tree.h
 *
 *  Created on: Dec 3, 2015
 *      Author: eia2
 */

#ifndef TREE_H_
#define TREE_H_
#include "master_header.h"
#include "CycleManager.h"

using namespace std;

struct TreeNode{
	int cycle_index;
	cycle c;
	vector<TreeNode> child; 
}
class Tree{
private:
	TreeNode* root;
public:
	void insert(TreeNode& n, int v);

};

#endif /* EDGEMANAGER_H_ */
