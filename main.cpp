#include "bigint.h"
#include <iostream>
using namespace std;
int main() {
	// (sqrt(2) - 1)^(2^k) = b1 * sqrt(2) - b2
	bigint digit = 10;
	for(int i = 0; i < 5; ++i) {
		digit *= digit;
	}
	bigint b1 = 1, b2 = 1;
	for(int i = 0; i < 14; ++i) {
		bigint c1 = b1 * b2 * 2;
		bigint c2 = b1 * b1 * 2 + b2 * b2;
		b1 = c1; b2 = c2;
		cout << (b2 * digit) / b1 << endl;
	}
	return 0;
}