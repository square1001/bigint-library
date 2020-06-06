#ifndef CLASS_BASICINT
#define CLASS_BASICINT

#include <vector>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include "ntt-implementation.h"

template<std::uint32_t base>
class basic_int {
	// 2 <= base < max(base_type) / 2 = 2^30
protected:
	using base_type = std::uint32_t; // base type
	using base_type_d = std::uint64_t; // double of base type
	std::size_t capacity;
	std::vector<base_type> digit;
	void resize(std::size_t capacity_) {
		if(capacity != capacity_) {
			assert(capacity_ >= 1);
			capacity = capacity_;
			digit.resize(capacity_);
		}
	}
	void minimize() {
		std::size_t d = digits();
		while(d & (d - 1)) {
			d += d & (-d);
		}
		resize(d);
	}
	std::size_t digits() const {
		for(std::size_t i = capacity - 1; i >= 1; --i) {
			if(digit[i] != 0) {
				return i + 1;
			}
		}
		return 1;
	}
public:
	// ----- #1. Costructors ----- //
	explicit basic_int() : capacity(1), digit(1, base_type(0)) {};
	explicit basic_int(std::vector<base_type> digit_) : digit(digit_) {
		assert(!digit.empty());
		for(base_type i : digit) {
			assert(0 <= i && i < base);
		}
		capacity = 1;
		while(capacity < digit.size()) {
			capacity <<= 1;
		}
		digit.resize(capacity, 0);
	};

	// ----- #2. Comparing Operators ----- //
	bool operator==(const basic_int &b) const noexcept {
		return digit == b.digit;
	}
	bool operator!=(const basic_int &b) const noexcept {
		return digit != b.digit;
	}
	bool operator<(const basic_int &b) const noexcept {
		if(capacity != b.capacity) {
			return capacity < b.capacity;
		}
		for(std::size_t i = capacity - 1; i >= 1; --i) {
			if(digit[i] != b.digit[i]) {
				return digit[i] < b.digit[i];
			}
		}
		return digit[0] < b.digit[0];
	}
	bool operator>(const basic_int &b) const noexcept {
		if(capacity != b.capacity) {
			return capacity > b.capacity;
		}
		for(std::size_t i = capacity - 1; i >= 1; --i) {
			if(digit[i] != b.digit[i]) {
				return digit[i] > b.digit[i];
			}
		}
		return digit[0] > b.digit[0];
	}
	bool operator<=(const basic_int &b) const noexcept {
		return basic_int(*this) < b || basic_int(*this) == b;
	}
	bool operator>=(const basic_int &b) const noexcept {
		return basic_int(*this) > b || basic_int(*this) == b;
	}

	// ----- #3. Shifting ----- //
	basic_int& operator<<=(std::size_t s) noexcept {
		std::size_t d = digits();
		if(d + s > capacity) {
			std::size_t nxtcap = d + s;
			while(nxtcap & (-nxtcap)) {
				nxtcap += nxtcap & (-nxtcap);
			}
			resize(nxtcap);
		}
		for(std::size_t i = d; i >= s; --i) {
			digit[i] = digit[i - s];
		}
		std::fill(digit.begin(), digit.begin() + s, 0);
		return *this;
	}
	basic_int& operator>>=(std::size_t s) noexcept {
		std::size_t d = digits();
		if(d <= s) {
			(*this) = basic_int();
		}
		else {
			for(std::size_t i = 0; i < d - s; ++i) {
				digit[i] = digit[i + s];
			}
			std::fill(digit.begin() + d - s, digit.begin() + d, 0);
			minimize();
		}
		return *this;
	}
	
	// ----- #4. Addition ----- //
	basic_int& operator+=(const basic_int &b) {
		resize(std::max(capacity, b.capacity));
		bool carry = false;
		for(std::uint32_t i = 0; i < b.capacity; ++i) {
			digit[i] += b.digit[i] + (carry ? 1 : 0);
			carry = (digit[i] >= base);
			if(carry) digit[i] -= base;
		}
		for(std::uint32_t i = b.capacity; i < capacity; ++i) {
			digit[i] += (carry ? 1 : 0);
			carry = (digit[i] >= base);
			if(carry) digit[i] -= base;
		}
		if(carry) {
			resize(capacity << 1);
			digit[capacity >> 1] = 1;
		}
		return *this;
	}
	
	// ----- #5. Subtraction ----- //
	basic_int& operator-=(const basic_int &b) {
		bool carry = false;
		for(std::uint32_t i = 0; i < b.capacity; ++i) {
			digit[i] += base - b.digit[i] - (carry ? 1 : 0);
			carry = (digit[i] < base);
			if(!carry) digit[i] -= base;
		}
		for(std::uint32_t i = b.capacity; i < capacity; ++i) {
			digit[i] += base - (carry ? 1 : 0);
			carry = (digit[i] < base);
			if(!carry) digit[i] -= base;
		}
		// assert(!carry) // noexcept
		minimize();
		return *this;
	}

	// ----- #6. Multiplication ----- //
	basic_int& operator*=(const basic_int &b) {
		using ntt1 = polynomial_ntt<469762049, 26, 3>;
		using ntt2 = polynomial_ntt<167772161, 25, 3>;
		const ntt2::modulo magic_mult = ntt2::modulo(ntt1::modulo::get_mod()).inv();
		std::size_t nxtcap = std::max(capacity, b.capacity) * 2;
		std::vector<ntt1::modulo> nxtdigit1 = ntt1::convolve(std::vector<ntt1::modulo>(digit.begin(), digit.end()), std::vector<ntt1::modulo>(b.digit.begin(), b.digit.end())); nxtdigit1.resize(nxtcap);
		std::vector<ntt2::modulo> nxtdigit2 = ntt2::convolve(std::vector<ntt2::modulo>(digit.begin(), digit.end()), std::vector<ntt2::modulo>(b.digit.begin(), b.digit.end())); nxtdigit2.resize(nxtcap);
		capacity = nxtcap;
		digit = std::vector<std::uint32_t>(capacity, 0);
		std::uint64_t carry = 0;
		for(std::uint32_t i = 0; i < capacity; ++i) {
			std::uint64_t nxtdigit = std::uint64_t(ntt1::modulo::get_mod()) * ((nxtdigit2[i] - nxtdigit1[i].get()) * magic_mult).get() + nxtdigit1[i].get();
			nxtdigit += carry;
			carry = nxtdigit / base;
			digit[i] = nxtdigit - carry * base;
		}
		minimize();
		return *this;
	}
};

#endif // CLASS_BASICINT