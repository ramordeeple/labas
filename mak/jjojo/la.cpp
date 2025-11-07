#include <iostream>
#include <string>
#include <vector>
#include <map>


using namespace std;

string toRoman(int);
int fromRoman(const string&);

int main() {
    int num;

    while (true) {
        cout << "Input a number (from 1 to 4000): ";
        cin >> num;

        if (num < 1 || num > 4000) {
            cout << "Number should be 1 to 4000!" << endl;
        }
        else {
            break; 
        }
    }

    cout << "input number: " << num << "\nRoman: " << toRoman(num) << endl;


    string roman = "";

    cin >> roman;
    cout << fromRoman(roman) << endl;


    return EXIT_SUCCESS;
}

string toRoman(int n) {
    map<int, string, greater<int>> romanMap = {
        {1000, "M"}, 
        {900, "CM"}, 
        {500, "D"}, 
        {400, "CD"},
        {100, "C"}, 
        {90, "XC"}, 
        {50, "L"}, 
        {40, "XL"},
        {10, "X"}, 
        {9, "IX"}, 
        {5, "V"}, 
        {4, "IV"}, 
        {1, "I"}
    };

    string roman = "";

    for (auto it = romanMap.begin(); it != romanMap.end(); ++it) {
        int key = it->first;
        string symbol = it->second;

        while (n >= key) {
            n -= key;
            roman += symbol;
        }
    }

    return roman;
}


int fromRoman(const string& s) {
    map<string, int> romanMap = {
        {"M", 1000}, 
        {"CM", 900}, 
        {"D", 500}, 
        {"CD", 400},
        {"C", 100}, 
        {"XC", 90}, 
        {"L", 50}, 
        {"XL", 40},
        {"X", 10}, 
        {"IX", 9}, 
        {"V", 5}, 
        {"IV", 4}, 
        {"I", 1}
    };

    int result = 0;
    for (size_t i = 0; i < s.length();) {
        if (i + 1 < s.length() && romanMap.count(s.substr(i, 2))) {
            result += romanMap[s.substr(i, 2)];
            i += 2;
        }
        else { 
            result += romanMap[s.substr(i, 1)];
            i += 1;
        }
    }
    return result;
}