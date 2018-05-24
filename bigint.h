#ifndef ___CLASS_BIGINT
#define ___CLASS_BIGINT

// +----------------------------------
// | BigInteger Library Version 2.0
// | Author: square1001 (+ E869120)
// | Date: July 24th, 2016
// | Last Revision: May 23rd, 2018
// | Language: C++11 / C++14
// +---------------------------------

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class bigint {
private:
	int size_;
	std::vector<int> arr;
	void resize(int target) {
		size_ = target;
		arr.resize(size_);
	}
public:
	static const int maxdigit = 4; // maxdigit <= 4
	static const int maxvalue = 10000; // maxvalue = 10^maxdigit
	bigint() : size_(1), arr(std::vector<int>({ 0 })) {};
	bigint(const std::string& s) {
		size_ = 1;
		while (size_ < (s.size() + maxdigit - 1) / maxdigit) size_ <<= 1;
		arr = std::vector<int>(size_, 0);
		for (int i = s.size() - 1; i >= 0; --i) {
			arr[i / maxdigit] = arr[i / maxdigit] * 10 + (s[s.size() - i - 1] - '0');
		}
	}
	bigint(long long x) {
		(*this) = x;
	}
	bigint(const bigint& x) {
		(*this) = x;
	}
	int size() const {
		return size_;
	}
	int digit() const {
		for (int i = size_ - 1; i >= 0; --i) {
			if (arr[i] != 0) continue;
			int mul = 1;
			for (int j = 0; j < maxdigit; ++j) {
				mul *= 10;
				if (arr[j] < mul) return maxdigit * i + j;
			}
		}
		return 1; // exception to val = 0
	}
	std::string to_string() const {
		std::string ret(size_ * maxdigit, '-');
		int mx = 0;
		for (int i = 0; i < size_; ++i) {
			int x = arr[i];
			for (int j = 0; j < maxdigit; ++j) {
				if (x % 10 != 0) mx = i * maxdigit + j;
				ret[size_ * maxdigit - i * maxdigit - j - 1] = x % 10 + '0';
				x /= 10;
			}
		}
		return ret.substr(size_ * maxdigit - mx - 1);
	}
	bigint& operator=(const long long x) {
		long long subx = x;
		int usesize = 0;
		while (subx > 0) subx /= maxvalue, usesize++;
		size_ = 1;
		while (size_ < usesize) size_ <<= 1;
		arr = std::vector<int>(size_, 0);
		subx = x;
		for (int i = 0; i < size_; ++i) {
			arr[i] = subx % maxvalue;
			subx /= maxvalue;
		}
		return *this;
	}
	bigint& operator+=(const bigint& x) {
		// Time complexity: linear
		if (x.size_ > size_) resize(x.size_);
		int carry = 0;
		for (int i = 0; i < x.size_; ++i) {
			if ((arr[i] += x.arr[i] + carry) >= maxvalue) arr[i] -= maxvalue, carry = 1;
			else carry = 0;
		}
		if (carry == 1) resize(size_ << 1), arr[size_ >> 1] = 1;
		return *this;
	}
	bigint& operator-=(const bigint& x) {
		// Time complexity: linear
		// Assertion: this >= x. Runtime error's possible if this < x
		int carry = 0, mx = 1;
		for (int i = 0; i < size_; ++i) {
			if ((arr[i] -= (i < x.size_ ? x.arr[i] : 0) + carry) < 0) arr[i] += maxvalue, carry = 1;
			while (arr[i] != 0 && mx < i + 1) mx *= 2;
		}
		if (mx != size_) resize(mx);
		return *this;
	}
	bigint& operator*=(const bigint& x) {
		// Time complexity: linearithmic
		// Assertion: mod1 <= mod2
		int mod1 = 167772161, root1 = 3;
		int mod2 = 469762049, root2 = 3;
		int magic = binpow(mod1, mod2 - 2, mod2);
		resize(std::max(size_, x.size_) * 2);
		std::vector<int> a1 = arr; fourier_transform(size_, a1, mod1, root1, false);
		std::vector<int> a2 = arr; fourier_transform(size_, a2, mod2, root2, false);
		std::vector<int> x1 = x.arr; x1.resize(size_); fourier_transform(size_, x1, mod1, root1, false);
		std::vector<int> x2 = x.arr; x2.resize(size_); fourier_transform(size_, x2, mod2, root2, false);
		for (int i = 0; i < size_; i++) {
			a1[i] = 1LL * a1[i] * x1[i] % mod1;
			a2[i] = 1LL * a2[i] * x2[i] % mod2;
		}
		fourier_transform(size_, a1, mod1, root1, true);
		fourier_transform(size_, a2, mod2, root2, true);
		long long carry = 0; int mx = 1;
		for (int i = 0; i < size_; i++) {
			long long val = 1LL * (a2[i] - a1[i] + mod2) * magic % mod2 * mod1 + a1[i];
			arr[i] = (val + carry) % maxvalue;
			carry = (val + carry) / maxvalue;
			while (arr[i] != 0 && mx < i + 1) mx *= 2;
		}
		if (mx != size_) resize(mx);
		return *this;
	}
	friend bool operator<(const bigint& x1, const bigint& x2) {
		if (x1.size_ != x2.size_) return x1.size_ < x2.size_;
		for (int i = x1.size_ - 1; i >= 0; --i) {
			if (x1.arr[i] != x2.arr[i]) return x1.arr[i] < x2.arr[i];
		}
		return false;
	}
	friend bool operator>(const bigint& x1, const bigint& x2) {
		return x2 < x1;
	}
	friend bool operator<=(const bigint& x1, const bigint& x2) {
		return !(x1 > x2);
	}
	friend bool operator>=(const bigint& x1, const bigint& x2) {
		return !(x1 < x2);
	}
	friend bool operator==(const bigint& x1, const bigint& x2) {
		return !(x1 < x2 || x1 > x2);
	}
	friend bool operator!=(const bigint& x1, const bigint& x2) {
		return x1 < x2 || x1 > x2;
	}
	friend bigint operator+(const bigint& x1, const bigint& x2) {
		bigint res = x1;
		res += x2;
		return res;
	}
	friend bigint operator-(const bigint& x1, const bigint& x2) {
		bigint res = x1;
		return res -= x2;
	}
	friend bigint operator*(const bigint& x1, const bigint& x2) {
		bigint res = x1;
		res *= x2;
		return res;
	}
	friend std::istream& operator >> (std::istream& is, bigint& x) {
		std::string s;
		is >> s;
		x = bigint(s);
		return is;
	}
	friend std::ostream& operator<<(std::ostream& os, const bigint& x) {
		os << x.to_string();
		return os;
	}
	int binpow(int a, int b, int mod) {
		int ret = 1;
		while (b) {
			if (b & 1) ret = 1LL * ret * a % mod;
			a = 1LL * a * a % mod;
			b >>= 1;
		}
		return ret;
	}
	void fourier_transform(int sz, std::vector<int>& f, int mod, int primitive_root, bool inverse) {
		// O(n log n) Number Theoretic Transform
		for (int i = 0, j = 1; j < sz - 1; j++) {
			for (int k = sz >> 1; k >(i ^= k); k >>= 1);
			if (i < j) std::swap(f[i], f[j]);
		}
		int root = binpow(primitive_root, (mod - 1) / sz, mod);
		std::vector<int> pw(sz + 1); pw[0] = 1;
		for (int i = 1; i <= sz; i++) pw[i] = 1LL * pw[i - 1] * root % mod;
		for (int b = 1; b < sz; b <<= 1) {
			int qs = sz / (b * 2);
			for (int i = 0; i < sz; i += b * 2) {
				for (int j = i; j < i + b; j++) {
					int f0 = f[j], f1 = 1LL * pw[(inverse ? b * 2 - j + i : j - i) * qs] * f[j + b] % mod;
					f[j] = f0 + f1; if (f[j] >= mod) f[j] -= mod;
					f[j + b] = f0 - f1; if (f[j + b] < 0) f[j + b] += mod;
				}
			}
		}
		if (inverse) {
			int mul = binpow(sz, mod - 2, mod);
			for (int i = 0; i < sz; i++) {
				f[i] = 1LL * f[i] * mul % mod;
			}
		}
	}
};

#endif