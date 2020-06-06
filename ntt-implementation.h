#ifndef CLASS_POLYNOMIAL_NTT
#define CLASS_POLYNOMIAL_NTT

#include <vector>
#include <algorithm>
#include "ntt-modint.h"

template<std::uint32_t mod, std::uint32_t depth, std::uint32_t primroot>
class polynomial_ntt {
public:
	using modulo = modint<mod>;
	static void fourier_transform(std::vector<modulo> &v, bool inverse) {
		std::size_t s = v.size();
		for (std::size_t i = 0, j = 1; j < s - 1; ++j) {
			for (std::size_t k = s >> 1; k > (i ^= k); k >>= 1);
			if (i < j) std::swap(v[i], v[j]);
		}
		std::size_t sc = 0, sz = 1;
		while (sz < s) sz *= 2, ++sc;
		modulo root = modulo(primroot).pow((mod - 1) >> sc);
		std::vector<modulo> pw(s + 1); pw[0] = 1;
		for (std::size_t i = 1; i <= s; i++) pw[i] = pw[i - 1] * root;
		std::size_t qs = s;
		for (std::size_t b = 1; b < s; b <<= 1) {
			qs >>= 1;
			for (std::size_t i = 0; i < s; i += b * 2) {
				for (std::size_t j = i; j < i + b; ++j) {
					modulo delta = pw[(inverse ? b * 2 - j + i : j - i) * qs] * v[j + b];
					v[j + b] = v[j] - delta;
					v[j] += delta;
				}
			}
		}
		if (!inverse) return;
		modulo powinv = modulo((mod + 1) / 2).pow(sc);
		for (std::size_t i = 0; i < s; ++i) {
			v[i] = v[i] * powinv;
		}
	}
	static std::vector<modulo> convolve(std::vector<modulo> v1, std::vector<modulo> v2) {
		if(v1.empty() || v2.empty()) {
			return std::vector<modulo>();
		}
		if(v1.size() < v2.size()) {
			swap(v1, v2);
		}
		std::size_t fsize = v1.size() + v2.size() - 1;
		std::size_t step = v2.size();
		while(step & (step - 1)) {
			step += step & (-step); // step to become minimum 2^k >= v2.size()
		}
		v1.resize((v1.size() + step - 1) / step * step); // change v1.size() to multiple of step
		v2.resize(step * 2);
		std::vector<modulo> ans(fsize);
		std::vector<modulo> pv1(step * 2);
		fourier_transform(v2, false);
		for(std::size_t i = 0; i < v1.size(); i += step) {
			std::copy(v1.begin() + i, v1.begin() + i + step, pv1.begin());
			std::fill(pv1.begin() + step, pv1.end(), modulo(0));
			fourier_transform(pv1, false);
			for(std::size_t j = 0; j < step * 2; ++j) {
				pv1[j] *= v2[j];
			}
			fourier_transform(pv1, true);
			for(std::size_t j = 0; j < step * 2 && i + j < fsize; ++j) {
				ans[j + i] += pv1[j];
			}
		}
		return ans;
	}
};

#endif // CLASS_POLYNOMIAL_NTT