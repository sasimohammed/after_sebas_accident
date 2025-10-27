#include<iostream>
#include<vector>
using namespace std;

typedef vector<vector<long long>> matrix;


matrix multiply(matrix A, matrix B) {
    matrix C(2, vector<long long>(2));
    C[0][0] = A[0][0]*B[0][0] + A[0][1]*B[1][0];
    C[0][1] = A[0][0]*B[0][1] + A[0][1]*B[1][1];
    C[1][0] = A[1][0]*B[0][0] + A[1][1]*B[1][0];
    C[1][1] = A[1][0]*B[0][1] + A[1][1]*B[1][1];
    return C;
}


matrix power(matrix A, long long n) {
    if (n == 1)
        return A;
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

int main()
{
    long long n;
    cout << "Enter n: ";
    cin >> n;
    cout << "F(" << n << ") = " << fibonacci(n)<<endl;

}
