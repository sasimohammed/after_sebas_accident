// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable {
private:
    static const int TABLE_SIZE = 101;
    struct Entry {
        int playerID;
        string name;
        bool occupied;

        Entry() : playerID(-1), name(""), occupied(false) {}
    };
    vector<Entry> table;
    int hash1(int key) {
        return key % TABLE_SIZE;
    }
    int hash2(int key) {
        return 1 + (key % (TABLE_SIZE - 1));
    }
public:
    ConcretePlayerTable() {
        table = vector<Entry>(TABLE_SIZE);
    }
    void insert(int playerID, string name) override {
        int h1 = hash1(playerID);
        int h2 = hash2(playerID);
        for (int i = 0; i < TABLE_SIZE; i++) {
            int index = (h1 + i * h2) % TABLE_SIZE;

            if (!table[index].occupied) {
                table[index].playerID = playerID;
                table[index].name = name;
                table[index].occupied = true;
                return;
            }
        }
        cout << "Table is Full";
    }
    string search(int playerID) override {
        int h1 = hash1(playerID);
        int h2 = hash2(playerID);
        for (int i = 0; i < TABLE_SIZE; i++) {
            int index = (h1 + i * h2) % TABLE_SIZE;
            if (!table[index].occupied) {
                return "";
            }
            if (table[index].playerID == playerID) {
                return table[index].name;
            }
        }
        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---
class ConcreteLeaderboard : public Leaderboard {
private:
    struct Node {
        int playerID;
        int score;
        vector<Node*> forward;
        Node(int id, int sc, int level) : playerID(id), score(sc), forward(level, nullptr) {}
    };
    Node* head;
    int maxLevel;
    float p;       
    int randomLevel() {
        int lvl = 1;
        while (((float) rand() / RAND_MAX) < p && lvl < maxLevel)
            lvl++;
        return lvl;
    }
    bool comesBefore(Node* a, Node* b) {
        if (a->score != b->score) return a->score > b->score; 
        return a->playerID < b->playerID;                  
    }
public:
    ConcreteLeaderboard(int maxLvl = 16, float prob = 0.5) {
        srand(time(0));
        maxLevel = maxLvl;
        p = prob;
        head = new Node(-1, -1, maxLevel);
    }
    void addScore(int playerID, int score) override {
        vector<Node*> update(maxLevel, nullptr);
        Node* current = head;
        for (int i = maxLevel - 1; i >= 0; i--) {
            while (current->forward[i] && comesBefore(current->forward[i], new Node(playerID, score, 0))) {
                current = current->forward[i];
            }
            update[i] = current;
        }
        int lvl = randomLevel();
        Node* newNode = new Node(playerID, score, lvl);
        for (int i = 0; i < lvl; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }
    void removePlayer(int playerID) override {
        vector<Node*> update(maxLevel, nullptr);
        Node* current = head;
        for (int i = maxLevel - 1; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->playerID != playerID) {
                current = current->forward[i];
            }
            update[i] = current;
        }
        Node* target = nullptr;
        for (int i = 0; i < maxLevel; i++) {
            if (update[i]->forward[i] && update[i]->forward[i]->playerID == playerID) {
                target = update[i]->forward[i];
                update[i]->forward[i] = target->forward[i];
            }
        }
        if (target) delete target;
    }
    vector<int> getTopN(int n) override {
        vector<int> result;
        Node* current = head->forward[0]; 
        while (current && result.size() < n) {
            result.push_back(current->playerID);
            current = current->forward[0];
        }
        return result;
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

enum Color { RED, BLACK };
struct RBNode {
    int itemID;
    int price;
    Color color;
    RBNode *left, *right, *parent;

    RBNode(int id, int p) : itemID(id), price(p), color(RED),
        left(nullptr), right(nullptr), parent(nullptr) {}
};

class ConcreteAuctionTree : public AuctionTree {
private:
    RBNode* root;
    void leftRotate(RBNode* x) {
        RBNode* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rightRotate(RBNode* y) {
        RBNode* x = y->left;
        y->left = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;
        if (!y->parent) root = x;
        else if (y == y->parent->left) y->parent->left = x;
        else y->parent->right = x;
        x->right = y;
        y->parent = x;
    }

    void insertFixup(RBNode* z) {
        while (z->parent && z->parent->color == RED) {
            RBNode* gp = z->parent->parent;
            if (z->parent == gp->left) {
                RBNode* y = gp->right;
                if (y && y->color == RED) { 
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    gp->color = RED;
                    z = gp;
                } else {
                    if (z == z->parent->right) { 
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = BLACK;
                    gp->color = RED;
                    rightRotate(gp);
                }
            } else {
                RBNode* y = gp->left;
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    gp->color = RED;
                    z = gp;
                } else {
                    if (z == z->parent->left) { 
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    gp->color = RED;
                    leftRotate(gp);
                }
            }
        }
        root->color = BLACK;
    }
    void transplant(RBNode* u, RBNode* v) {
        if (!u->parent) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        if (v) v->parent = u->parent;
    }

    RBNode* minimum(RBNode* node) {
        while (node->left) node = node->left;
        return node;
    }
    void deleteFixup(RBNode* x, RBNode* xParent) {
        while (x != root && (!x || x->color == BLACK)) {
            if (x == xParent->left) {
                RBNode* w = xParent->right;
                if (w && w->color == RED) {
                    w->color = BLACK;
                    xParent->color = RED;
                    leftRotate(xParent);
                    w = xParent->right;
                }
                if ((!w->left || w->left->color == BLACK) &&
                    (!w->right || w->right->color == BLACK)) {
                    if(w) w->color = RED;
                    x = xParent;
                    xParent = x->parent;
                } else {
                    if (!w->right || w->right->color == BLACK) {
                        if(w->left) w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = xParent->right;
                    }
                    if(w) w->color = xParent->color;
                    xParent->color = BLACK;
                    if(w && w->right) w->right->color = BLACK;
                    leftRotate(xParent);
                    x = root;
                    break;
                }
            } else {
                RBNode* w = xParent->left;
                if (w && w->color == RED) {
                    w->color = BLACK;
                    xParent->color = RED;
                    rightRotate(xParent);
                    w = xParent->left;
                }
                if ((!w->left || w->left->color == BLACK) &&
                    (!w->right || w->right->color == BLACK)) {
                    if(w) w->color = RED;
                    x = xParent;
                    xParent = x->parent;
                } else {
                    if (!w->left || w->left->color == BLACK) {
                        if(w->right) w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = xParent->left;
                    }
                    if(w) w->color = xParent->color;
                    xParent->color = BLACK;
                    if(w && w->left) w->left->color = BLACK;
                    rightRotate(xParent);
                    x = root;
                    break;
                }
            }
        }
        if(x) x->color = BLACK;
    }

    RBNode* searchByID(RBNode* node, int itemID) {
        if (!node) return nullptr;
        if (node->itemID == itemID) return node;
        RBNode* leftResult = searchByID(node->left, itemID);
        if (leftResult) return leftResult;
        return searchByID(node->right, itemID);
    }
    bool lessThan(RBNode* a, RBNode* b) {
        if (a->price != b->price) return a->price < b->price;
        return a->itemID < b->itemID;
    }

public:
    ConcreteAuctionTree() {
        root = nullptr;
    }

    void insertItem(int itemID, int price) override {
        RBNode* z = new RBNode(itemID, price);
        RBNode* y = nullptr;
        RBNode* x = root;

        while (x) {
            y = x;
            if (lessThan(z, x)) x = x->left;
            else x = x->right;
        }
        z->parent = y;
        if (!y) root = z;
        else if (lessThan(z, y)) y->left = z;
        else y->right = z;

        insertFixup(z);
    }
    void deleteItem(int itemID) override {
        RBNode* z = searchByID(root, itemID);
        if (!z) return;

        RBNode* y = z;
        RBNode* x = nullptr;
        RBNode* xParent = nullptr;
        Color yOriginalColor = y->color;

        if (!z->left) {
            x = z->right;
            xParent = z->parent;
            transplant(z, z->right);
        } else if (!z->right) {
            x = z->left;
            xParent = z->parent;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == z) {
                if(x) x->parent = y;
                xParent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
                xParent = y->parent;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        if (yOriginalColor == BLACK)
            deleteFixup(x, xParent);
    }
};


