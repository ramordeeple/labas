#include <iostream>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <algorithm>
#include <bitset>


using namespace std;
using boost::multiprecision::cpp_int;

string toBinary(cpp_int x);

int main() {
    cpp_int num = 1;

    // Вычисляем 2^100
    for (int i = 0; i < 100; i++) num *= 2;

    string binary = toBinary(num);

    cout << "2^100 in binary: " << binary << endl;
    cout << "Number of bits: " << binary.size() << endl;

    return EXIT_SUCCESS;
}

string toBinary(cpp_int x) {
    if (x == 0) return "0";
    string s;
    while (x > 0) {
        s.push_back('0' + int(x % 2));
        x /= 2;
    }
    reverse(s.begin(), s.end());
    return s;
}
