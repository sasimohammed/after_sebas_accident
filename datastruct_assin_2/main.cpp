/**********************************************************************
/*
IDs & Names:
20231079 - sohila walid mohammed
20231087 - seba ibraheem ezzat
20230289 - katren nader nagy
00000000 -
00000000 -
*/

//Suffix Array Construction using Prefix Doubling
//************************************************************************/

#include <iostream>
using namespace std;

/**********************************************************************
 Structure to store suffix information for each suffix of the text:
 - ind : the starting index of the suffix in the original text
 - r1  : current rank of this suffix (used for sorting)
 - r2  : next rank for tie-breaking if r1 is the same
************************************************************************/
struct Node {
    int ind; // where this suffix starts in text
    int r1;  // rank of this suffix
    int r2;  // rank of next part, helps break ties
};

/**********************************************************************
 Merge function for merge sort based on (r1,r2)
 This merges two sorted halves into one sorted array
************************************************************************/
void merge(Node *A, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Node *L = new Node[n1];
    Node *R = new Node[n2];

    // copy left half
    for (int i = 0; i < n1; i++) L[i] = A[left + i];
    // copy right half
    for (int i = 0; i < n2; i++) R[i] = A[mid + 1 + i];

    int i = 0, j = 0, k = left;

    // merge step
    while (i < n1 && j < n2) {
        if (L[i].r1 < R[j].r1 || (L[i].r1 == R[j].r1 && L[i].r2 <= R[j].r2))
            A[k++] = L[i++];
        else
            A[k++] = R[j++];
    }

    // copy remaining elements if any
    while (i < n1) A[k++] = L[i++];
    while (j < n2) A[k++] = R[j++];

    delete[] L;
    delete[] R;
}

/**********************************************************************
 Merge sort wrapper for (r1,r2)
************************************************************************/
void mergeSort(Node *A, int left, int right) {
    if (left >= right) return; // base case: one element
    int mid = (left + right) / 2;
    mergeSort(A, left, mid);       // sort left half
    mergeSort(A, mid + 1, right);  // sort right half
    merge(A, left, mid, right);    // merge sorted halves
}

/**********************************************************************
 Merge function for sorting by `ind` only
 We need this to realign suffixes by their original index when updating r2
************************************************************************/
void mergeByInd(Node *A, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Node *L = new Node[n1];
    Node *R = new Node[n2];

    for (int i = 0; i < n1; i++) L[i] = A[left + i];
    for (int i = 0; i < n2; i++) R[i] = A[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].ind <= R[j].ind)
            A[k++] = L[i++];
        else
            A[k++] = R[j++];
    }

    while (i < n1) A[k++] = L[i++];
    while (j < n2) A[k++] = R[j++];

    delete[] L;
    delete[] R;
}

/**********************************************************************
 Merge sort wrapper for `ind`
************************************************************************/
void mergeSortByInd(Node *A, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    mergeSortByInd(A, left, mid);
    mergeSortByInd(A, mid + 1, right);
    mergeByInd(A, left, mid, right);
}

/**********************************************************************
 SuffixArray class
************************************************************************/
class SuffixArray {
private:
    char text[10000]; // input text stored as char array
    int n;            // length of input text
    int sa[10000];    // final suffix array to be filled

public:
    /**************************************************************
     Constructor: copy input string into internal array
    **************************************************************/
    SuffixArray(const char *s) {
        n = 0;
        while (s[n] != '\0') {
            text[n] = s[n]; // copy character by character
            n++;
        }
        text[n] = '\0'; // null terminate
    }

    /**************************************************************
     Construct suffix array using Prefix Doubling
     Step-by-step human explanation:
     1. Create array of Nodes {ind, r1, r2}
        - r1 = rank of first character
        - r2 = rank of next character
     2. Sort array by (r1,r2)
     3. Sequentially assign new ranks r1 (same rank if tie)
     4. Sort array by ind to realign
     5. Update r2 for next doubling step
     6. Repeat doubling length until all ranks unique
    **************************************************************/
    void ConstructUsingPrefixDoubling() {
        Node *arr = new Node[n];       // create array to hold suffix info
        int *tmpRank = new int[n];     // temporary array to update ranks

        // Step 0: initialize r1,r2 from first two characters
        for (int i = 0; i < n; i++) {
            arr[i].ind = i;
            arr[i].r1 = (unsigned char)text[i];
            arr[i].r2 = (i + 1 < n ? (unsigned char)text[i + 1] : -1);
        }

        // Step 1: initial sort by r1,r2
        mergeSort(arr, 0, n - 1);

        // Step 2: doubling loop (compare first len characters)
        for (int len = 2;; len *= 2) {
            int r = 0; // new rank counter
            tmpRank[arr[0].ind] = 0;
            int old_r1 = arr[0].r1;
            int old_r2 = arr[0].r2;

            // Step 2a: sequentially assign new ranks
            for (int i = 1; i < n; i++) {
                if (arr[i].r1 == old_r1 && arr[i].r2 == old_r2) {
                    tmpRank[arr[i].ind] = r; // tie, same rank
                } else {
                    r++;
                    tmpRank[arr[i].ind] = r; // increase rank
                    old_r1 = arr[i].r1;
                    old_r2 = arr[i].r2;
                }
            }

            // Step 2b: update r1 in arr
            for (int i = 0; i < n; i++)
                arr[i].r1 = tmpRank[arr[i].ind];

            // Step 2c: if all ranks unique, done
            if (r == n - 1) break;

            // Step 2d: sort by index to update r2
            mergeSortByInd(arr, 0, n - 1);
            for (int i = 0; i < n; i++) {
                int next = arr[i].ind + len;
                arr[i].r2 = (next < n ? tmpRank[next] : -1);
            }

            // Step 2e: sort again by r1,r2 for next iteration
            mergeSort(arr, 0, n - 1);
        }

        // Step 3: extract final suffix array
        for (int i = 0; i < n; i++)
            sa[i] = arr[i].ind;

        delete[] arr;
        delete[] tmpRank;
    }

    /**************************************************************
     Print the final suffix array
    **************************************************************/
    void Print() {
        for (int i = 0; i < n; i++)
            cout << sa[i] << " ";
        cout <<endl;
    }
};
