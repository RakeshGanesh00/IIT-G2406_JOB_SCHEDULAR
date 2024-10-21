#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;

int main()
{

    vector<int> arr;
    int n, input;

    cout << "kitna number ka array chiaye" << endl;
    cin >> n;

    for (int i = 1; i <= n; i++){
        cout << "arr number " << i << " = ";
        cin >> input;
        arr.push_back(input);
    }

    cout << "unsoarted array  ";
    for (int i = 0; i < n; i++){
        cout << arr[i] << " ";
    }

    cout << endl;
    cout << "unsoarted array  ";

    sort(arr.begin(), arr.end());

    for (int i = 0; i < n; i++)
    {
        cout << arr[i] << " ";
    }

    return 0;
}