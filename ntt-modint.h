#ifndef CLASS_MODINT
#define CLASS_MODINT

#include <cassert>
#include <cstdint>

template <std::uint32_t mod>
class modint {
private:
	std::uint32_t n;
public:
	modint() noexcept : n(0) {};
	modint(std::int64_t n_) noexcept : n((n_ >= 0 ? n_ : mod - (-n_) % mod) % mod) {};
	static constexpr std::uint32_t get_mod() noexcept { return mod; }
	std::uint32_t get() const noexcept { return n; }
	bool operator==(const modint& m) const noexcept { return n == m.n; }
	bool operator!=(const modint& m) const noexcept { return n != m.n; }
	modint& operator+=(const modint& m) noexcept { n += m.n; n = (n < mod ? n : n - mod); return *this; }
	modint& operator-=(const modint& m) noexcept { n += mod - m.n; n = (n < mod ? n : n - mod); return *this; }
	modint& operator*=(const modint& m) noexcept { n = std::uint64_t(n) * m.n % mod; return *this; }
	modint operator+(const modint& m) const noexcept { return modint(*this) += m; }
	modint operator-(const modint& m) const noexcept { return modint(*this) -= m; }
	modint operator*(const modint& m) const noexcept { return modint(*this) *= m; }
	modint inv() const {
		assert(n != 0);
		return pow(mod - 2);
	}
	modint pow(std::uint64_t b) const noexcept {
		modint ans = 1, m = modint(*this);
		while (b) {
			if (b & 1) ans *= m;
			m *= m;
			b >>= 1;
		}
		return ans;
	}
};

#endif // CLASS_MODINT