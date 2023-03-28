#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    virtual void insertFix (Node<Key,Value>* p, Node<Key,Value>* n);
    virtual void removeFix (Node<Key, Value>* n, int diff);
    //virtual void rotateRight (Node<Key, Value>* g, Node<Key,Value>* p, Node<Key,Value>* n);
    //virtual void rotateLeft (Node<Key, Value>* g, Node<Key,Value>* p, Node<Key,Value>* n);
    virtual void rotateRight (Node<Key, Value>* g);
    virtual void rotateLeft (Node<Key, Value>* g);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    //If empty set n as root

    if (this->root_ == nullptr){
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    }
    // insert n by walking to a leaf and inserting the new node as its child
    else{
        Node<Key, Value>* temp = this->root_;
        while(true){
            if (temp->getKey() == new_item.first){
                temp->setValue(new_item.second);
                return;
            }
            else if (temp->getKey() < new_item.first){
                if (temp->right_ == nullptr){
                    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, static_cast<AVLNode<Key,Value>*>(temp));
                    temp->setRight(newNode);
                    break;
                }
                temp = temp->right_;
            }
            else{
                if (temp->left_ == nullptr){
                    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, static_cast<AVLNode<Key,Value>*>(temp));
                    temp->setLeft(newNode);
                    break;
                }
                temp = temp->left_;
            }
        }
        //If -1 now = 0
        if (static_cast<AVLNode<Key,Value>*>(temp)->getBalance() == -1){
            static_cast<AVLNode<Key,Value>*>(temp)->setBalance(0);
        }
        //If +1 now = 0. Done!
        else if(static_cast<AVLNode<Key,Value>*>(temp)->getBalance() == 1){
            static_cast<AVLNode<Key,Value>*>(temp)->setBalance(0);
        }
        // If 0 update b(p)
        else{
            if (temp->left_ != nullptr){
                static_cast<AVLNode<Key,Value>*>(temp)->setBalance(-1);
                insertFix(temp, temp->left_);
            }
            else if (temp->right_ != nullptr){
                static_cast<AVLNode<Key,Value>*>(temp)->setBalance(1);
                insertFix(temp, temp->right_);
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix (Node<Key,Value>* p, Node<Key,Value>* n){
	//If p is nullptr or parent(p) is nullptr, return
	if (p == nullptr || p->getParent() == nullptr){
			return;
	}
	//Let g = parent(p)
	Node<Key, Value>* g = p->getParent();

	if (p == g->left_){

			static_cast<AVLNode<Key,Value>*>(g)->updateBalance(-1);
			//b(g) == 0, return
			if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == 0){
					return;
			}
			// b(g) == -1, insertFix(g, p) // recurse
			else if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == -1){
					insertFix(g, p);
			}
			// b(g) == -2
			else if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == -2){

					if (n == p->left_){
	
							rotateRight(g);
							static_cast<AVLNode<Key,Value>*>(p)->setBalance(0);
							static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
					}

					else{
							rotateLeft(p);
							rotateRight(g);
							
							if (static_cast<AVLNode<Key,Value>*>(n)->getBalance() == -1){
									static_cast<AVLNode<Key,Value>*>(p)->setBalance(0);
									static_cast<AVLNode<Key,Value>*>(g)->setBalance(1);
									static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
							}
							else if (static_cast<AVLNode<Key,Value>*>(n)->getBalance() == 0){
									static_cast<AVLNode<Key,Value>*>(p)->setBalance(0);
									static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
									static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
							}
							else{
									static_cast<AVLNode<Key,Value>*>(p)->setBalance(-1);
									static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
									static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
							}
					}
			}
	}


	else if (p == g->right_){

			static_cast<AVLNode<Key,Value>*>(g)->updateBalance(1);

		if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == 0){
					return;
			}

		else if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == 1){
					insertFix(g, p);
			}

		else if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == 2){

					if (n == p->right_){
							rotateLeft(g);
							static_cast<AVLNode<Key,Value>*>(p)->setBalance(0);
							static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
					}
					else{
							rotateRight(p);
							rotateLeft(g);
							if (static_cast<AVLNode<Key,Value>*>(n)->getBalance() == 1){
									static_cast<AVLNode<Key,Value>*>(p)->setBalance(0);
									static_cast<AVLNode<Key,Value>*>(g)->setBalance(-1);
									static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
							}
							else if (static_cast<AVLNode<Key,Value>*>(n)->getBalance() == 0){
									static_cast<AVLNode<Key,Value>*>(p)->setBalance(0);
									static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
									static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
							}
							else{
									static_cast<AVLNode<Key,Value>*>(p)->setBalance(1);
									static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
									static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
							}
					}
			}
	}
}





/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    
    //Find n to remove by walking the tree
    if (this->root_ == nullptr){
        return;
    }
    Node<Key, Value>* temp = this->root_;
    while(temp != nullptr){
        if (temp->getKey() == key){
            break;
        }
        else if (temp->getKey()< key){
            //new temp is right_
            temp = temp->right_;
        }
        else{
            temp = temp->left_;
        }
    }
    if (temp == nullptr){
        //node not in tree, ignore
        return;
    }
    if (temp->left_ != nullptr && temp->right_ != nullptr){
        AVLTree<Key, Value>::nodeSwap(static_cast<AVLNode<Key,Value>*>(temp), static_cast<AVLNode<Key,Value>*>(this->predecessor(temp)));
    }
    Node<Key, Value>*& n = temp;
    Node<Key, Value>* p = n->getParent();
    int diff = 0;
    if (p != nullptr){

        if (n == p->left_){
            diff = 1;
           if (n->left_ != nullptr){
               p->left_ = n->left_;
               n->left_->parent_ = p;
           }
           else if (n->right_ != nullptr){
               p->left_ = n->right_;
               n->right_->parent_ = p;
           }
           else{
               p->left_ = nullptr;
           }
        }
        else if (n == p->right_){
            diff = -1;
            
            if (n->left_ != nullptr){
               p->right_ = n->left_;
           }
           else if (n->right_ != nullptr){
               p->right_ = n->right_;
               n->right_->parent_ = p;
           }
           else{
               p->right_ = nullptr;
           }
        }
    }
    
    
    else{
        if (n->left_ != nullptr){
            this->root_ = n->left_;
            n->left_->parent_ = nullptr;
            static_cast<AVLNode<Key,Value>*>(n->left_)->setBalance(0);
        }
        else if (n->right_ != nullptr){
            this->root_ = n->right_;
            n->right_->parent_ = nullptr;
            static_cast<AVLNode<Key,Value>*>(n->right_)->setBalance(0);
        }
        else{
            this->root_ = nullptr;
        }
    }
        delete n;// delete only after updating pointers

        // fix tree
        removeFix(p, diff);
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight (Node<Key,Value>* g){
    if (g == this->root_){
        this->root_ = g->left_;
        g->left_->setParent(nullptr);
        Node<Key, Value>* temp = g->left_->right_;
        g->left_->setRight(g);
        g->setParent(g->left_);
        g->setLeft(temp);
        if (temp!= nullptr){
            temp->setParent(g);
        }

    }
    else{
        BinarySearchTree<Key, Value>::nodeSwap(g, g->left_);
        Node<Key, Value>* temp = g->left_;
        g->left_ = g->right_;
        g->right_ = g->parent_->right_;
        if (g->right_ != nullptr){
            g->right_->parent_ = g;
        }
        g->parent_->right_ = g;
        g->parent_->left_ = temp;
        if (temp != nullptr){
            temp->parent_ = g->parent_;
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft (Node<Key,Value>* g){
    if (g == this->root_){
        this->root_ = g->right_;
        g->right_->setParent(nullptr);
        Node<Key, Value>* temp = g->right_->left_;
        g->right_->setLeft(g);
        g->setParent(g->right_);
        g->setRight(temp);
        if (temp!= nullptr){
            temp->setParent(g);
        }

    }
    else{
        BinarySearchTree<Key, Value>::nodeSwap(g, g->right_);
        Node<Key, Value>* temp = g->right_;
        g->right_ = g->left_;
        g->left_ = g->parent_->left_;
        if (g->left_ != nullptr){
            g->left_->parent_ = g;
        }
        g->parent_->left_ = g;
        g->parent_->right_ = temp;
        if (temp != nullptr){
            temp->parent_ = g->parent_;
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(Node<Key, Value>* n, int diff){
    
    // return if n == nullptr
    if (n == nullptr){
        return;
    }
    int ndiff = 0;
    Node<Key, Value>* p = n->getParent();

    if (p != nullptr){
        if (p->left_ == n){
            ndiff = 1;
        }
        else{
            ndiff = -1;
        }
    }
	// make the appropriate rotations for each of the cases eg. zigzig 
        if (((int) static_cast<AVLNode<Key,Value>*>(n)->getBalance()) + diff == -2){

            Node<Key, Value>* c = n->left_;

            if (static_cast<AVLNode<Key,Value>*>(c)->getBalance() == -1){

                rotateRight(n);
                static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
                static_cast<AVLNode<Key,Value>*>(c)->setBalance(0);
                removeFix(p, ndiff);
            }
            
            else if (static_cast<AVLNode<Key,Value>*>(c)->getBalance() == 0){
                rotateRight(n);
                static_cast<AVLNode<Key,Value>*>(n)->setBalance(-1);
                static_cast<AVLNode<Key,Value>*>(c)->setBalance(1);
                return;
            }
            else if (static_cast<AVLNode<Key,Value>*>(c)->getBalance() == 1){
                Node<Key, Value>* g = c->right_;
                rotateLeft(c);
                rotateRight(n);
                if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == 1){
                    static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
                    static_cast<AVLNode<Key,Value>*>(c)->setBalance(-1);
                    static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
                }
                else if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == 0){
                    static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
                    static_cast<AVLNode<Key,Value>*>(c)->setBalance(0);
                    static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
                }
                else if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == -1){
                    static_cast<AVLNode<Key,Value>*>(n)->setBalance(1);
                    static_cast<AVLNode<Key,Value>*>(c)->setBalance(0);
                    static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
                }
                return;
            }
        }
        
        else if (((int) static_cast<AVLNode<Key,Value>*>(n)->getBalance()) + diff == -1){
            static_cast<AVLNode<Key,Value>*>(n)->setBalance(-1);
            return;
        }
        else if (((int) static_cast<AVLNode<Key,Value>*>(n)->getBalance()) + diff == 0){
            static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
            removeFix(p, ndiff);
        }
    

        else if (((int) static_cast<AVLNode<Key,Value>*>(n)->getBalance()) + diff == 2){
            Node<Key, Value>* c = n->right_;


            if (static_cast<AVLNode<Key,Value>*>(c)->getBalance() == 1){
                // rotateRight(n)
                rotateLeft(n);
                static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
                static_cast<AVLNode<Key,Value>*>(c)->setBalance(0);
                removeFix(p, ndiff);
            }
            

            else if (static_cast<AVLNode<Key,Value>*>(c)->getBalance() == 0){

                rotateLeft(n);
                static_cast<AVLNode<Key,Value>*>(n)->setBalance(1);
                static_cast<AVLNode<Key,Value>*>(c)->setBalance(-1);
                return;
            }

            else if (static_cast<AVLNode<Key,Value>*>(c)->getBalance() == -1){

                Node<Key, Value>* g = c->left_;
                rotateRight(c);
                rotateLeft(n);
                if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == -1){
                    static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
                    static_cast<AVLNode<Key,Value>*>(c)->setBalance(1);
                    static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
                }
                else if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == 0){
                    static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
                    static_cast<AVLNode<Key,Value>*>(c)->setBalance(0);
                    static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
                }
                else if (static_cast<AVLNode<Key,Value>*>(g)->getBalance() == 1){
                    static_cast<AVLNode<Key,Value>*>(n)->setBalance(-1);
                    static_cast<AVLNode<Key,Value>*>(c)->setBalance(0);
                    static_cast<AVLNode<Key,Value>*>(g)->setBalance(0);
                }
                return;
            }
        }
        
        else if (((int) static_cast<AVLNode<Key,Value>*>(n)->getBalance()) + diff == 1){
            static_cast<AVLNode<Key,Value>*>(n)->setBalance(1);
            return;
        }
        else if (((int) static_cast<AVLNode<Key,Value>*>(n)->getBalance()) + diff == 0){
            static_cast<AVLNode<Key,Value>*>(n)->setBalance(0);
            removeFix(p, ndiff);
        }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif