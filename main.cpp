#include "bigint.h"
#include <queue>
#include <iostream>
int main() {
	int N = 100000;
	std::queue<bigint> que;
	for(int i = 1; i <= N; ++i) {
		que.push(bigint(i));
	}
	while(que.size() >= 2) {
		bigint b1 = que.front(); que.pop();
		bigint b2 = que.front(); que.pop();
		// std::cerr << "!" << ' ' << b1 << ' ' << b2 << ' ' << que.size() << std::endl;
		que.push(b1 * b2);
	}
	bigint ans = que.front(); que.pop();
	std::cout << ans << std::endl;
	std::cerr << double(clock()) / CLOCKS_PER_SEC << std::endl;
	return 0;
}