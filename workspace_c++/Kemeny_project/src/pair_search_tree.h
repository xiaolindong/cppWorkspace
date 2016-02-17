#ifndef PAIR_SEARCH_TREE_H_
#define PAIR_SEARCH_TREE_H_


#include "ranking.h"
#include "Functions.h"



struct tree_node
{
	public:
		tree_node* left;
		tree_node* right;
		pair<int, int> key;
		tree_node(pair<int,int> k):left(NULL), right(NULL){
			if (k.first < k.second){
				key.first = k.first ;
				key.second = k.second;
			}
			else if (k.first > k.second){
				key.first = k.second;
				key.second = k.first;

			}

		}
};

class Tree{
	tree_node* root;

	public:
	Tree(){root = NULL;};
	~Tree(){destroy_tree(root);};
	void insert();
	tree_node* TreeSearch(tree_node* x, pair<int,int>& t);
	void destroy_tree(tree_node *x);

};

#endif /* PAIR_SEARCH_TREE_H_ */
