#include <bits/stdc++.h>
using namespace std;

long long FibonacciRecursion(long long num) {
    if (num == 0)
        return 0;
    if (num == 1)
        return 1;
    return FibonacciRecursion(num - 1) + FibonacciRecursion(num - 2);
}

int main() {
    long long num;
    cout << "Enter a number to get its Fibonacci: ";
    cin >> num;
    cout << "The Fibonacci of " << num << " is: " << FibonacciRecursion(num);
    return 0;
}


