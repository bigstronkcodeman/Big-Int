#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <sstream>

//#include "Timer.hpp"

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ulonglong;
typedef long long _long;

const size_t UCHAR_BYTES = sizeof(unsigned char);
const size_t UINT_BYTES = sizeof(uint);
const size_t ULONGLONG_BYTES = sizeof(ulonglong);
const size_t BITS_IN_NIBBLE = 4;
const size_t BITS_IN_BYTE = 8;
const size_t BITS_IN_UINT = UINT_BYTES * BITS_IN_BYTE;
const size_t UCHARS_IN_UINT = UINT_BYTES / UCHAR_BYTES;
const uint KARATSUBA_CUTOFF = 50;

const uint BIGGEST_POW10 = (uint)1000000000; // max power of 10 we can store in 32 bits
const uint POW10_DIGITS = log10(BIGGEST_POW10);

class BigInt {
private:
	std::vector<uint> digits;
	bool positive;
	static const uint _UINT_MAX = std::numeric_limits<uint>::max();
	static const ulonglong BASE = _UINT_MAX + (ulonglong)1;

	BigInt(const std::vector<uint>& digits_in, const bool positive_in);

	template<typename T> void init(T _num);
	static BigInt add_like_signs(const BigInt& a, const BigInt& b);
	static BigInt add_diff_signs(const BigInt& a, const BigInt& b);
	static BigInt long_mult(const BigInt& a, const BigInt& b);
	static BigInt karatsuba_mult(const BigInt& a, const BigInt& b); // todo
	static BigInt schonhage_strassen_mult(const BigInt& a, const BigInt& b); // todo
	static BigInt long_div(const BigInt& a, const BigInt& b);

public:
	BigInt();
	BigInt(const BigInt& other);
	BigInt(BigInt&& other) noexcept;
	BigInt(int num);
	BigInt(long num);
	BigInt(long long num);
	BigInt(float num);
	BigInt(double num);

	std::vector<uint> get_digits() const;
	static std::vector<bool> get_bits(uint num, bool pad_32 = false);
	std::vector<bool> get_bits() const;
	std::string to_string() const;
	std::string to_string2() const;
	std::string to_binary_string() const;
	size_t num_digits() const;
	
	static BigInt pow(const BigInt& a, const BigInt& b);

	// assignment operator overloads
	BigInt& operator= (const BigInt& right);
	BigInt& operator= (BigInt&& right) noexcept;
	BigInt& operator+= (const BigInt& right);
	BigInt& operator-= (const BigInt& right); // todo
	BigInt& operator*= (const BigInt& right); // todo
	BigInt& operator/= (const BigInt& right); // todo

	// comparison operator overloads
	bool operator== (const BigInt& right) const;
	bool operator< (const BigInt& right) const;
	bool operator> (const BigInt& right) const;
	bool operator<= (const BigInt& right) const;
	bool operator>= (const BigInt& right) const;

	// arithmetic operator overloads
	BigInt operator+ (const BigInt& right) const;
	BigInt operator- (const BigInt& right) const;
	BigInt operator* (const BigInt& right) const;
	BigInt operator/ (const BigInt& right) const; // todo
};