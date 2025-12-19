#include <iostream>
#include <vector>
using namespace std;

// ----------------------------
// Sequential Search (Loop)
// ----------------------------
bool SequentialSearch(vector<long long> vec, long long target) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == target)
            return true;
    }
    return false;
}

// ----------------------------
// Sequential Search (Recursive)
// ----------------------------
bool RecursiveSequential(vector<long long> vec, long long target, int i) {
    if (i == vec.size())
        return false;
    if (vec[i] == target)
        return true;
    return RecursiveSequential(vec, target, i + 1);
}

// ----------------------------
// Binary Search (Iterative)
// ----------------------------
int binarySearchIterative(vector<long long> list, long long target) {
    int left = 0, right = list.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (list[mid] == target) return mid;
        else if (list[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

// ----------------------------
// Binary Search (Recursive)
// ----------------------------
int binarySearchRecursive(vector<long long> list, long long target, int left, int right) {
    if (left > right) return -1;
    int mid = left + (right - left) / 2;
    if (list[mid] == target) return mid;
    else if (list[mid] < target) return binarySearchRecursive(list, target, mid + 1, right);
    else return binarySearchRecursive(list, target, left, mid - 1);
}

// ----------------------------
// Main - Test all search algorithms
// ----------------------------
int main() {
    long long n, target;
    cout << "Enter the size of the list: ";
    cin >> n;

    vector<long long> vec(n);
    cout << "Enter the numbers in sorted order (for binary search):\n";
    for (int i = 0; i < n; i++) {
        cin >> vec[i];
    }

    cout << "Enter the number to search for: ";
    cin >> target;

    cout << "\n=== Search Algorithms Test ===\n";

    // Sequential Search (Loop)
    cout << "Sequential Search (Loop): ";
    cout << (SequentialSearch(vec, target) ? "Found" : "Not Found") << endl;

    // Sequential Search (Recursive)
    cout << "Sequential Search (Recursive): ";
    cout << (RecursiveSequential(vec, target, 0) ? "Found" : "Not Found") << endl;

    // Binary Search (Iterative)
    cout << "Binary Search (Iterative): ";
    int indexIter = binarySearchIterative(vec, target);
    if (indexIter != -1) cout << "Found at index " << indexIter << endl;
    else cout << "Not Found" << endl;

    // Binary Search (Recursive)
    cout << "Binary Search (Recursive): ";
    int indexRec = binarySearchRecursive(vec, target, 0, vec.size() - 1);
    if (indexRec != -1) cout << "Found at index " << indexRec << endl;
    else cout << "Not Found" << endl;

    return 0;
}
