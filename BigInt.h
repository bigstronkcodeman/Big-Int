#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

#include "Timer.h"

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ulong;
typedef long long _long;

const size_t UCHAR_BYTES = sizeof(unsigned char);
const size_t UINT_BYTES = sizeof(uint);
const size_t ULONG_BYTES = sizeof(ulong);
const size_t BITS_IN_NIBBLE = 4;
const size_t BITS_IN_BYTE = 8;
const size_t BITS_IN_UINT = UINT_BYTES * BITS_IN_BYTE;
const size_t UCHARS_IN_UINT = UINT_BYTES / UCHAR_BYTES;

enum class MultType {
	LONG,
	KARATSUBA,
	SCHONHAGE_STRASSEN
};


class BigInt {
private:
	static const uint _UINT_MAX = std::numeric_limits<uint>::max();
	static const ulong BASE = _UINT_MAX + (ulong)1;
	static const MultType MULT_MODE = MultType::LONG;
	std::vector<uint> digits;
	bool positive;

	BigInt(const std::vector<uint>& digits_in, const bool positive_in);

	template<typename T> void init(T _num);
	static BigInt add_like_signs(const BigInt& a, const BigInt& b);
	static BigInt add_diff_signs(const BigInt& a, const BigInt& b);
	static BigInt long_mult(const BigInt& a, const BigInt& b);
	static BigInt karatsuba_mult(const BigInt& a, const BigInt& b);
	static BigInt schonhage_strassen_mult(const BigInt& a, const BigInt& b);

public:
	BigInt();
	BigInt(const BigInt& other);
	BigInt(BigInt&& other) noexcept;
	BigInt(int num);
	BigInt(long long num);
	BigInt(float num);
	BigInt(double num);

	std::vector<uint> get_digits() const;
	static std::vector<bool> get_bits(uint num, bool pad_32 = false);
	std::vector<bool> get_bits() const;
	std::string to_string() const; // wip
	std::string to_binary_string() const;
	size_t num_digits() const;

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