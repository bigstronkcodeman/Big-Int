#include "BigInt.h"

/* ***************************************************
   *              BIGINT CLASS METHODS               *
   ***************************************************  */

BigInt::BigInt()
	: digits(std::vector<uint>(1, 0))
	, positive(true)
{
	digits.reserve(8);
}

BigInt::BigInt(const BigInt& other)
	: digits(other.digits)
	, positive(other.positive)
{  }

BigInt::BigInt(BigInt&& other) noexcept
	: digits(std::move(other.digits))
	, positive(std::exchange(other.positive, 0))
{  }

BigInt::BigInt(int num) { init<int>(num); }

BigInt::BigInt(long long num) { init<long long>(num); }

BigInt::BigInt(float num) { init<int>(static_cast<int>(num)); }

BigInt::BigInt(double num) { init<double>(num); }

BigInt::BigInt(const std::vector<uint>& digits_in, const bool is_positive)
	: digits(digits_in)
	, positive(is_positive)
{  }

template <typename T> void BigInt::init(T _num) {
	static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type :/");
	positive = (_num >= 0) ? true : false;
	_long num = static_cast<_long>(abs(_num));
	int num_digits = num == 0 ? 1 : static_cast<int>(log10(num));

	digits.reserve(num_digits);
	for (_long n = num; n > 0; n /= BASE) {
		digits.push_back(n % BASE);
	}
}

std::string BigInt::to_string() const {
	std::string return_str = (this->positive) ? "" : "-1 * (";
	for (_long i = digits.size() - 1; i >= 0; --i) {
		return_str += std::to_string(digits[(uint)i]) + " * " + std::to_string(BASE) + "^" + std::to_string(i);
		if (i > 0) {
			return_str += " + ";
		}
	}

	if (!this->positive) {
		return_str += ")";
	}

	return return_str;
}

std::string BigInt::to_binary_string() const {
	return Bitset(*this).to_string();
}

std::vector<uint> BigInt::get_digits() const {
	return this->digits;
}

std::vector<bool> BigInt::get_bits(uint num, bool pad_32) {
	std::vector<bool> res_bits;
	res_bits.reserve(32);

	uint mask = 1;
	while (num > 0) {
		res_bits.push_back(num & mask);
		num >>= 1;
	}

	if (pad_32) {
		for (size_t i = res_bits.size(); i < 32; ++i) {
			res_bits.push_back(0);
		}
	}

	return res_bits;
}

std::vector<bool> BigInt::get_bits() const {
	std::vector<bool> res_bits;
	size_t my_size = this->num_digits();
	res_bits.reserve((ulong)32 * my_size);

	for (size_t i = 0; i < my_size; ++i) {
		std::vector<bool> bit_block_32 = get_bits(this->digits[i], (i < my_size - 1));
		res_bits.insert(res_bits.end(), bit_block_32.begin(), bit_block_32.end());
	}

	return res_bits;
}

size_t BigInt::num_digits() const {
	return this->digits.size();
}

BigInt BigInt::add_like_signs(const BigInt& a, const BigInt& b) {
	std::vector<uint> result_digits;
	size_t a_num_digits = a.num_digits();
	size_t b_num_digits = b.num_digits();
	result_digits.reserve(std::max(a_num_digits, b_num_digits) + 16);

	size_t i = 0;
	size_t j = 0;
	ulong overflow = 0;
	while (i < a_num_digits && j < b_num_digits) {
		ulong true_add = (ulong)a.digits[i++] + b.digits[j++] + overflow;
		uint new_digit = (uint)true_add;
		overflow = true_add >> 32;
		result_digits.push_back(new_digit);
	}

	while (i < a_num_digits) {
		ulong true_add = (ulong)a.digits[i++] + overflow;
		uint new_digit = (uint)true_add;
		overflow = true_add >> 32;
		result_digits.push_back(new_digit);
	}

	while (j < b_num_digits) {
		ulong true_add = (ulong)b.digits[j++] + overflow;
		uint new_digit = (uint)true_add;
		overflow = true_add >> 32;
		result_digits.push_back(new_digit);
	}

	if (overflow) {
		result_digits.push_back((uint)overflow);
	}

	return BigInt(result_digits, a.positive);
}

BigInt BigInt::add_diff_signs(const BigInt& big, const BigInt& small) {
	std::vector<uint> result_digits;
	size_t big_num_digits = big.num_digits();
	size_t small_num_digits = small.num_digits();
	result_digits.reserve(small_num_digits + 1);

	size_t i = 0;
	size_t j = 0;
	uint underflow = 0;
	while (i < big_num_digits && j < small_num_digits) {
		uint new_digit = big.digits[i] - small.digits[j] - underflow;
		underflow = (big.digits[i++] < small.digits[j++]) ? 1 : 0;
		result_digits.push_back(new_digit);
	}

	while (i < big_num_digits) {
		uint new_digit = big.digits[i++] - underflow;
		underflow = 0;
		result_digits.push_back(new_digit);
	}

	while (j < small_num_digits) {
		uint new_digit = small.digits[j++] - underflow;
		underflow = 0;
		result_digits.push_back(new_digit);
	}

	for (i = result_digits.size() - 1; i > 0; --i) {
		if (result_digits[i] != 0) {
			result_digits.resize(i + 1);
			break;
		}
	}

	return BigInt(result_digits, big.positive);
}

BigInt BigInt::long_mult(const BigInt& a, const BigInt& b) {
	bool sign = !(a.positive ^ b.positive);
	const BigInt* longer = &a;
	const BigInt* shorter = &b;
	size_t longer_size = longer->num_digits();
	size_t shorter_size = shorter->num_digits();
	if (shorter_size > longer_size) {
		std::swap(longer, shorter);
		std::swap(longer_size, shorter_size);
	}

	BigInt result;
	size_t pad_zeros = 0;
	for (size_t i = 0; i < shorter_size; ++i) {
		uint overflow = 0;
		std::vector<uint> add_digits(pad_zeros);
		add_digits.reserve(longer_size + 1 + pad_zeros);

		for (size_t j = 0; j < longer_size; ++j) {
			ulong true_mult = ((ulong)shorter->digits[i] * longer->digits[j]) + overflow;
			uint new_digit = (uint)true_mult;
			overflow = true_mult >> 32;
			add_digits.push_back(new_digit);
		}

		if (overflow) {
			add_digits.push_back(overflow);
		}
		result += BigInt(add_digits, sign);
		++pad_zeros;
	}
	return result;
}

// todo
BigInt BigInt::karatsuba_mult(const BigInt& a, const BigInt& b) {
	return BigInt();
}

// todo
BigInt BigInt::schonhage_strassen_mult(const BigInt& a, const BigInt& b) {
	return BigInt();
}

BigInt BigInt::operator* (const BigInt& right) const {
	switch (this->MULT_MODE) {
	case MultType::LONG:
		return long_mult(*this, right);
	case MultType::KARATSUBA:
		return karatsuba_mult(*this, right);
	case MultType::SCHONHAGE_STRASSEN:
		return schonhage_strassen_mult(*this, right);
	default:
		std::cout << "what the heck\n";
		exit(1);
	}
}

BigInt& BigInt::operator= (const BigInt& right) {
	digits = std::vector<uint>(right.digits);
	positive = right.positive;
	return *this;
}

BigInt& BigInt::operator= (BigInt&& right) noexcept {
	digits = std::move(right.digits);
	positive = std::exchange(right.positive, 0);
	return *this;
}

bool BigInt::operator== (const BigInt& right) const {
	size_t my_size = this->num_digits();
	if (this->positive == right.positive) {
		if (my_size == right.num_digits()) {
			for (size_t i = 0; i < my_size; ++i) {
				if (digits[i] != right.digits[i]) {
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

bool BigInt::operator< (const BigInt& right) const {
	size_t my_size = this->num_digits();
	size_t their_size = right.num_digits();
	if (this->positive != right.positive) {
		if (this->positive) {
			return false;
		}
		return true;
	}
	if (my_size < their_size) {
		return true;
	}
	if (my_size > their_size) {
		return false;
	}

	size_t i = my_size - 1;
	size_t j = their_size - 1;
	while (i > 0 && j > 0) {
		if (digits[i] < right.digits[j]) {
			return true;
		}
		--i; --j;
	}
	return digits[i] < right.digits[j];
}

bool BigInt::operator> (const BigInt& right) const {
	if (!(*this == right) && !(*this < right)) {
		return true;
	}
	return false;
}

bool BigInt::operator<= (const BigInt& right) const {
	if (!(*this > right)) {
		return true;
	}
	return false;
}

bool BigInt::operator>= (const BigInt& right) const {
	if (!(*this < right)) {
		return true;
	}
	return false;
}

BigInt BigInt::operator+ (const BigInt& right) const {
	if (this->positive == right.positive) {
		return add_like_signs(*this, right);
	}

	BigInt me(*this);
	bool positive_save = me.positive;
	me.positive = right.positive;
	if (me < right) {
		me.positive = positive_save;
		return add_diff_signs(right, me);
	}
	me.positive = positive_save;
	return add_diff_signs(me, right);
}

BigInt BigInt::operator- (const BigInt& right) const {
	BigInt opp_right = BigInt(right.digits, !right.positive);
	return *this + opp_right;
}

BigInt& BigInt::operator+= (const BigInt& right) {
	*this = *this + right;
	return *this;
}







/* *************************************************** 
   *              BITSET CLASS METHODS               * 
   ***************************************************  */

Bitset::Bitset()
	: bytes(std::vector<byte>(1, 0))
{
	bytes.reserve(BS_DEFAULT_BYTES);
}

Bitset::Bitset(const size_t size)
	: bytes(std::vector<byte>(1, 0))
{
	bytes.reserve(size);
}

Bitset::Bitset(const Bitset& other)
	: bytes(other.bytes)
{  }

Bitset::Bitset(Bitset&& other) noexcept
	: bytes(std::move(other.bytes))
{  }

Bitset::Bitset(const BigInt& bi) {
	const size_t bi_num_digits = bi.num_digits();
	bytes = std::vector<byte>(bi_num_digits * 4);

	std::vector<uint> bi_digits = bi.get_digits();
	for (size_t i = 0; i < bi_num_digits; ++i) {
		for (size_t j = 0; j < 4; ++j) {
			bytes[(i * 4) + j] = Bitset::get_byte(bi_digits[i], j);
		}
	}

	for (size_t i = bytes.size() - 1; i > 0; --i) {
		if (bytes[i] != 0) {
			break;
		}
		bytes.pop_back();
	}
}

byte Bitset::get_byte(uint num, const uint which) {
	if (which < 4) {
		uint shift = 8 * which;
		uint mask = 0xFF << shift;
		num &= mask;
		return (num >> shift);
	}

	std::cout << "Error: requested byte #" << which + 1
		<< " of " << num << ", which does not exist\n";
	return -1;
}

size_t Bitset::num_bytes() const {
	return bytes.size();
}

std::string Bitset::to_string() const {
	std::string bit_str = "";
	uint mask = 1;
	for (size_t i = 0; i < this->num_bytes(); ++i) {
		byte byte_copy = bytes[i];
		for (size_t j = 0; j < 8; ++j) {
			if (byte_copy & mask) {
				bit_str.insert(0, 1, '1');
			}
			else {
				bit_str.insert(0, 1, '0');
			}
			byte_copy >>= 1;
		}
	}

	size_t zero_count = 0;
	for (zero_count; zero_count < bit_str.length(); ++zero_count) {
		if (bit_str[zero_count] != '0') {
			break;
		}
	}
	bit_str.erase(0, zero_count);

	return bit_str;
}