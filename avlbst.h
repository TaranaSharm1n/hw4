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
* An explicit constructor to initialize the elements by calling the base class constructor
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

    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
    void rebalance(AVLNode<Key, Value>* node, int change, bool growing);


};


template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(
    AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* newRoot = node->getRight();
    AVLNode<Key, Value>* middle = newRoot->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    // Connect the new subtree root to the old parent.
    if (parent == nullptr) {
        this->root_ = newRoot;
    } else if (parent->getLeft() == node) {
        parent->setLeft(newRoot);
    } else {
        parent->setRight(newRoot);
    }

    newRoot->setParent(parent);

    // Transfer the middle subtree.
    node->setRight(middle);

    if (middle != nullptr) {
        middle->setParent(node);
    }

    // Move the old root below the new root.
    newRoot->setLeft(node);
    node->setParent(newRoot);

    // Update balances using right-height minus left-height.
    node->setBalance(
        static_cast<int>(node->getBalance()) - 1 -
        std::max(0, static_cast<int>(newRoot->getBalance())));

    newRoot->setBalance(
        static_cast<int>(newRoot->getBalance()) - 1 +
        std::min(0, static_cast<int>(node->getBalance())));

    return newRoot;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(
    AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* newRoot = node->getLeft();
    AVLNode<Key, Value>* middle = newRoot->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    if (parent == nullptr) {
        this->root_ = newRoot;
    } else if (parent->getLeft() == node) {
        parent->setLeft(newRoot);
    } else {
        parent->setRight(newRoot);
    }

    newRoot->setParent(parent);

    node->setLeft(middle);

    if (middle != nullptr) {
        middle->setParent(node);
    }

    newRoot->setRight(node);
    node->setParent(newRoot);

    node->setBalance(
        static_cast<int>(node->getBalance()) + 1 -
        std::min(0, static_cast<int>(newRoot->getBalance())));

    newRoot->setBalance(
        static_cast<int>(newRoot->getBalance()) + 1 +
        std::max(0, static_cast<int>(node->getBalance())));

    return newRoot;
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(
    AVLNode<Key, Value>* node, int change, bool growing)
{
    while (node != nullptr) {
        node->updateBalance(change);

        int balance = node->getBalance();
        AVLNode<Key, Value>* subtreeRoot = node;

        if (balance == 0) {
            // After insertion, this subtree did not grow taller.
            if (growing) {
                return;
            }
        } else if (balance == -1 || balance == 1) {
            // After removal, this subtree did not get shorter.
            if (!growing) {
                return;
            }
        } else {
            if (balance == -2) {
                // Left-right case: straighten the bend first.
                if (node->getLeft()->getBalance() > 0) {
                    rotateLeft(node->getLeft());
                }

                subtreeRoot = rotateRight(node);
            } else {
                // Right-left case: straighten the bend first.
                if (node->getRight()->getBalance() < 0) {
                    rotateRight(node->getRight());
                }

                subtreeRoot = rotateLeft(node);
            }

            // Insertion needs no further repairs after rotation.
            if (growing) {
                return;
            }

            // After deletion, a nonzero balance here means
            // the rotated subtree kept its previous height.
            if (subtreeRoot->getBalance() != 0) {
                return;
            }
        }

        AVLNode<Key, Value>* parent = subtreeRoot->getParent();

        if (parent == nullptr) {
            return;
        }

        // Determine how this subtree's height change
        // affects its parent's balance.
        if (subtreeRoot == parent->getLeft()) {
            change = growing ? -1 : 1;
        } else {
            change = growing ? 1 : -1;
        }

        node = parent;
    }
}

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(
    const std::pair<const Key, Value>& new_item)
{
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(
            new_item.first, new_item.second, nullptr);
        return;
    }

    AVLNode<Key, Value>* curr =
        static_cast<AVLNode<Key, Value>*>(this->root_);

    AVLNode<Key, Value>* parent = nullptr;

    while (curr != nullptr) {
        parent = curr;

        if (new_item.first == curr->getKey()) {
            curr->setValue(new_item.second);
            return;
        } else if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        } else {
            curr = curr->getRight();
        }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(
        new_item.first, new_item.second, parent);

    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);

        // Left subtree grew, so balance decreases.
        rebalance(parent, -1, true);
    } else {
        parent->setRight(newNode);

        // Right subtree grew, so balance increases.
        rebalance(parent, 1, true);
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* target =
        static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (target == nullptr) {
        return;
    }

    if (target->getLeft() != nullptr &&
        target->getRight() != nullptr) {

        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(
                this->predecessor(target));

        nodeSwap(target, pred);
    }

    // After the swap, target has at most one child.
    AVLNode<Key, Value>* child = target->getLeft();

    if (child == nullptr) {
        child = target->getRight();
    }

    AVLNode<Key, Value>* parent = target->getParent();
    int change = 0;

    if (parent == nullptr) {
        this->root_ = child;
    } else if (parent->getLeft() == target) {
        parent->setLeft(child);

        // Left subtree shrank, so balance increases.
        change = 1;
    } else {
        parent->setRight(child);

        // Right subtree shrank, so balance decreases.
        change = -1;
    }

    if (child != nullptr) {
        child->setParent(parent);
    }

    delete target;

    if (parent != nullptr) {
        rebalance(parent, change, false);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(
    AVLNode<Key, Value>* n1,
    AVLNode<Key, Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);

    int8_t tempBalance = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempBalance);
}


#endif
