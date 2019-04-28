/*
 Data Structures and Agorithms
 Spring Semester 2019
 Bonus Exercise II: AVL tree implementation
 */

#include <iostream>
#include <cassert>
#include <algorithm>
#include <string>
#include <cctype>

//Class declaration
template <typename T>
class AvlTree{
public:

    //subordinate class AVL node
    class Node{
    public:
        Node* left = nullptr;
        Node* right = nullptr;

        T value = 0;
        int balance = 0;

        Node(T v): value (v){
        }

        ~Node(){
            if (left != nullptr) delete left;
            if (right != nullptr) delete right;
        }
    };

    //empty constructor
    AvlTree() {}

    //destructor
    ~AvlTree(){
        if (root != nullptr) delete root;
    };

    //recursive height calculation
    int height(Node* node){
        if(node == nullptr){
            return 0;
        }
        else{
            int left = height(node->left);
            int right = height(node->right);
            return std::max(left, right) + 1;
        }
    }

    //Backtracking helper function
    Node* find_parent(Node* const node){
        if (node == root){
            return nullptr;
        }

        Node* current = root;
        Node* parent = nullptr;
        while (current != node){
            parent = current;
            //go left, value of node smaller than current
            if (current->value > node->value){
                current = current->left;
            }
            //go right, value of node greater than current
            else if (current->value < node->value){
                current = current->right;
            }
        }
        return parent;
    }

    //for assertion
    bool check_bal(Node* const node){
        int right_bal = height(node->right) - height(node->left);
        return (right_bal == node->balance);
    }

    //helper function to calculate balance after an insertion or deletion
    void calc_bal(Node* & node){
        if(node != nullptr){
            node->balance = height(node->right) - height(node->left);
        }
    }

    //rebalancing function, checks AVL condition and performs rotations in case of error
    void rebalance(Node* & node){
        Node* current = node;
        Node* son = nullptr;

        while (current != nullptr){
            calc_bal(current);

            if(current->balance > 1){ //I
                if (son->balance > 0){//I.a
                    current = LeftRot(current);
                }
                else{ //I.b
                    current = RightLeftRot(current);
                }
            }
            else if (current->balance < -1){//II
                if (son->balance < 0){
                    current = RightRot(current);
                }
                else{
                    current = LeftRightRot(current);
                }
            }
            son = current;
            current = find_parent(current);
        }
    }

    //rotations
    Node* RightRot (Node* & node){//node that is imbalanced (double negative, on left side)
        Node* parent = find_parent(node);
        Node* temp = node->left;
        node->left = temp->right;
        temp->right = node;

        if (node == root){
            root = temp;
        }
        else{
            if (temp->value < parent->value){
                parent->left = temp;
            }
            else {
                parent->right = temp;
            }
        }

        calc_bal(node);
        calc_bal(temp);

        return temp;
    }

    Node* LeftRot (Node* & node){
        Node* parent = find_parent(node);
        Node* temp = node->right;
        node->right = temp->left;
        temp->left = node;

        if (node == root){
            root = temp;
        }
        else {
            if (temp->value < parent->value) {
                parent->left = temp;
            } else {
                parent->right = temp;
            }
        }

        calc_bal(node);
        calc_bal(temp);

        return temp;
    }

    Node * LeftRightRot(Node * & node){
        Node * temp = LeftRot(node->left);
        calc_bal(temp->left);

        node->left = temp;

        return RightRot(node);
    }

    Node * RightLeftRot(Node * & node){
        Node * temp = RightRot(node->right);
        calc_bal(temp->right);

        node->right = temp;

        return LeftRot(node);
    }

    // insert key into the AVL tree
    void insert(T key){
        Node* parent = nullptr;
        Node* current = root;

        //find location to insert
        while (current != nullptr){
            parent = current;
            //go left, smaller than current
            if (key < current->value){
                current = current->left;
            }

            //go right, greater than current
            else if (key > current->value){
                current = current->right;
            }
        }

        if (parent == nullptr){//empty tree
            root = new Node (key);
        }
        else {//tree with nodes
            if (key < parent->value){
                parent->left = new Node (key);
            }
            else {
                parent->right = new Node (key);
            }
            rebalance(parent); //restore AVL condition
        }

    }

    //helper function for remove
    bool is_leaf(Node* const node){
        return (node->left == nullptr && node->right == nullptr);
    }

    //helper function to find symmetric successor of a node
    Node* sym_suc(Node* node){
        Node* current = node->right;

        if (current == nullptr){
            return nullptr;
        }
        else if (current->left == nullptr){
            return current;
        }
        else {
            return sym_suc(current->left);
        }
    }

    // remove key from the AVL tree
    void remove(T key){
        Node * current = root;
        Node * parent = nullptr;

        //a) find node to delete
        while (current->value != key){
            parent = current;
            //go left, smaller than current
            if (key < current->value){
                current = current->left;
            }

                //go right, greater than current
            else if (key > current->value){
                current = current->right;
            }
        }

        //b)remove node
        //Case 1: node is a leaf
        if (is_leaf(current)){
            if (key < parent->value){
                parent->left = nullptr;
                delete current;
            }
            else{
                parent->right = nullptr;
                delete current;
            }
            rebalance(parent);
        }

            //Case 2: node has one child
            //2.a) left chld
        else if(current->right == nullptr){
            if (key < parent->value){ //to the left of parent
                parent->left = current->left;
            }
            else{
                parent->right = current->left;
            }
            delete current;
        }
            //2.b) right child
        else if(current->left == nullptr){
            if (key < parent->value){ //to the left of parent
                parent->left = current->right;
            }
            else{
                parent->right = current->right;
            }
            delete current;
            rebalance (parent);
        }

            //Case 3: node has 2 children
        else{
            Node* new_node = sym_suc(current);

            new_node->left = current->left;

            current->left = current->right = nullptr;

            if (new_node->value < parent->value){
                parent->left = new_node;
            }
            else{
                parent->right = new_node;
            }
            delete current;
            rebalance(new_node);
        }

    }

    //preorder tree walk
    void preorder_walk(Node* start){
        if (start != nullptr){
            assert(check_bal(start));
            std::cout << start->value << " ";
            preorder_walk(start->left);
            preorder_walk(start->right);
        }
    }

    //helper function to output balance, inactive
    void preorder_balance(Node* start){
        if (start != nullptr){
            std::cerr << start->balance << " ";
            preorder_balance(start->left);
            preorder_balance(start->right);
        }
    }

    // output the content of the tree in pre-order: node then node->left then node->right
    void print(){
        preorder_walk(root);
        //preorder_balance(root);
    }

private:
    Node* root = nullptr;
};


/*
 From lecturer: Felix Friedrich @ ETH
 */

// please do not modify this function
bool hasInt(std::istream& is){
    return is.good() && is >> std::ws && is.good() && !std::isalpha(is.peek());
}

// main function -- driver of the tests, please do not modify
int main() {
    AvlTree<int> tree;
    for(std::string command; std::cin >> command && command != "end";) {
        if(command == "insert") {
            for(int num; hasInt(std::cin) && std::cin >> num;){
                tree.insert(num);
            }
        }
        else if (command == "remove") { // removal is an optional part of the exercise -- it is not tested
            for(int num; hasInt(std::cin) && std::cin >> num;){
                tree.remove(num);
            }
        }
        else if(command == "print") {
            std::cout << "[ ";
            tree.print(); // output of the tree in pre-order: node then left then right
            std::cout << " ]" << std::endl;
        }
        else {
            std::cerr << "command?" << std::endl;
        }
    }
    return 0;
}
