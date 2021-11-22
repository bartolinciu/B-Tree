#ifndef TREE_CPP
#define TREE_CPP

#include "tree.h"
#include <set>
#include <memory>
#include <list>
#include <iostream>


template<typename T>
BTree<T>::BTree( int order ){
	this->t = order;
}

template<typename T>
void BTree< T >::print(){
	if( root == nullptr )
		return;

	this->print( this->root, 0 );
	std::cout<<std::endl<<std::endl;

}

template<typename T>
void BTree< T >::print( std::shared_ptr< node_t > node, int depth ){
	if( node->children.empty() )
		for( const auto& key : node->keys ){
			for( int i = 0; i < depth; i++ )
				std::cout<<"\t";
			std::cout<<key<<std::endl;
		}
	else{
		auto child = node->children.begin();
		for( const auto& key : node->keys ){
			this->print( *child++, depth+1 );
			for( int i = 0; i < depth; i++ )
				std::cout<<"\t";
			std::cout<<key<<std::endl;
		}
		this->print( *child, depth+1 );
	}
}

template<typename T>
void BTree < T >::insert( T key ){
	if( !this->root ){
		this->root = std::shared_ptr< node_t >( new node_t );
		this->root->keys.insert( key );
		return;
	}
	if( this->root->keys.size() == this->t * 2 - 1 ){
		std::shared_ptr< node_t > newRoot( new node_t );
		this->root->parent = newRoot;
		newRoot->children.push_back( this->root );
		this->root->pos = newRoot->children.begin();
		this->split( this->root );
		this->root = newRoot;
	}
	this->insert( this->root, key );
}

template<typename T>
void BTree< T >::split( std::shared_ptr< node_t > node ){
	auto it = node->keys.begin();
	std::advance( it, this->t-1 );
	node->parent->keys.insert( *it );
	std::shared_ptr< node_t > sibling( new node_t );
	sibling->keys.insert(  std::next(it), node->keys.end() );
	node->keys.erase( it, node->keys.end() );

	if( !node->children.empty() ){
		auto it2 = node->children.begin();
		std::advance( it2, this->t + 1 );
		sibling->children.insert( sibling->children.end(), it2, node->children.end() );
		for( auto child = sibling->children.begin(); child != sibling->children.end(); child++ ){
			(*child)->pos = child;
			(*child)->parent = sibling;
		}
		node->children.erase( it2, node->children.end() );
	}
	
	node->parent->children.insert( std::next(node->pos), sibling );
	sibling->pos = std::next(node->pos);
	sibling->parent = node->parent;

}

template<typename T>
void BTree < T > ::insert( std::shared_ptr< node_t > node, T key ){
	if( node->children.empty() ){
		node->keys.insert( key );
	}else{
		auto it = node->children.begin();
		auto k = node->keys.begin();
		for( ; k != node->keys.end(); k++ ){
			std::cout<<node<<" "<<key<<std::endl;
			if( *k < key ){
				it++;
			}
			else{
				
				if( (*it)->keys.size() == 2 * this->t - 1 && !this->find( *it, key ) ){
					
					this->split( *it );
					std::cout<<*it<<std::endl;
					if( *--k < key )
						it++;
				}
				this->insert( *it, key );
				return;
			}
		}
		if( (*it)->keys.size() == 2 * this->t - 1 && !this->find( *it, key ) ){
					
					this->split( *it );
					std::cout<<*it<<std::endl;
					if( *--k < key )
						it++;
				}
				this->insert( *it, key );
		
	}
}

template<typename T>
bool BTree< T >::find( std::shared_ptr< node_t > node, T key ){
	if( node->keys.find( key ) != node->keys.end() )
		return true;
	if( !node->children.empty() ){
		auto child = node->children.begin();
		for( const auto& k : node->keys ){
			if( k < key )
				child++;
			else{
				return this->find( *child, key );
			}
		}
		return this->find( *child, key );
	}
	return false;
}

template<typename T>
void BTree< T >::erase( T key ){
	if(this->root == nullptr)
		return;
	if( !this->find( this->root, key ) )
		return;

	this->erase( this->root, key );
	if( this->root->keys.empty() || this->root->children.size() == 1 )
		this->root = *this->root->children.begin();
}

template<typename T>
void BTree< T >::erase( std::shared_ptr< node_t > node, T key ){
	if( node->children.empty() ){
		node->keys.erase( key );
		return;
	}	

	if( node->keys.find( key ) == node->keys.end() ){
		auto child = node->children.begin();
		for( const auto& k : node->keys ){
			if( k < key )
				child++;
			else{
				break;
			}
		}

		this->erase( this->fill( *child ), key );
		
		return;
	}else{
		auto child = node->children.begin();
		for( const auto& k : node->keys ){
			if( k < key )
				child++;
			else{
				break;
			}
		}
		if( (*child)->keys.size() >= this->t - 1 ){
			T pred = this->getRightMost( *child );
			this->erase( *child, pred );
			node->keys.insert( pred );
			node->keys.erase( key );
		}
		else if( (*std::next( child ))->keys.size() >= this->t - 1 ){
			auto sibling = std::next( child );
			T succ = this->getLeftMost( *sibling );
			this->erase( *sibling, succ );
			node->keys.insert( succ );
			node->keys.erase( key );
		}else{
			this->merge( *child, *std::next( (*child)->pos ) );
			this->erase( *child, key );
		}
	}

}

template<typename T>
auto BTree< T >::fill( std::shared_ptr< node_t > node ){
	if( node->keys.size() >= this->t - 1 )
		return node;
	std::cout<<"fill"<<std::endl;
	if( node->pos != node->parent->children.begin() && (*std::prev( node->pos ))->keys.size() >= this->t ){
		this->borrowFromPrev( node );
		return node;
	}else if( std::next( node->pos ) != node->parent->children.end() && (*std::next(node->pos))->keys.size() >= this->t ){
		borrowFromNext( node );
		return node;
	}else{
		if( std::next( node->pos ) == node->parent->children.end() ){
			auto prev = *std::prev( node->pos );
			this->merge( prev, node );
			return prev;
		}
		else{
			this->merge( node, *std::next( node->pos ) );
			return node;
		}

	}
}

template<typename T>
void BTree< T >::merge( std::shared_ptr < node_t > first, std::shared_ptr < node_t > second ){
	if( first->parent != second->parent || std::next( first->pos ) != second->pos )
		return;
	first->keys.insert( second->keys.begin(), second->keys.end() );

	for( auto& child: second->children )
		child->parent = first;


	first->children.insert( first->children.end(), second->children.begin(), second->children.end() );

	for( auto it = std::prev( first->children.end(), second->children.size() ); it != first->children.end(); it++ )
		(*it)->pos = it;
	
	auto it = std::next( first->parent->keys.begin(), std::distance( first->parent->children.begin(), first->pos ) );
	first->keys.insert( *it );
	first->parent->keys.erase( it );
	second->parent->children.erase( second->pos );
}

template<typename T>
T BTree< T >::getRightMost( std::shared_ptr< node_t > node ){
	if( node->children.empty() )
		return *std::prev( node->keys.end() );
	else
		return this->getRightMost( *std::prev( node->children.end() ) );
}

template<typename T>
T BTree< T >::getLeftMost( std::shared_ptr< node_t > node ){
	if( node->children.empty() )
		return *node->keys.begin();
	else
		return this->getRightMost( *node->children.begin() );
}

template<typename T>
void BTree< T >::borrowFromNext( std::shared_ptr< node_t > node ){
	auto sibling = *std::next( node->pos );
	auto it = std::next( node->parent->keys.begin(), std::distance( node->parent->children.begin(), node->pos ) );
	node->keys.insert( *it );
	node->parent->keys.erase( it );
	if( !node->children.empty() ){
		node->children.insert( node->children.end(), *sibling->children.begin() );
		sibling->children.erase( sibling->children.begin() );
		(*std::prev( node->children.end() ) )->parent = node;
		(*std::prev( node->children.end() ) )->pos = std::prev( node->children.end() );
	}

	node->parent->keys.insert( *sibling->keys.begin() );
	sibling->keys.erase( *sibling->keys.begin() );
}

template<typename T>
void BTree< T >::borrowFromPrev( std::shared_ptr< node_t > node ){
	auto sibling = *std::prev( node->pos );
	auto it = std::next( node->parent->keys.begin(), std::distance( node->parent->children.begin(), node->pos ) );
	node->keys.insert( *it );
	node->parent->keys.erase( it );

	if( !node->children.empty() ){
		node->children.insert( node->children.begin(), *std::prev(sibling->children.end()) );
		(*node->children.begin())->parent = node;
		(*node->children.begin())->pos = node->children.begin();
		sibling->children.erase( std::prev( sibling->children.end() ) );
	}
	node->parent->keys.insert( *std::prev( sibling->keys.end() ) );
	sibling->keys.erase( sibling->keys.end() );
}

#endif