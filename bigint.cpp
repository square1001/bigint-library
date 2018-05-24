#include <ctime>
#include <iostream>
#include <algorithm>
#include "bigint.h"
using namespace std;
int main() {
	int u = clock();
	bigint a = 2;
	for (int i = 0; i < 20; i++) {
		a *= a;
	}
	int v = clock();
	cout << a << '\n';
	cout << v - u << '\n';
	return 0;
}