#ifndef CLASS_BIGINT
#define CLASS_BIGINT

#include <string>
#include <iostream>
#include "basic-int.h"

constexpr std::size_t base_digit = 1;
constexpr std::uint32_t base_number = 10;

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
		while(capacity < str_digits) {
			capacity *= 2;
		}
		digit = std::vector<std::uint32_t>(capacity, 0);
		for(std::size_t i = 0; i < str_digits - base_digit; i += base_digit) {
			digit[i] = std::stoi(str.substr(str.size() - i - base_digit, base_digit));
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
	bool operator==(const bigint& b) const noexcept { return sign == b.sign && basic_int<base_number>::operator==(b); }
	bool operator!=(const bigint& b) const noexcept { return sign != b.sign || basic_int<base_number>::operator!=(b); }
	bool operator<(const bigint& b) const noexcept { return sign != b.sign ? sign : basic_int<base_number>::operator<(b); }
	bool operator>(const bigint& b) const noexcept { return sign != b.sign ? !sign : basic_int<base_number>::operator>(b); }
	bool operator<=(const bigint& b) const noexcept { return sign != b.sign ? sign : basic_int<base_number>::operator<=(b); }
	bool operator>=(const bigint& b) const noexcept { return sign != b.sign ? !sign : basic_int<base_number>::operator>=(b); }
	bigint& operator+=(const bigint& b) { basic_int<base_number>::operator+=(b); return *this; }
	bigint& operator-=(const bigint& b) { basic_int<base_number>::operator-=(b); return *this; }
	bigint& operator*=(const bigint& b) { basic_int<base_number>::operator*=(b); return *this; }
	bigint operator+(const bigint& b) const { return bigint(*this) += b; }
	bigint operator-(const bigint& b) const { return bigint(*this) -= b; }
	bigint operator*(const bigint& b) const { return bigint(*this) *= b; }
	friend std::istream& operator>>(std::istream& is, bigint& x) { std::string s; is >> s; x = bigint(s); return is; }
	friend std::ostream& operator<<(std::ostream& os, const bigint& x) { os << x.to_string(); return os; }
};

#endif // CLASS_BIGINT