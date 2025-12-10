/**********************************************************************
 IDs:
 20000000 – Pre-master placeholder
 Name:
 Seba Ibraheem

 Suffix Array Construction using Prefix Doubling
 
************************************************************************/

#include <iostream>
using namespace std;

/**********************************************************************
 Structure to store suffix information:
 - ind : index of suffix in text
 - r1  : current rank of the suffix
 - r2  : rank of suffix starting at ind + k (tie-breaker)
************************************************************************/
struct Node {
    int ind;
    int r1;
    int r2;
};

/**********************************************************************
 Merge function for merge sort based on (r1,r2)
************************************************************************/
void merge(Node *A, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Node *L = new Node[n1];
    Node *R = new Node[n2];

    for (int i = 0; i < n1; i++)
        L[i] = A[left + i];
    for (int i = 0; i < n2; i++)
        R[i] = A[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].r1 < R[j].r1 ||
            (L[i].r1 == R[j].r1 && L[i].r2 <= R[j].r2)) {
            A[k++] = L[i++];
        } else {
            A[k++] = R[j++];
        }
    }

    while (i < n1)
        A[k++] = L[i++];
    while (j < n2)
        A[k++] = R[j++];

    delete[] L;
    delete[] R;
}

/**********************************************************************
 Merge sort wrapper for (r1,r2)
************************************************************************/
void mergeSort(Node *A, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    mergeSort(A, left, mid);
    mergeSort(A, mid + 1, right);
    merge(A, left, mid, right);
}

/**********************************************************************
 Merge function for sorting purely by `ind`
************************************************************************/
void mergeByInd(Node *A, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Node *L = new Node[n1];
    Node *R = new Node[n2];

    for (int i = 0; i < n1; i++)
        L[i] = A[left + i];
    for (int i = 0; i < n2; i++)
        R[i] = A[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].ind <= R[j].ind)
            A[k++] = L[i++];
        else
            A[k++] = R[j++];
    }

    while (i < n1)
        A[k++] = L[i++];
    while (j < n2)
        A[k++] = R[j++];

    delete[] L;
    delete[] R;
}

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
    char text[10000];   // input text
    int n;              // length of text
    int sa[10000];      // final suffix array

public:
    /**************************************************************
     Constructor copies char* into internal array (no string)
    **************************************************************/
    SuffixArray(const char *s) {
        n = 0;
        while (s[n] != '\0') {
            text[n] = s[n];
            n++;
        }
        text[n] = '\0';
    }

    /**************************************************************
     Construct suffix array using Prefix Doubling O(n (log n)^2)
    **************************************************************/
    void ConstructUsingPrefixDoubling() {
        Node *arr = new Node[n];

        // Initial ranks based on first character and next character
        for (int i = 0; i < n; i++) {
            arr[i].ind = i;
            arr[i].r1 = (unsigned char)text[i]; // first character
            arr[i].r2 = (i + 1 < n ? (unsigned char)text[i + 1] : -1); // next char
        }

        // Initial sort by (r1,r2)
        mergeSort(arr, 0, n - 1);

        int *rank = new int[n];       // rank[i] = rank of suffix at index i
        int *tmpRank = new int[n];    // temporary rank array for updating

        // Doubling loop: compare first len characters of suffix
        for (int len = 2;; len *= 2) {

            // Step 1: assign new ranks based on sorted (r1,r2)
            int r = 0;
            tmpRank[arr[0].ind] = 0;
            int old_r1 = arr[0].r1;
            int old_r2 = arr[0].r2;

            for (int i = 1; i < n; i++) {
                if (arr[i].r1 == old_r1 && arr[i].r2 == old_r2) {
                    tmpRank[arr[i].ind] = r;
                } else {
                    r++;
                    tmpRank[arr[i].ind] = r;
                    old_r1 = arr[i].r1;
                    old_r2 = arr[i].r2;
                }
            }

            // Step 2: update r1 in arr
            for (int i = 0; i < n; i++)
                arr[i].r1 = tmpRank[arr[i].ind];

            // Step 3: if all ranks unique, done
            if (r == n - 1) break;

            // Step 4: sort by index to update r2
            mergeSortByInd(arr, 0, n - 1);
            for (int i = 0; i < n; i++) {
                int next = arr[i].ind + len;
                arr[i].r2 = (next < n ? tmpRank[next] : -1);
            }

            // Step 5: sort again by (r1,r2)
            mergeSort(arr, 0, n - 1);
        }

        // Extract final suffix array
        for (int i = 0; i < n; i++)
            sa[i] = arr[i].ind;

        delete[] arr;
        delete[] rank;
        delete[] tmpRank;
    }

    /**************************************************************
     Print the suffix array
    **************************************************************/
    void Print() {
        for (int i = 0; i < n; i++)
            cout << sa[i] << " ";
        cout << "\n";
    }
};

int main() {
    SuffixArray t("ACGACTACGATAAC$");

    t.ConstructUsingPrefixDoubling();

    t.Print(); // Expected: 14 11 12 0 6 3 9 13 1 7 4 2 8 10 5

    return 0;
}
