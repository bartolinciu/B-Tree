#ifndef TREE_H
#define TREE_H

#include <set>
#include <memory>
#include <list>

template< typename T >
class BTree{
	int t;

	struct node_t{
		std::shared_ptr< node_t >parent; 
		std::set<T> keys;
		std::list< std::shared_ptr< node_t > >children;
		typename std::list< std::shared_ptr< node_t > >::iterator pos;
	};

	void print( std::shared_ptr< node_t > node, int depth );
	void split( std::shared_ptr< node_t > node );
	void insert( std::shared_ptr< node_t > node, T key );
	void erase( std::shared_ptr< node_t > node, T key );

	bool find( std::shared_ptr< node_t > node, T key );
	auto fill( std::shared_ptr< node_t > node );
	void merge( std::shared_ptr < node_t > first, std::shared_ptr < node_t > second );
	T getLeftMost( std::shared_ptr< node_t > node );
	T getRightMost( std::shared_ptr< node_t > node );
	void borrowFromNext( std::shared_ptr< node_t > node );
	void borrowFromPrev( std::shared_ptr< node_t > node );

	std::shared_ptr< node_t > root;

public:
	BTree( int order = 10 );
	void insert( T key );
	void erase( T key );
	void print();

};

#include "tree.cpp"

#endif