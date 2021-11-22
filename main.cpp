#include <iostream>
#include "tree.h"



int main(){
	BTree<int> tree( 3 );
	tree.insert(1);
	tree.print();
	tree.insert(7);
	tree.print();
	tree.insert(2);
	tree.print();
	tree.insert(25);
	tree.print();
	tree.insert(4);
	tree.print();
	std::cout<<"--------------"<<std::endl;
	tree.insert(11);
	tree.print();
	tree.insert(73);
	tree.print();
	tree.insert(41);
	tree.print();
	tree.erase(41);
	tree.print();
std::cout<<"--------------"<<std::endl;
	
}