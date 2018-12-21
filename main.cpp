#include "newbigfloat.h"
#include <ctime>
using namespace std;
bigint sqrt(bigint x) {
	int max_scale = (x.size() + 1) / 2;
	int scale = min(4, max_scale);
	bigint a = bigint(1) << (scale - 1), pre;
	int loop = 0, u = clock();
	while (pre != a) {
		pre = a;
		bigint xd = x;
		if (x.size() > 2 * scale) xd >>= (x.size() - 2 * scale + x.size() % 2);
		bigint b = xd / a;
		a = (a + b).divide_by_2();
		//cout << "Loop #" << ++loop << ": " << scale << ' ' << clock() - u << endl;
	}
	pre = bigint();
	while (pre != a) {
		pre = a;
		bigint xd = x;
		if (x.size() > 2 * scale) xd >>= (x.size() - 2 * scale + x.size() % 2);
		bigint b = xd / a;
		a = (a + b).divide_by_2();
		int next_scale = std::min(max_scale, scale * 2);
		a <<= next_scale - scale;
		//cout << "Loop #" << ++loop << ": " << scale << ' ' << clock() - u << endl;
		scale = next_scale;
	}
	return a;
}
bigfloat sqrt(bigfloat x, int final_scale) {
	x <<= 2 * final_scale;
	bigint b = x.to_bigint();
	b = sqrt(b);
	bigfloat ans = b;
	ans >>= final_scale;
	return ans;
}
vector<string> ps;
bigfloat calculate_pi(int final_scale) {
	bigfloat a(bigint(1) << final_scale, -final_scale);
	bigfloat b = bigfloat(bigint(1) << (2 * final_scale), -2 * final_scale) / sqrt(bigint(2), final_scale);
	bigfloat t = bigfloat(bigint(1) << final_scale, -final_scale).divide_by_2().divide_by_2();
	bigfloat p(1);
	bigfloat ans;
	int u = clock();
	int step = 0;
	while (true) {
		bigfloat eval = (a + b) * (a + b) / (t * bigfloat(4));
		//cout << eval << endl;
		ps.push_back(eval.to_string());
		if (eval == ans) break;
		ans = eval;
		bigfloat xa = (a + b).divide_by_2();
		bigfloat xb = sqrt(a * b, final_scale);
		bigfloat xt = t - p * (a - xa) * (a - xa);
		bigfloat xp = p * bigfloat(2);
		xt.set_scale(-final_scale);
		a = xa;
		b = xb;
		t = xt;
		p = xp;
		//cout << "Step #" << ++step << " Completed! Time = " << clock() - u << " miliseconds" << endl;
	}
	return ans;
}
void sqrt_verify() {
	for (int i = 256; i <= 1048576; i *= 2) {
		int u = clock();
		bigfloat b = sqrt(bigfloat(2), i);
		cout << i * 4 << " DIGITS: TIME = " << clock() - u << " MILLISECONDS" << endl;
	}
}
#include <fstream>
void pi_verify() {
	int max_value = 262144;
	for (int i = 16; i <= max_value; i *= 2) {
		int u = clock();
		bigfloat b = calculate_pi(i);
		cout << i * 4 << " DIGITS: TIME = " << clock() - u << " MILLISECONDS" << endl;
		if (i == max_value) {
			ofstream fos("pi_1048576.txt");
			fos << b << '\n';
		}
	}
}
int main() {
	pi_verify();
	return 0;
}