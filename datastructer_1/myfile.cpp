/*
IDs & Names:
20231079 - sohila walid mohammed
20231087 - seba ibraheem ezzat
20230289 - katren nader nagy
00000000 -
00000000 -
*/

#include <iostream>
using namespace std;

// --------------------------
// BTree template
// --------------------------
template <class T, int Order>
class BTree {
private:
    // --------------------------
    // Node structure
    // --------------------------
    struct Node {
        int keysCount;           // number of keys currently in node
        T* keys;                 // array of keys
        Node** child;            // array of child pointers
        bool leaf;               // true if node is leaf

        // Constructor for node
        Node() {
            keysCount = 0;       // initialize keys count to 0
            leaf = true;         // default node is a leaf
            keys = new T[Order]; // allocate array for keys
            child = new Node*[Order + 1]; // allocate array for child pointers (max children = Order)
            for (int i = 0; i <= Order; i++)
                child[i] = nullptr;       // initialize all child pointers to nullptr
        }

        // Destructor to free dynamic memory
        ~Node() {
            delete[] keys;       // delete keys array
            delete[] child;      // delete children array
        }
    };

    Node* root;                 // root of the BTree

    // --------------------------
    // Split full child node
    // --------------------------
    void SplitChild(Node* parent, int index) {
        Node* fullChild = parent->child[index];  // the child node that is full
        Node* newNode = new Node();              // create new node to store half keys
        newNode->leaf = fullChild->leaf;         // new node has same leaf status

        int mid = fullChild->keysCount / 2;      // middle key index

        int moveCount = fullChild->keysCount - mid - 1;  // number of keys to move to new node
        for (int i = 0; i < moveCount; i++)
            newNode->keys[i] = fullChild->keys[i + mid + 1]; // copy keys to new node

        // Move children if fullChild is not leaf
        if (!fullChild->leaf) {
            for (int i = 0; i <= moveCount; i++)
                newNode->child[i] = fullChild->child[i + mid + 1]; // copy children
        }

        newNode->keysCount = moveCount;         // set key count of new node
        fullChild->keysCount = mid;             // reduce key count of original node

        // Shift parent's children to make space for new child
        for (int i = parent->keysCount; i >= index + 1; i--)
            parent->child[i + 1] = parent->child[i];
        parent->child[index + 1] = newNode;    // insert new child in parent

        // Shift parent's keys and insert middle key
        for (int i = parent->keysCount - 1; i >= index; i--)
            parent->keys[i + 1] = parent->keys[i];
        parent->keys[index] = fullChild->keys[mid]; // middle key moves up to parent
        parent->keysCount++;                          // increase parent's key count
    }

    // --------------------------
    // Insert value into node
    // --------------------------
    void InsertNode(Node* node, T value) {
        if (node->leaf) {
            // Insert value in sorted order in leaf
            int i = node->keysCount - 1;
            while (i >= 0 && value < node->keys[i]) {
                node->keys[i + 1] = node->keys[i]; // shift keys to make space
                i--;
            }
            node->keys[i + 1] = value;            // insert value in correct position
            node->keysCount++;                     // increment keys count
        } else {
            // Find child to insert value
            int i = 0;
            while (i < node->keysCount && value > node->keys[i])
                i++; // find correct child index

            // Recursively insert in child
            InsertNode(node->child[i], value);

            // Split child if full after insertion
            if (node->child[i]->keysCount == Order)
                SplitChild(node, i);
        }
    }

    // --------------------------
    // Print tree recursively
    // --------------------------
    void PrintNode(Node* node, int depth) {
        if (!node) return;                  // safety check

        // Print keys of this node with proper indentation
        for (int i = 0; i < depth; i++)
            cout << "  ";                   // 2 spaces per depth
        for (int i = 0; i < node->keysCount; i++) {
            cout << node->keys[i];          // print key
            if (i + 1 < node->keysCount)
                cout << ",";                // comma between keys
        }
        cout << endl;

        // Recursively print children
        if (!node->leaf) {
            for (int i = 0; i <= node->keysCount; i++)
                PrintNode(node->child[i], depth + 1); // print children with increased depth
        }
    }

public:
    // --------------------------
    // Constructor
    // --------------------------
    BTree() { root = nullptr; }           // initialize root to nullptr

    // --------------------------
    // Public Insert function
    // --------------------------
    void Insert(T value) {
        if (!root) {
            root = new Node();             // create root if tree is empty
            root->keys[0] = value;         // insert first value
            root->keysCount = 1;
            return;
        }

        InsertNode(root, value);           // insert recursively

        // Split root if full after insertion
        if (root->keysCount == Order) {
            Node* newRoot = new Node();    // create new root
            newRoot->leaf = false;
            newRoot->child[0] = root;      // old root becomes child
            SplitChild(newRoot, 0);        // split old root
            root = newRoot;                // update root pointer
        }
    }

    // --------------------------
    // Public Print function
    // --------------------------
    void Print() { PrintNode(root, 0); }   // print starting from root, depth=0

    // --------------------------
    // Destructor
    // --------------------------
    ~BTree() { delete root; }             // free all allocated memory
};

// --------------------------
// Main function (do not modify)
// --------------------------
int main() {
    // Create a BTree of order 3 with int data
    BTree<int, 3> t1;

    // Insert values as per assignment
    t1.Insert(1);
    t1.Insert(5);
    t1.Insert(0);
    t1.Insert(4);
    t1.Insert(3);
    t1.Insert(2);

    t1.Print(); // Expected output matches assignment

//    expected output

    /*
      1,4
        0
        2,3
        5
      */


    // Create a BTree of order 5 with char data
    BTree<char, 5> t;

    // Insert values as per assignment
    t.Insert('G'); t.Insert('I'); t.Insert('B'); t.Insert('J'); t.Insert('C');
    t.Insert('A'); t.Insert('K'); t.Insert('E'); t.Insert('D'); t.Insert('S');
    t.Insert('T'); t.Insert('R'); t.Insert('L'); t.Insert('F'); t.Insert('H');
    t.Insert('M'); t.Insert('N'); t.Insert('P'); t.Insert('Q');

    t.Print(); // Expected output matches assignment

    //    expected output
/*
    K
      C,G
        A,B
        D,E,F
        H,I,J
      N,R
        L,M
        P,Q
        S,T
    */


    return 0;
}
