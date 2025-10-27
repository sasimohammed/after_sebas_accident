#include <iostream>
#include <vector>

using namespace std;

bool SequentialSearch(vector<long long>vec,long long target)
{
    for(int i=0;i<vec.size();i++)
    {
        if(vec[i] == target)
        {
            return true;
        }
    }
    return false;
}

bool RecursiveSequential(vector<long long>vec,long long target,int i)
{
    if(i==vec.size())
    {
        return false;
    }
    else
    {
        if(vec[i] == target)
        {
            return true;
        }
        else
        {
            return RecursiveSequential(vec,target,i+1);
        }
    }
}


int main() {

    long long n, target;
    cout << "Enter the size of the list: ";
    cin >> n;
    vector<long long> vec(n);
    for (int i = 0; i < n; i++)
    {
        cout << "Enter number " << i + 1 << ": ";
        cin >> vec[i];
    }

    cout << "Enter the number to search for: ";
    cin >> target;

    cout << "Choose search method:\n1. Loop\n2. Recursion\nYour choice: ";
    int choice;
    cin >> choice;

    bool found;
    if(choice==1)
    {
        found = SequentialSearch(vec, target);
    }
    else if(choice==2)
    {
        found = RecursiveSequential(vec, target, 0);
    }
    else
    {
        cout<<"Invalid Choice"<<endl;
        return 0;
    }
    cout << (found ? "Found" : "Not found")<<endl;


}
