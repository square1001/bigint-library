#ifndef CLASS_BIGINT
#define CLASS_BIGINT

#include <string>
#include <iostream>
#include "basic-int.h"

// (digit / base_digit) * base_number^2 <= 1811939329 * 2013265921 = 3647915701995307009
// (digit / base_digit) < min(2^27, 2^26) = 67108864
// ---> max_digit = min(1823957850, 335544320) = 335544320

constexpr std::size_t base_digit = 5;
constexpr std::uint32_t base_number = 100000;

class bigint : private basic_int<base_number> {
private:
	bool sign; // positive/zero = false, negative = true
	bool is_number(std::string str) {
		if(str.empty() || str == "-" || !(('0' <= str[0] && str[0] <= '9') || str[0] == '-')) {
			return false;
		}
		for(std::uint64_t i = 1; i < str.size(); ++i) {
			if(!('0' <= str[i] && str[i] <= '9')) {
				return false;
			}
		}
		return true;
	}
	std::vector<std::uint32_t> to_basic_expression(std::uint64_t x) {
		std::vector<std::uint32_t> res;
		do {
			res.push_back(x % base_number);
			x /= base_number;
		} while(x != 0);
		return res;
	}
public:
	// ----- #1. Constructors / String Functions ----- //
	bigint() : basic_int(), sign(false) {};
	bigint(std::int64_t x) : basic_int(to_basic_expression(x >= 0 ? x : -x)), sign(x < 0) {};
	bigint(const std::string& str) {
		assert(is_number(str));
		sign = (str[0] == '-');
		std::size_t initial = (sign ? 1 : 0);
		while(initial + 1 != str.size() && str[initial] == '0') {
			++initial;
		}
		std::size_t str_digits = str.size() - initial;
		capacity = 1;
		while(capacity * base_digit < str_digits) {
			capacity *= 2;
		}
		digit = std::vector<std::uint32_t>(capacity, 0);
		for(std::size_t i = 0; (i + 1) * base_digit < str_digits; ++i) {
			digit[i] = std::stoi(str.substr(str.size() - i * base_digit - base_digit, base_digit));
		}
		digit[(str_digits - 1) / base_digit] = std::stoi(str.substr(initial, (str.size() - (str_digits - 1) / base_digit * base_digit) - initial));
	}
	std::string to_string() const {
		std::size_t d = digits();
		std::string res = (sign ? "-" : "") + std::to_string(digit[d - 1]);
		for(std::size_t i = d - 2; i != std::size_t(-1); --i) {
			std::string digit_str = std::to_string(digit[i]);
			res += std::string(base_digit - digit_str.size(), '0') + digit_str;
		}
		return res;
	}

	// ----- #2. Operators ----- //
	bool operator==(const bigint& b) const noexcept { return sign == b.sign && basic_int::operator==(b); }
	bool operator!=(const bigint& b) const noexcept { return sign != b.sign || basic_int::operator!=(b); }
	bool operator<(const bigint& b) const noexcept { return sign != b.sign ? sign : basic_int::operator<(b); }
	bool operator>(const bigint& b) const noexcept { return sign != b.sign ? !sign : basic_int::operator>(b); }
	bool operator<=(const bigint& b) const noexcept { return sign != b.sign ? sign : basic_int::operator<=(b); }
	bool operator>=(const bigint& b) const noexcept { return sign != b.sign ? !sign : basic_int::operator>=(b); }
	bigint& operator+=(const bigint& b) {
		if(sign == b.sign) {
			basic_int::operator+=(b);
		}
		else if(basic_int(*this) >= basic_int(b)) {
			basic_int::operator-=(b);
			if(sign && basic_int(*this) == basic_int()) sign = false;
		}
		else {
			sign = !sign;
			(*this) = b - (*this);
		}
		return *this;
	}
	bigint& operator-=(const bigint& b) {
		if(sign != b.sign) {
			basic_int::operator+=(b);
		}
		else if(basic_int(*this) >= basic_int(b)) {
			basic_int::operator-=(b);
			if(sign && basic_int(*this) == basic_int()) sign = false;
		}
		else {
			sign = !sign;
			(*this) += b;
			sign = !sign;
		}
		return *this;
	}
	bigint& operator*=(const bigint& b) {
		basic_int::operator*=(b);
		if(b.sign) sign = !sign;
		if(sign && basic_int(*this) == basic_int()) sign = false;
		return *this;
	}
	bigint& operator/=(const bigint& b) {
		basic_int::operator/=(b);
		if(b.sign) sign = !sign;
		if(sign && basic_int(*this) == basic_int()) sign = false;
		return *this;
	}
	bigint operator%=(const bigint& b) {
		bigint itself(*this);
		(*this) = itself - itself / b * b;
		return *this;
	}
	bigint operator+(const bigint& b) const { return bigint(*this) += b; }
	bigint operator-(const bigint& b) const { return bigint(*this) -= b; }
	bigint operator*(const bigint& b) const { return bigint(*this) *= b; }
	bigint operator/(const bigint& b) const { return bigint(*this) /= b; }
	bigint operator%(const bigint& b) const { return bigint(*this) %= b; }
	friend std::istream& operator>>(std::istream& is, bigint& x) { std::string s; is >> s; x = bigint(s); return is; }
	friend std::ostream& operator<<(std::ostream& os, const bigint& x) { os << x.to_string(); return os; }
};

#endif // CLASS_BIGINT