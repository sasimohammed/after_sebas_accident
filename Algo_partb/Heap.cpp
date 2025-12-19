#include <bits/stdc++.h>
using namespace std;

// Heap implementation (Max-Heap)
template<typename T>
class Heap {
    vector<T> vec;


    void heapify_up(int i) {
        if(i && vec[(i - 1) / 2] < vec[i]) {
            swap(vec[i], vec[(i - 1) / 2]);
            heapify_up((i - 1) / 2);
        }
    }


    void heapify_down(int i) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int largest = i;

        if(left < vec.size() && vec[left] > vec[largest])
            largest = left;

        if(right < vec.size() && vec[right] > vec[largest])
            largest = right;

        if(largest != i) {
            swap(vec[i], vec[largest]);
            heapify_down(largest);
        }
    }

public:

    void insert(T data) {
        vec.push_back(data);
        heapify_up(vec.size() - 1);
    }

    T extractMax() {
        if(vec.empty())
            throw runtime_error("Heap is empty");
        T maxVal = vec[0];
        vec[0] = vec.back();
        vec.pop_back();
        if(!vec.empty()) heapify_down(0);
        return maxVal;
    }


    T extractMin() {
        if(vec.empty())
            throw runtime_error("Heap is empty");
        return *min_element(vec.begin(), vec.end());
    }


    void printHeap() {
        for(const auto& x : vec)
            cout << x << " ";
        cout << endl;
    }


    int size() {
        return vec.size();
    }


    vector<T> getVector() {
        return vec;
    }


    void clear() {
        vec.clear();
    }
};

// Priority Queue Implementation
template<typename T>
class PriorityQueue {
    Heap<T> heap;

public:

    void push(T value) {
        heap.insert(value);
    }


    T pop() {
        return heap.extractMax();
    }


    void print() {
        heap.printHeap();
    }


    int size() {
        return heap.size();
    }
};


template<typename T>
void heapSort(vector<T>& arr) {
    Heap<T> h;
    for(auto x : arr)
        h.insert(x);

    for(int i = arr.size() - 1; i >= 0; i--) {
        arr[i] = h.extractMax();
    }
}


int main() {

    cout << "=== Heap Test ===" << endl;
    Heap<int> h;
    h.insert(10);
    h.insert(20);
    h.insert(5);
    h.insert(30);

    cout << "Heap elements: ";
    h.printHeap();

    cout << "Max element: " << h.extractMax() << endl;
    cout << "Min element: " << h.extractMin() << endl;

    cout << "Heap after extractMax: ";
    h.printHeap();

    // Priority Queue test
    cout << "\n=== Priority Queue Test ===" << endl;
    PriorityQueue<int> pq;
    pq.push(50);
    pq.push(15);
    pq.push(60);
    pq.push(5);

    cout << "Priority Queue elements: ";
    pq.print();

    cout << "Pop from priority queue: " << pq.pop() << endl;
    cout << "Priority Queue after pop: ";
    pq.print();

    // Heap Sort test
    cout << "\n=== Heap Sort Test ===" << endl;
    vector<int> arr = {12, 3, 17, 8, 34, 25};
    cout << "Array before Heap Sort: ";
    for(auto x : arr) cout << x << " ";
    cout << endl;

    heapSort(arr);

    cout << "Array after Heap Sort: ";
    for(auto x : arr) cout << x << " ";
    cout << endl;

    return 0;
}
