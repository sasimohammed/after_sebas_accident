#include<iostream>
#include<algorithm>
#include<vector>
using namespace std;


int leastInterval(vector<char>& tasks, int n) {
    vector<int> freq(26, 0);
    for (char t : tasks)
        freq[t - 'A']++;

    int maxFreq = *max_element(freq.begin(), freq.end());
    int countMax = 0;

    for (int f : freq)
        if (f == maxFreq)
            countMax++;

    return max(
            (int)tasks.size(),
            (maxFreq - 1) * (n + 1) + countMax
    );
}

int main()
{
    int n;
    cin>>n;
    vector<char>task;
    int size;
    cin>>size;
    for(int i=0;i<size;i++)
    {
        char x;
        cin>>x;
        task.push_back(x);
    }

    cout<<leastInterval(task,n);

}
