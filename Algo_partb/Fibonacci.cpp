#include <iostream>
#include <vector>
using namespace std;

// ----------------------------
// Recursive Method
// ----------------------------
long long FibonacciRecursion(long long num) {
    if (num == 0) return 0;
    if (num == 1) return 1;
    return FibonacciRecursion(num - 1) + FibonacciRecursion(num - 2);
}

// ----------------------------
// Dynamic Programming Method
// ----------------------------
long long nthFibonacci(long long n) {
    if (n <= 1) return n;

    vector<long long> dp(n + 1);
    dp[0] = 0;
    dp[1] = 1;

    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }

    return dp[n];
}

// ----------------------------
// Matrix Multiplication Method
// ----------------------------
typedef vector<vector<long long>> matrix;

matrix multiply(matrix A, matrix B) {
    matrix C(2, vector<long long>(2));
    C[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0];
    C[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1];
    C[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0];
    C[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1];
    return C;
}

matrix power(matrix A, long long n) {
    if (n == 1) return A;
    if (n % 2 == 0) {
        matrix half = power(A, n / 2);
        return multiply(half, half);
    } else {
        matrix half = power(A, n - 1);
        return multiply(A, half);
    }
}

long long fibonacci(long long n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    matrix T = {{1, 1}, {1, 0}};
    matrix result = power(T, n - 1);
    return result[0][0];
}

// ----------------------------
// Main - Test all methods
// ----------------------------
int main() {
    long long n;
    cout << "Enter n: ";
    cin >> n;

    cout << "\n=== Fibonacci Algorithms Test ===\n";

    cout << "Recursive Method result: " << FibonacciRecursion(n) << endl;
    cout << "Dynamic Programming result: " << nthFibonacci(n) << endl;
    cout << "Matrix Exponentiation result: " << fibonacci(n) << endl;


    if (FibonacciRecursion(n) == nthFibonacci(n) && nthFibonacci(n) == fibonacci(n))
        cout << "All methods give the same result.\n";
    else
        cout << "Results are different!\n";

    return 0;
}
