#include "HCTree.hpp"
#include <stack>

using namespace std;


/** function prototype(s) */
void postorder_delete(HCNode*);
void print_tree();
void print_helper(HCNode*, int);




/**
 * Print Huffman Tree
 */
void HCTree::print_tree() {
    print_helper(root, 0);
    cout << "Done printing tree." << endl;

}

/**
 * Rescursive helper for print_tree
 * Print each node as:  (symbol, freq)
 * Tree printed sideways: root on left, right subtree on top, left subtree on bottom
 */
void print_helper(HCNode* curr, int num_spaces) {

    if (curr == nullptr) { return; }

    num_spaces += 10;

    // right subtree on top
    print_helper(curr->c1, num_spaces);

    cout << endl;
    for (int i = 10; i < num_spaces; i++) {
        cout << " ";
    }
    cout << "(" << curr->symbol << "," << curr->count << ")" << endl;

    // left subtree on bottom
    print_helper(curr->c0, num_spaces);
}






/**
HCTree Destructor
*/
HCTree::~HCTree() {
    postorder_delete(root);
    root = nullptr;
}




/**
Recursive post-order binary tree delete
*/
void postorder_delete(HCNode* n) {

    if (n == nullptr) { return; }

    postorder_delete(n->c0); //left
    postorder_delete(n->c1); //right

    delete n;
    n = nullptr;
}







/**
* Use the Huffman algorithm to build a Huffman coding tree.
* PRECONDITION: freqs is a vector of ints, such that freqs[i] is the frequency of occurrence of byte i in the input file.
* POSTCONDITION: root points to the root of the trie, and leaves[i] points to the leaf node containing byte i.
* 
* Min-Heap w/ priority_queue: https://www.geeksforgeeks.org/priority-queue-in-cpp-stl/
* Cast from int to unsigned char: https://www.reddit.com/r/cpp_questions/comments/fhxt2c/help_with_casting_from_int_to_unsigned_char_in_c/
*
* If int > 256 may lose some data when casting, but size of freqs should be 256 (each index corresponds to symbol) so its okay
* https://cboard.cprogramming.com/c-programming/64068-convert-integer-unsigned-char.html
*/
void HCTree::build(const vector<int>& freqs) {


    /*
        Can edit this so that freqs size isnt 256 and rather just as long as it needs to be
        i.e. no non-zero elements. Then, need top

        1. edit directly below in this function
        2. edit how freq_list is made in compress.cpp
        3. edit how file header is stored

    */


    // store roots of all trees in forest
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> forest;

    // create leaf Nodes for all symbols with non-zero frequencies and add to forest
    for (int i = 0; i < freqs.size(); i++) {
        if (freqs[i] > 0) {
            HCNode* newNode = new HCNode(freqs[i], (unsigned char)i);
            forest.push(newNode);
            leaves[i] = newNode; // POSTCONDITION: leaves[i] points to the leaf node containing byte i
        }
    }


    // build Huffman Tree
    while (forest.size() > 1) {
        HCNode* t1 = forest.top(); // tree 1 root
        forest.pop();
        HCNode* t2 = forest.top(); // tree 2 root
        forest.pop();


        /*
         * LOWER frequency node will be LEFT (c0) and have edge labeled by 0
         * HIGHER frequency node will be RIGHT (c1) and have edge labeled by 1
         * PARENT will store symbol of c0 node (left child) 
         */
         HCNode* c0_node;
         HCNode* c1_node;
         if (t1->count < t2->count) {
             c0_node = t1;
             c1_node = t2;
         } else {
             c0_node = t2;
             c1_node = t1;
         }


        // store symbol of LEFT child (0 path) in the internal node (to deal with tie breakers later)
        HCNode* parent = new HCNode(t1->count + t2->count, c0_node->symbol);

        parent->c0 = c0_node;
        parent->c1 = c1_node;
        c0_node->p = parent;
        c1_node->p = parent;  

        // add parent to forest
        forest.push(parent);
    }


    // point root to root of Huffman Tree (binary trie)
    root = forest.top(); 

    return;   
}




/**
* Write to the given FancyOutputStream the sequence of bits coding the given symbol (encode SINGLE symbol).
* PRECONDITION: build() has been called, to create the coding tree, and initialize root pointer and leaves vector.
*/
void HCTree::encode(unsigned char symbol, FancyOutputStream & out) const {

    int idx = symbol; //cast char to ASCII value

    stack<int> reverse_encoding;

    HCNode* curr = leaves[idx]; //ptr to corresponding node to symbol in tree

    //travel upwards towards root and push path label (0 or 1) to stack 
    while (curr->p != nullptr) {
        if (curr->p->c0 == curr) { // if curr connected to parent by 0-labeled edge
            reverse_encoding.push(0);
        } else {
            reverse_encoding.push(1);
        }
        curr = curr->p; //move curr ptr up the tree
    }


    while (!reverse_encoding.empty()) {
        //pop stack elements and write to FancyOutputStream
        out.write_bit(reverse_encoding.top());
        reverse_encoding.pop();
    }

    return;
}



/**
* Return symbol coded in the next sequence of bits from the stream.
* PRECONDITION: build() has been called, to create the coding tree, and initialize root pointer and leaves vector.
*/
unsigned char HCTree::decode(FancyInputStream & in) const {

    /*  [DO LATER]note: (serialize tree approach)
        keep going down indices of the array until a -1 is reached (-1 when no child)
    */



    // start at root and keep going down tree until a leaf is reached
    // while !(leaf)
    // read bit
    // move curr ptr left or right depending on whihc bit matches

    HCNode* curr = root;

    // while (curr != nullptr) { // change this to be c0 and c1
    while ((curr->c0 != nullptr) && (curr->c1 != nullptr)) { // change this to be c0 and c1
        int curr_bit = in.read_bit();
        if (curr_bit == 0) {
            curr = curr->c0;
        } else {
            curr = curr-> c1;
        }
    }

    return curr->symbol;

}






