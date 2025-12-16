// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <functional>

using namespace std;


// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

//We are trying to find the min diff between two groubs 
// let say total sum is S and subset sum is x 
// we want two subsets be as can as even 
// so we need to minimize S-2x , so max x 
// so to be two evenly groubs their sum will be the closest to S/2
int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {

    int totalSum = 0;
    for(int i = 0; i < n; i++)
        totalSum += coins[i];

    int halfTotal = totalSum / 2;

    // isAchievable[j] = true if sum j is achievable
    // vector of size of halfTotal and intially all false
    vector<bool> isAchievable(halfTotal + 1, false);
    isAchievable[0] = true;   // base case

    // check coins one by one
    for(int i = 0; i < n; i++){
        for(int j = halfTotal; j >= coins[i]; j--){
            if(isAchievable[j - coins[i]])
                isAchievable[j] = true;
        }
    }

    // find best achievable sum <= halfTotal
    int best = 0;
    for(int j = halfTotal; j >= 0; j--){
        if(isAchievable[j]){
            best = j;
            break;
        }
    }

    return totalSum - 2 * best;
    
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {

    // Helper function to try all possibilities
    // index  → number of items we can still use
    // remainingCapacity → how much weight we can still carry
    function<int(int, int)> solve = [&](int index, int remainingCapacity) -> int {

        // Stop if no items left OR no capacity left
        if (index == 0 || remainingCapacity == 0)
            return 0;

        // Get weight/value of the current item
        int weight = items[index - 1].first;
        int value = items[index - 1].second;

        // Case 1: skip the current item
        int notPick = solve(index - 1, remainingCapacity);

        // Case 2: take the current item (if it fits)
        int pick = 0;
        if (weight <= remainingCapacity) {
            pick = value + solve(index - 1, remainingCapacity - weight);
        }

        // Return the better choice (take or skip)
        return max(pick, notPick);
    };

    // Start checking using all items and full capacity
    return solve(items.size(), capacity);
}



long long InventorySystem::countStringPossibilities(string s) {
    int n = s.size();
    if(n==0 || n== 1) return 1 ;

    const long long MOD = 1e9 + 7;
    long long dp [n] ;
    dp[0]=1 ;

    for (int i =1; i<n; i++){
        dp[i]=0;

        //normal step for all chars
        dp[i] = dp[i-1] ;

        if ((s[i] == 'u' && s[i-1] == 'u')|| (s[i] == 'n' && s[i-1] == 'n')) {

            if (i >= 2)
                dp[i] += dp[i - 2] % MOD;
            else
                dp[i] += 1 ;   // i == 1 case
        }

        dp[i] %= MOD;

    }
    
    return dp[n-1];
}


int main(){
    // int n;
    // cout<<"Enter Size of coins list:";
    // cin >> n ;

    // vector<int>coins(n);
    // cout<<"Enter the "<<n<<" coins value:";
    // for(int i =0 ; i<n; i ++) cin>>coins[i] ;

    // cout<< InventorySystem::optimizeLootSplit(n ,coins) ;

    
    // // items = { weight, value }
    // vector<pair<int, int>> items = {
    //     {1, 10},   // weight 2, value 3
    //     {2, 15},   // weight 3, value 4
    //     {3, 40}   // weight 5, value 8
    // };

    // int capacity = 5;

    // int maxValue = InventorySystem::maximizeCarryValue(capacity, items);

    // cout << "Maximum carry value = " << maxValue << endl;

    cout << InventorySystem::countStringPossibilities("uunn");
    cout << InventorySystem::countStringPossibilities("uu");
}