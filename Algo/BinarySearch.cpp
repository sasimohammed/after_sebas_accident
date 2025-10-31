#include <bits/stdc++.h>
using namespace std;

int binarySearchIterative(vector<long long>list,long long target) {
    int left = 0;
    int right = list.size() - 1;

    while (left <= right) {
        int mid = left + (right - left)/2;

        if (list[mid] == target)
            return mid;

        else if (list[mid] < target)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return -1;
}
int binarySearchRecursive(vector<long long>list,long long target,int left,int right){
    if (left > right)
        return -1;

    int mid = left + (right - left)/2;

    if (list[mid] == target) {
        return mid;
    }
    else if (list[mid] < target) {
        return binarySearchRecursive(list,target,mid + 1,right);
    }
    else {
        return binarySearchRecursive(list,target,left,mid - 1);
    }
}
int main() {
    vector<long long>list = {2,4,5,6,8,10,11,12,14,15};
    int target;

    cout << "Enter a number to search for: ";
    cin >> target;
    int Index = binarySearchIterative(list,target);

    if (Index != -1)
        cout << "Number found at index: " << Index << endl;
    else
        cout << "Number not found." << endl;

    return 0;
}
