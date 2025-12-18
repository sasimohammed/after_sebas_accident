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
#include <functional>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

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
    int size;

    int hash1(int key) {
        return key % TABLE_SIZE;
    }

    int hash2(int key) {
        return 1 + (key % (TABLE_SIZE - 1));
    }

public:
    ConcretePlayerTable() {
        table = vector<Entry>(TABLE_SIZE);
        size = 0;
    }

    void insert(int playerID, string name) override {
        if (size >= TABLE_SIZE) {
            cout << "Table is full" << endl;
            return;
        }

        int h1 = hash1(playerID);
        int h2 = hash2(playerID);

        for (int i = 0; i < TABLE_SIZE; i++) {
            int index = (h1 + i * h2) % TABLE_SIZE;

            if (!table[index].occupied) {
                table[index].playerID = playerID;
                table[index].name = name;
                table[index].occupied = true;
                size++;
                return;
            }

            if (table[index].playerID == playerID) {
                table[index].name = name;
                return;
            }
        }

        cout << "Table is full" << endl;
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
    float probability;

    int randomLevel() {
        int level = 1;
        while (((float)rand() / RAND_MAX) < probability && level < maxLevel) {
            level++;
        }
        return level;
    }

    bool isLessThan(Node* a, Node* b) {
        if (a->score != b->score) {
            return a->score < b->score;
        }
        return a->playerID > b->playerID;
    }

public:
    ConcreteLeaderboard() {
        maxLevel = 16;
        probability = 0.5;
        head = new Node(-1, INT_MIN, maxLevel);
    }

    ~ConcreteLeaderboard() {
        Node* current = head->forward[0];
        while (current) {
            Node* next = current->forward[0];
            delete current;
            current = next;
        }
        delete head;
    }

    void addScore(int playerID, int score) override {
        vector<Node*> update(maxLevel, nullptr);
        Node* current = head;

        for (int i = maxLevel - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr) {
                Node* next = current->forward[i];
                if (next->score > score || (next->score == score && next->playerID < playerID)) {
                    current = next;
                } else {
                    break;
                }
            }
            update[i] = current;
        }

        Node* next = current->forward[0];
        if (next != nullptr && next->playerID == playerID) {
            removePlayer(playerID);
            addScore(playerID, score);
            return;
        }

        int newLevel = randomLevel();
        Node* newNode = new Node(playerID, score, newLevel);

        for (int i = 0; i < newLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }

    void removePlayer(int playerID) override {
        vector<Node*> update(maxLevel, nullptr);
        Node* current = head;

        for (int i = maxLevel - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->playerID != playerID) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        Node* toDelete = current->forward[0];

        if (toDelete != nullptr && toDelete->playerID == playerID) {
            for (int i = 0; i < maxLevel; i++) {
                if (update[i]->forward[i] != toDelete) {
                    break;
                }
                update[i]->forward[i] = toDelete->forward[i];
            }
            delete toDelete;
        }
    }

    vector<int> getTopN(int n) override {
        vector<int> result;
        Node* current = head->forward[0];

        while (current != nullptr && result.size() < n) {
            result.push_back(current->playerID);
            current = current->forward[0];
        }

        return result;
    }
};

enum Color { RED, BLACK };

struct RBNode {
    int itemID;
    int price;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;

    RBNode(int id, int p) : itemID(id), price(p), color(RED),
                            left(nullptr), right(nullptr), parent(nullptr) {}
};

class ConcreteAuctionTree : public AuctionTree {
private:
    RBNode* root;

    void leftRotate(RBNode* x) {
        RBNode* y = x->right;
        x->right = y->left;

        if (y->left != nullptr) {
            y->left->parent = x;
        }

        y->parent = x->parent;

        if (x->parent == nullptr) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }

        y->left = x;
        x->parent = y;
    }

    void rightRotate(RBNode* y) {
        RBNode* x = y->left;
        y->left = x->right;

        if (x->right != nullptr) {
            x->right->parent = y;
        }

        x->parent = y->parent;

        if (y->parent == nullptr) {
            root = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }

        x->right = y;
        y->parent = x;
    }

    void insertFixup(RBNode* z) {
        while (z->parent != nullptr && z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                RBNode* y = z->parent->parent->right;

                if (y != nullptr && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                RBNode* y = z->parent->parent->left;

                if (y != nullptr && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }

        root->color = BLACK;
    }

    RBNode* findNodeByID(int itemID) {
        return findNodeByIDHelper(root, itemID);
    }

    RBNode* findNodeByIDHelper(RBNode* node, int itemID) {
        if (node == nullptr) return nullptr;

        if (node->itemID == itemID) return node;

        RBNode* leftResult = findNodeByIDHelper(node->left, itemID);
        if (leftResult != nullptr) return leftResult;

        return findNodeByIDHelper(node->right, itemID);
    }

    RBNode* minimum(RBNode* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    void transplant(RBNode* u, RBNode* v) {
        if (u->parent == nullptr) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }

        if (v != nullptr) {
            v->parent = u->parent;
        }
    }

    void deleteFixup(RBNode* x, RBNode* xParent) {
        while (x != root && (x == nullptr || x->color == BLACK)) {
            if (x == xParent->left) {
                RBNode* w = xParent->right;

                if (w->color == RED) {
                    w->color = BLACK;
                    xParent->color = RED;
                    leftRotate(xParent);
                    w = xParent->right;
                }

                if ((w->left == nullptr || w->left->color == BLACK) &&
                    (w->right == nullptr || w->right->color == BLACK)) {
                    w->color = RED;
                    x = xParent;
                    xParent = x->parent;
                } else {
                    if (w->right == nullptr || w->right->color == BLACK) {
                        if (w->left != nullptr) {
                            w->left->color = BLACK;
                        }
                        w->color = RED;
                        rightRotate(w);
                        w = xParent->right;
                    }

                    w->color = xParent->color;
                    xParent->color = BLACK;
                    if (w->right != nullptr) {
                        w->right->color = BLACK;
                    }
                    leftRotate(xParent);
                    x = root;
                }
            } else {
                RBNode* w = xParent->left;

                if (w->color == RED) {
                    w->color = BLACK;
                    xParent->color = RED;
                    rightRotate(xParent);
                    w = xParent->left;
                }

                if ((w->right == nullptr || w->right->color == BLACK) &&
                    (w->left == nullptr || w->left->color == BLACK)) {
                    w->color = RED;
                    x = xParent;
                    xParent = x->parent;
                } else {
                    if (w->left == nullptr || w->left->color == BLACK) {
                        if (w->right != nullptr) {
                            w->right->color = BLACK;
                        }
                        w->color = RED;
                        leftRotate(w);
                        w = xParent->left;
                    }

                    w->color = xParent->color;
                    xParent->color = BLACK;
                    if (w->left != nullptr) {
                        w->left->color = BLACK;
                    }
                    rightRotate(xParent);
                    x = root;
                }
            }
        }

        if (x != nullptr) {
            x->color = BLACK;
        }
    }

    bool isLessThan(RBNode* a, RBNode* b) {
        if (a->price != b->price) {
            return a->price < b->price;
        }
        return a->itemID < b->itemID;
    }

    void deleteTree(RBNode* node) {
        if (node == nullptr) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

public:
    ConcreteAuctionTree() {
        root = nullptr;
    }

    ~ConcreteAuctionTree() {
        deleteTree(root);
    }

    void insertItem(int itemID, int price) override {
        RBNode* newNode = new RBNode(itemID, price);

        RBNode* y = nullptr;
        RBNode* x = root;

        while (x != nullptr) {
            y = x;
            if (isLessThan(newNode, x)) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        newNode->parent = y;

        if (y == nullptr) {
            root = newNode;
        } else if (isLessThan(newNode, y)) {
            y->left = newNode;
        } else {
            y->right = newNode;
        }

        insertFixup(newNode);
    }

    void deleteItem(int itemID) override {
        RBNode* z = findNodeByID(itemID);

        if (z == nullptr) {
            return;
        }

        RBNode* y = z;
        RBNode* x = nullptr;
        RBNode* xParent = nullptr;
        Color yOriginalColor = y->color;

        if (z->left == nullptr) {
            x = z->right;
            xParent = z->parent;
            transplant(z, z->right);
        } else if (z->right == nullptr) {
            x = z->left;
            xParent = z->parent;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;

            if (y->parent == z) {
                if (x != nullptr) {
                    x->parent = y;
                }
                xParent = y;
            } else {
                xParent = y->parent;
                transplant(y, y->right);
                y->right = z->right;
                if (y->right != nullptr) {
                    y->right->parent = y;
                }
            }

            transplant(z, y);
            y->left = z->left;
            if (y->left != nullptr) {
                y->left->parent = y;
            }
            y->color = z->color;
        }

        delete z;

        if (yOriginalColor == BLACK) {
            deleteFixup(x, xParent);
        }
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    int totalSum = 0;
    for(int i = 0; i < n; i++)
        totalSum += coins[i];

    int halfTotal = totalSum / 2;

    vector<bool> isAchievable(halfTotal + 1, false);
    isAchievable[0] = true;

    for(int i = 0; i < n; i++){
        for(int j = halfTotal; j >= coins[i]; j--){
            if(isAchievable[j - coins[i]])
                isAchievable[j] = true;
        }
    }

    int best = 0;
    for(int j = halfTotal; j >= 0; j--){
        if(isAchievable[j]){
            best = j;
            break;
        }
    }

    return totalSum - 2 * best;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    int n = items.size();
    vector<vector<int>> dp(n + 1, vector<int>(capacity + 1, 0));

    for (int i = 1; i <= n; i++) {
        int weight = items[i-1].first;
        int value = items[i-1].second;

        for (int w = 0; w <= capacity; w++) {
            if (weight > w) {
                dp[i][w] = dp[i-1][w];
            } else {
                dp[i][w] = max(dp[i-1][w], value + dp[i-1][w - weight]);
            }
        }
    }

    return dp[n][capacity];
}

long long InventorySystem::countStringPossibilities(string s) {
    int n = s.size();
    if (n == 0) return 1;

    const long long MOD = 1e9 + 7;
    vector<long long> dp(n + 1, 0);
    dp[0] = 1;
    dp[1] = 1;

    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i-1];

        if ((s[i-1] == 'u' && s[i-2] == 'u') || (s[i-1] == 'n' && s[i-2] == 'n')) {
            dp[i] = (dp[i] + dp[i-2]) % MOD;
        }
    }

    return dp[n];
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    if(source == dest) return true;

    vector<vector<int>> adj(n);
    for(auto& e : edges){
        adj[e[0]].push_back(e[1]);
        adj[e[1]].push_back(e[0]);
    }

    vector<bool> visited(n, false);
    queue<int> q;
    q.push(source);
    visited[source] = true;

    while(!q.empty()){
        int node = q.front();
        q.pop();

        if(node == dest) return true;

        for(int neighbor : adj[node]){
            if(!visited[neighbor]){
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    vector<vector<pair<int, long long>>> adj(n);
    for (auto& r : roadData) {
        long long cost = r[2] * goldRate + r[3] * silverRate;
        adj[r[0]].push_back({r[1], cost});
        adj[r[1]].push_back({r[0], cost});
    }

    vector<bool> visited(n, false);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
            greater<pair<long long, int>>> pq;
    pq.push({0, 0});

    long long totalCost = 0;
    int visitedCount = 0;

    while (!pq.empty() && visitedCount < n) {
        auto [cost, node] = pq.top();
        pq.pop();

        if (visited[node]) continue;

        visited[node] = true;
        totalCost += cost;
        visitedCount++;

        for (auto& [neighbor, edgeCost] : adj[node]) {
            if (!visited[neighbor]) {
                pq.push({edgeCost, neighbor});
            }
        }
    }

    return (visitedCount == n) ? totalCost : -1;
}

string WorldNavigator::sumMinDistancesBinary(
        int n,
        vector<vector<int>>& roads
) {
    const long long INF = 1e18;

    vector<vector<long long>> dist(n, vector<long long>(n, INF));
    for (int i = 0; i < n; i++)
        dist[i][i] = 0;

    for (auto &r : roads) {
        int u = r[0], v = r[1], w = r[2];
        dist[u][v] = min(dist[u][v], (long long)w);
        dist[v][u] = min(dist[v][u], (long long)w);
    }

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (dist[i][k] < INF && dist[k][j] < INF) {
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }

    __int128 total = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (dist[i][j] < INF) {
                total += dist[i][j];
            }
        }
    }

    if (total == 0) return "0";

    string binary = "";
    while (total > 0) {
        binary.push_back((total & 1) + '0');
        total >>= 1;
    }

    reverse(binary.begin(), binary.end());
    return binary;
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    vector<int> freq(26, 0);
    for (char t : tasks) {
        freq[t - 'A']++;
    }

    int maxFreq = *max_element(freq.begin(), freq.end());

    int countMax = 0;
    for (int f : freq) {
        if (f == maxFreq)
            countMax++;
    }

    int intervals = (maxFreq - 1) * (n + 1) + countMax;

    return max((int)tasks.size(), intervals);
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
PlayerTable* createPlayerTable() {
    return new ConcretePlayerTable();
}

Leaderboard* createLeaderboard() {
    return new ConcreteLeaderboard();
}

AuctionTree* createAuctionTree() {
    return new ConcreteAuctionTree();
}
}