#include "newbigint.h"
using namespace std;
int main() {
	int initial_precis = 8;
	int max_precis = 1024;
	int precis = 8;
	bigint b = bigint(1) << precis;
	bigint pre;
	while (pre != b) {
		pre = b;
		bigint c = (bigint(2) << (2 * precis)) / b;
		b = (b + c) / 2;
	}
	pre = bigint();
	while (pre != b) {
		pre = b;
		bigint c = (bigint(2) << (2 * precis)) / b;
		b = (b + c) / 2;
		int next_precis = std::min(2 * precis, max_precis);
		if (precis != next_precis) {
			b <<= next_precis - precis;
			precis = next_precis;
		}
	}
	ios_base::sync_with_stdio(false);
	string ans = b.to_string();
	cout << ans[0] << "." << ans.substr(1) << '\n';
	return 0;
}