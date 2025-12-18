#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>

// Add these declarations before including ArcadiaEngine.h
extern "C" {
class PlayerTable;
class Leaderboard;
class AuctionTree;

PlayerTable* createPlayerTable();
Leaderboard* createLeaderboard();
AuctionTree* createAuctionTree();
}

#include "ArcadiaEngine.h"

using namespace std;

// ==========================================
// TEST UTILITIES
// ==========================================
class StudentTestRunner {
    int count = 0;
    int passed = 0;
    int failed = 0;

public:
    void runTest(const string& testName, bool condition) {  // Fixed: const reference
        count++;
        cout << "TEST: " << left << setw(50) << testName;
        if (condition) {
            cout << "[ PASS ]";
            passed++;
        } else {
            cout << "[ FAIL ]";
            failed++;
        }
        cout << endl;
    }

    void printSummary() const {  // Fixed: made const
        cout << "\n==========================================" << endl;
        cout << "SUMMARY: Passed: " << passed << " | Failed: " << failed << endl;
        cout << "==========================================" << endl;
        cout << "TOTAL TESTS: " << count << endl;
    }
};

StudentTestRunner runner;

// ==========================================
// PART A: DATA STRUCTURES TESTS
// ==========================================
void test_PartA_PlayerTable() {
    cout << "\n--- Part A.1: PlayerTable (Double Hashing) ---" << endl;

    // Test 1: Basic Insert and Search
    runner.runTest("PlayerTable: Insert & Search (ID 123)", [&]() {
        PlayerTable* table = createPlayerTable();
        table->insert(123, "Player1");
        string result = table->search(123);
        delete table;
        return result == "Player1";
    }());

    // Test 2: Search Non-existent Player
    runner.runTest("PlayerTable: Search Non-existent (ID 999)", [&]() {
        PlayerTable* table = createPlayerTable();
        string result = table->search(999);
        delete table;
        return result == "";
    }());

    // Test 3: Update Existing Player
    runner.runTest("PlayerTable: Update Existing Player", [&]() {
        PlayerTable* table = createPlayerTable();
        table->insert(100, "OldName");
        table->insert(100, "NewName");
        string result = table->search(100);
        delete table;
        return result == "NewName";
    }());

    // Test 4: Multiple Players with Collisions
    runner.runTest("PlayerTable: Multiple Inserts with Collisions", [&]() {
        PlayerTable* table = createPlayerTable();
        table->insert(100, "Alice");
        table->insert(201, "Bob");  // Should cause collision with double hashing
        table->insert(302, "Charlie");

        bool allFound = true;
        allFound &= (table->search(100) == "Alice");
        allFound &= (table->search(201) == "Bob");
        allFound &= (table->search(302) == "Charlie");

        delete table;
        return allFound;
    }());

    // Test 5: Fill Table and Test Full Condition
    runner.runTest("PlayerTable: Insert Many Players (No Crash)", [&]() {
        PlayerTable* table = createPlayerTable();
        bool success = true;

        // Insert 100 players (not filling completely to avoid "full" message)
        for (int i = 1; i <= 100; i++) {
            table->insert(i, "Player" + to_string(i));
        }

        // Verify some random entries
        success &= (table->search(1) == "Player1");
        success &= (table->search(50) == "Player50");
        success &= (table->search(100) == "Player100");

        delete table;
        return success;
    }());
}

void test_PartA_Leaderboard() {
    cout << "\n--- Part A.2: Leaderboard (Skip List) ---" << endl;

    // Test 1: Basic Add and GetTopN
    runner.runTest("Leaderboard: AddScore & GetTopN (3 players)", [&]() {
        Leaderboard* lb = createLeaderboard();
        lb->addScore(1, 100);
        lb->addScore(2, 200);
        lb->addScore(3, 150);

        vector<int> top = lb->getTopN(2);
        bool correct = (top.size() == 2) && (top[0] == 2) && (top[1] == 3);

        delete lb;
        return correct;
    }());

    // Test 2: Tie-breaking (same score, different IDs)
    runner.runTest("Leaderboard: Tie-breaking (ascending ID)", [&]() {
        Leaderboard* lb = createLeaderboard();
        lb->addScore(10, 500);
        lb->addScore(20, 500);  // Same score as player 10
        lb->addScore(5, 600);

        vector<int> top = lb->getTopN(3);
        // Order should be: ID 5 (score 600), ID 10 (score 500), ID 20 (score 500)
        bool correct = (top.size() == 3) &&
                       (top[0] == 5) &&
                       (top[1] == 10) &&
                       (top[2] == 20);

        delete lb;
        return correct;
    }());

    // Test 3: Remove Player
    runner.runTest("Leaderboard: Remove Player", [&]() {
        Leaderboard* lb = createLeaderboard();
        lb->addScore(1, 100);
        lb->addScore(2, 200);
        lb->addScore(3, 300);

        lb->removePlayer(2);  // Remove middle player

        vector<int> top = lb->getTopN(3);
        bool correct = (top.size() == 2) &&
                       (top[0] == 3) &&
                       (top[1] == 1);

        delete lb;
        return correct;
    }());

    // Test 4: Update Player Score
    runner.runTest("Leaderboard: Update Existing Player Score", [&]() {
        Leaderboard* lb = createLeaderboard();
        lb->addScore(1, 100);
        lb->addScore(2, 200);
        lb->addScore(1, 300);  // Update player 1's score

        vector<int> top = lb->getTopN(2);
        bool correct = (top.size() == 2) &&
                       (top[0] == 1) &&  // Now highest score
                       (top[1] == 2);

        delete lb;
        return correct;
    }());

    // Test 5: GetTopN more than available
    runner.runTest("Leaderboard: GetTopN(5) with 3 players", [&]() {
        Leaderboard* lb = createLeaderboard();
        lb->addScore(1, 100);
        lb->addScore(2, 200);
        lb->addScore(3, 150);

        vector<int> top = lb->getTopN(5);
        bool correct = (top.size() == 3) &&
                       (top[0] == 2) &&
                       (top[1] == 3) &&
                       (top[2] == 1);

        delete lb;
        return correct;
    }());

    // Test 6: GetTopN(0)
    runner.runTest("Leaderboard: GetTopN(0) returns empty", [&]() {
        Leaderboard* lb = createLeaderboard();
        lb->addScore(1, 100);
        lb->addScore(2, 200);

        vector<int> top = lb->getTopN(0);
        bool correct = top.empty();

        delete lb;
        return correct;
    }());
}

void test_PartA_AuctionTree() {
    cout << "\n--- Part A.3: AuctionTree (Red-Black Tree) ---" << endl;

    // Test 1: Basic Insertion
    runner.runTest("AuctionTree: Basic Insert", [&]() {
        AuctionTree* tree = createAuctionTree();
        tree->insertItem(1, 100);
        tree->insertItem(2, 200);
        tree->insertItem(3, 150);

        // Since we can't directly inspect the tree structure,
        // we test by inserting and deleting without crash
        tree->deleteItem(2);

        delete tree;
        return true;  // If no crash, test passes
    }());

    // Test 2: Delete Root
    runner.runTest("AuctionTree: Delete Root Node", [&]() {
        AuctionTree* tree = createAuctionTree();
        tree->insertItem(1, 100);
        tree->insertItem(2, 200);

        tree->deleteItem(1);  // Delete root

        // Insert more to test tree still works
        tree->insertItem(3, 150);
        tree->insertItem(4, 50);

        delete tree;
        return true;  // No crash means success
    }());

    // Test 3: Delete Non-existent Item
    runner.runTest("AuctionTree: Delete Non-existent Item", [&]() {
        AuctionTree* tree = createAuctionTree();
        tree->insertItem(1, 100);
        tree->insertItem(2, 200);

        tree->deleteItem(999);  // Non-existent

        // Tree should still work
        tree->insertItem(3, 300);

        delete tree;
        return true;  // No crash
    }());

    // Test 4: Duplicate Prices, Different IDs
    runner.runTest("AuctionTree: Items with Same Price", [&]() {
        AuctionTree* tree = createAuctionTree();
        tree->insertItem(1, 100);
        tree->insertItem(2, 100);  // Same price as item 1
        tree->insertItem(3, 100);  // Same price as item 1 and 2

        // Delete middle one
        tree->deleteItem(2);

        // Insert more
        tree->insertItem(4, 50);
        tree->insertItem(5, 150);

        delete tree;
        return true;  // No crash
    }());

    // Test 5: Complex Insert/Delete Pattern
    runner.runTest("AuctionTree: Complex Operations", [&]() {
        AuctionTree* tree = createAuctionTree();

        // Insert in random order
        tree->insertItem(5, 500);
        tree->insertItem(2, 200);
        tree->insertItem(8, 800);
        tree->insertItem(1, 100);
        tree->insertItem(7, 700);

        // Delete some
        tree->deleteItem(2);
        tree->deleteItem(8);

        // Insert more
        tree->insertItem(3, 300);
        tree->insertItem(6, 600);

        // Delete leaf
        tree->deleteItem(1);

        // Delete node with one child
        tree->deleteItem(3);

        delete tree;
        return true;  // No crash
    }());

    // Test 6: Delete All Items
    runner.runTest("AuctionTree: Delete All Items", [&]() {
        AuctionTree* tree = createAuctionTree();

        tree->insertItem(1, 100);
        tree->insertItem(2, 200);
        tree->insertItem(3, 300);

        tree->deleteItem(2);
        tree->deleteItem(1);
        tree->deleteItem(3);

        // Tree should be empty but still functional
        tree->insertItem(4, 400);

        delete tree;
        return true;  // No crash
    }());
}

// ==========================================
// PART B: INVENTORY SYSTEM TESTS
// ==========================================
void test_PartB_InventorySystem() {
    cout << "\n--- Part B: Inventory System ---" << endl;

    // 1. Loot Splitting
    runner.runTest("optimizeLootSplit: {1,2,4} -> diff 1", [&]() {
        vector<int> coins = {1,2,4};
        InventorySystem inv;
        return inv.optimizeLootSplit(3, coins) == 1;
    }());

    runner.runTest("optimizeLootSplit: {2,2} -> diff 0", [&]() {
        vector<int> coins = {2,2};
        InventorySystem inv;
        return inv.optimizeLootSplit(2, coins) == 0;
    }());

    // 2. Knapsack
    runner.runTest("maximizeCarryValue: capacity 3, items {{1,10},{2,15},{3,40}} -> 40", [&]() {
        vector<pair<int,int>> items = {{1,10},{2,15},{3,40}};
        InventorySystem inv;
        return inv.maximizeCarryValue(3, items) == 40;
    }());

    runner.runTest("maximizeCarryValue: capacity 5, items {{1,10},{2,15},{3,40}} -> 55", [&]() {
        vector<pair<int,int>> items = {{1,10},{2,15},{3,40}};
        InventorySystem inv;
        return inv.maximizeCarryValue(5, items) == 55;
    }());

    // 3. String Possibilities
    runner.runTest("countStringPossibilities: 'uu' -> 2", [&]() {
        InventorySystem inv;
        return inv.countStringPossibilities("uu") == 2;
    }());

    runner.runTest("countStringPossibilities: 'uunn' -> 4", [&]() {
        InventorySystem inv;
        return inv.countStringPossibilities("uunn") == 4;
    }());
}

// ==========================================
// PART C: WORLD NAVIGATOR TESTS
// ==========================================
void test_PartC_Navigator() {
    cout << "\n--- Part C: World Navigator ---" << endl;

    // 1. Safe Passage (Path Exists)
    runner.runTest("PathExists: 0->1->2 -> True", [&]() {
        vector<vector<int>> edges = {{0, 1}, {1, 2}};
        return WorldNavigator::pathExists(3, edges, 0, 2) == true;
    }());

    // 2. Safe Passage (No Path)
    runner.runTest("PathExists: 0->2 (No Path) -> False", [&]() {
        vector<vector<int>> edges = {{0, 1}, {2, 3}};
        return WorldNavigator::pathExists(4, edges, 0, 2) == false;
    }());

    // 3. Single Node Start=End
    runner.runTest("PathExists: Single Node Start=End -> True", [&]() {
        vector<vector<int>> edges = {};
        return WorldNavigator::pathExists(1, edges, 0, 0) == true;
    }());

    // 4. Cycle Graph
    runner.runTest("PathExists: Cycle Graph 0->1->2->0, 0->2 -> True", [&]() {
        vector<vector<int>> edges = {{0,1},{1,2},{2,0}};
        return WorldNavigator::pathExists(3, edges, 0, 2) == true;
    }());

    // 5. MinBribeCost
    runner.runTest("MinBribeCost: Triangle Graph -> Cost 15", [&]() {
        vector<vector<int>> roads = {
                {0, 1, 10, 0},
                {1, 2, 5, 0},
                {0, 2, 20, 0}
        };
        return WorldNavigator::minBribeCost(3, 3, 1, 1, roads) == 15;
    }());

    // 6. MinBribeCost Disconnected
    runner.runTest("MinBribeCost: Disconnected Graph -> INF", [&]() {
        vector<vector<int>> roads = {
                {0, 1, 5, 0}
        };
        return WorldNavigator::minBribeCost(3, 1, 1, 1, roads) == -1;
    }());

    // 7. MinBribeCost Square
    runner.runTest("MinBribeCost: Square Graph -> Cost 9", [&]() {
        vector<vector<int>> roads = {
                {0,1,3,0},{1,2,4,0},{2,3,5,0},{3,0,2,0}
        };
        return WorldNavigator::minBribeCost(4, 4, 1, 1, roads) == 9;
    }());

    // 8. MinBribeCost Single Node
    runner.runTest("MinBribeCost: Single Node -> Cost 0", [&]() {
        vector<vector<int>> roads = {};
        return WorldNavigator::minBribeCost(1, 0, 1, 1, roads) == 0;
    }());

    // BinarySum Tests
    runner.runTest("BinarySum: Line Graph -> '100'", [&]() {
        vector<vector<int>> roads = {{0, 1, 4}};
        return WorldNavigator::sumMinDistancesBinary(2, roads) == "100";
    }());

    runner.runTest("BinarySum: Disconnected Graph -> '0'", [&]() {
        vector<vector<int>> roads = {};
        return WorldNavigator::sumMinDistancesBinary(1, roads) == "0";
    }());

    runner.runTest("BinarySum: Complete Graph 3 nodes -> '100'", [&]() {
        vector<vector<int>> roads = {{0,1,1},{1,2,1},{0,2,2}};
        return WorldNavigator::sumMinDistancesBinary(3, roads) == "100";
    }());
}

// ==========================================
// PART D: SERVER KERNEL TESTS
// ==========================================
void test_PartD_ServerKernel() {
    cout << "\n--- Part D: Server Kernel (Greedy) ---" << endl;

    // Basic tests
    runner.runTest("TaskScheduler: {A,A,B}, n=2 -> 4", [&]() {
        vector<char> tasks = {'A','A','B'};
        ServerKernel sk;
        return sk.minIntervals(tasks, 2) == 4;
    }());

    runner.runTest("TaskScheduler: {A,A,A}, n=2 -> 7", [&]() {
        vector<char> tasks = {'A','A','A'};
        ServerKernel sk;
        return sk.minIntervals(tasks, 2) == 7;
    }());

    runner.runTest("TaskScheduler: {A,B,C}, n=2 -> 3", [&]() {
        vector<char> tasks = {'A','B','C'};
        ServerKernel sk;
        return sk.minIntervals(tasks, 2) == 3;
    }());

    runner.runTest("TaskScheduler: {A,A,A,B,B,B}, n=2 -> 8", [&]() {
        vector<char> tasks = {'A','A','A','B','B','B'};
        ServerKernel sk;
        return sk.minIntervals(tasks, 2) == 8;
    }());

    runner.runTest("TaskScheduler: {A,A,A}, n=0 -> 3", [&]() {
        vector<char> tasks = {'A','A','A'};
        ServerKernel sk;
        return sk.minIntervals(tasks, 0) == 3;
    }());

    runner.runTest("TaskScheduler: {A}, n=5 -> 1", [&]() {
        vector<char> tasks = {'A'};
        ServerKernel sk;
        return sk.minIntervals(tasks, 5) == 1;
    }());
}

// ==========================================
// PART D: SERVER KERNEL (Doctor Test Cases)
// ==========================================
void test_PartD_ServerKernel_Doctor() {
    cout << "\n--- Part D: Server Kernel (Doctor Test Cases) ---" << endl;

    runner.runTest("Doctor: {A,A,B}, n=2 -> 4", [&]() {
        vector<char> tasks = {'A','A','B'};
        ServerKernel sk;
        return sk.minIntervals(tasks, 2) == 4;
    }());

    runner.runTest("Doctor: {A,A,A}, n=2 -> 7", [&]() {
        vector<char> tasks = {'A','A','A'};
        ServerKernel sk;
        return sk.minIntervals(tasks, 2) == 7;
    }());

    runner.runTest("Doctor: {A,B,C}, n=2 -> 3", [&]() {
        vector<char> tasks = {'A','B','C'};
        ServerKernel sk;
        return sk.minIntervals(tasks, 2) == 3;
    }());

    runner.runTest("Doctor: {A,A,A,B,B,B}, n=2 -> 8", [&]() {
        vector<char> tasks = {'A','A','A','B','B','B'};
        ServerKernel sk;
        return sk.minIntervals(tasks, 2) == 8;
    }());
}

// ==========================================
// COMPREHENSIVE INTEGRATION TESTS
// ==========================================
void test_Integration() {
    cout << "\n--- Integration Tests ---" << endl;

    // Test that all factory functions work
    runner.runTest("Integration: All Factory Functions", [&]() {
        PlayerTable* pt = createPlayerTable();
        Leaderboard* lb = createLeaderboard();
        AuctionTree* at = createAuctionTree();

        bool success = (pt != nullptr) && (lb != nullptr) && (at != nullptr);

        delete pt;
        delete lb;
        delete at;
        return success;
    }());
}

// ==========================================
// MAIN
// ==========================================
int main() {
    cout << "Arcadia Engine - Comprehensive Tests" << endl;
    cout << "=====================================" << endl;

    // Seed random for skip list tests
    srand(time(0));

    // Run all test suites
    test_PartA_PlayerTable();
    test_PartA_Leaderboard();
    test_PartA_AuctionTree();
    test_PartB_InventorySystem();
    test_PartC_Navigator();
    test_PartD_ServerKernel();
    test_PartD_ServerKernel_Doctor();
    test_Integration();

    runner.printSummary();

    return 0;
}