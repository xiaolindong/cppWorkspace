/*
 * heuristic_solver.cpp
 *  Created on: Jan 14, 2014
 *  Author: eia2
 */
#include "pair_search_tree.h"

void Tree::destroy_tree(tree_node* x){
	if (x != NULL){
		destroy_tree(x->left);
		destroy_tree(x->right);
		delete x;
	}
		
}

void Tree::insert(){

}


tree_node* Tree::TreeSearch(tree_node* x, pair<int,int>& k){

	if ( (x == NULL) || (x->key.first == k.first && x->key.second == k.second ))
		return x;
	if (x->key.first < k.first || (x->key.first == k.first && x->key.second < k.second))
		return TreeSearch(x->left, k);
	else 
		return TreeSearch(x->right, k);
 
}
