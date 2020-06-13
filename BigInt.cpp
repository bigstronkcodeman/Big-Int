#include "BigInt.h"

/* ***************************************************
   *              BIGINT CLASS METHODS               *
   ***************************************************  */

// Default constructor
BigInt::BigInt()
	: digits(std::vector<uint>(1, 0))
	, positive(true)
{
	digits.reserve(8);
}

// Copy constructor
BigInt::BigInt(const BigInt& other)
	: digits(other.digits)
	, positive(other.positive)
{  }

// Move constructor
BigInt::BigInt(BigInt&& other) noexcept
	: digits(std::move(other.digits))
	, positive(std::exchange(other.positive, 0))
{  }

// int constructor
BigInt::BigInt(int num) { init<int>(num); }

// long long constructor
BigInt::BigInt(long long num) { init<long long>(num); }

// float constructor
BigInt::BigInt(float num) { init<int>(static_cast<int>(num)); }

// double constructor
BigInt::BigInt(double num) { init<double>(num); }

// Private constructor for initialization with digits and sign
BigInt::BigInt(const std::vector<uint>& digits_in, const bool is_positive)
	: digits(digits_in)
	, positive(is_positive)
{  }

// Constructor helper function, initializes BigInt from primitive number types
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

// work in progress
// for now, returns n-digit number in the form a1*(2^32)^0+a2*(2^32)^1+...+an*(2^32)^(n-1)
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

// return binary string representation of BigInt
std::string BigInt::to_binary_string() const {
	Timer<Microseconds> t;
	return Bitset(*this).to_string();
}

// get number of digits
std::vector<uint> BigInt::get_digits() const {
	return this->digits;
}

// return a boolean array containing the bits of your number
// ex: get_bits(15, false) == vector<bool>({ 1, 1, 1, 1 })  <== true
std::vector<bool> BigInt::get_bits(uint num, bool pad_uint) {
	std::vector<bool> res_bits;
	res_bits.reserve(BITS_IN_UINT);

	uint mask = 1;
	while (num > 0) {
		res_bits.push_back(num & mask); //grab last bit of num
		num >>= 1; // shift num right 1 bit, now what was the second to last bit is last bit
	}

	if (pad_uint) { // pad vector with 0s if they want 32-bit number
		for (size_t i = res_bits.size(); i < BITS_IN_UINT; ++i) {
			res_bits.push_back(0);
		}
	}

	return res_bits;
}

// return a boolean array containing the bits of your BigInt
//		this is done by calling above function for each uint digit 
//		and iteratively prepending their bit vectors
std::vector<bool> BigInt::get_bits() const {
	std::vector<bool> res_bits;
	size_t my_size = this->num_digits();
	res_bits.reserve(BITS_IN_UINT * my_size); 

	for (size_t i = 0; i < my_size; ++i) {
		std::vector<bool> bit_block_32 = get_bits(this->digits[i], (i < my_size - 1));
		res_bits.insert(res_bits.end(), bit_block_32.begin(), bit_block_32.end());
	}

	return res_bits;
}

// get number of base 2^32 digits in BigInt
size_t BigInt::num_digits() const {
	return this->digits.size();
}

// adds two BigInts with like signs (i.e. a+b where a,b >= 0 OR a,b < 0)
BigInt BigInt::add_like_signs(const BigInt& a, const BigInt& b) {
	std::vector<uint> result_digits;
	size_t a_num_digits = a.num_digits();
	size_t b_num_digits = b.num_digits();
	result_digits.reserve(std::max(a_num_digits, b_num_digits) + 16);

	size_t i = 0;
	size_t j = 0;
	ulong overflow = 0;
	while (i < a_num_digits && j < b_num_digits) { // add a's and b's digits pair-wise + carry
		ulong true_add = (ulong)a.digits[i++] + b.digits[j++] + overflow;
		uint new_digit = (uint)true_add; // gives result mod 2^32
		overflow = true_add >> BITS_IN_UINT; // 1 if true_add > BASE - 1 else 0
		result_digits.push_back(new_digit);
	}

	// b had less digits than a. 
	// iterate through remainder of a's digits, adding possible carries
	while (i < a_num_digits) {
		ulong true_add = (ulong)a.digits[i++] + overflow;
		uint new_digit = (uint)true_add; // gives result mod 2^32
		overflow = true_add >> BITS_IN_UINT; // 1 if true_add > BASE - 1 else 0
		result_digits.push_back(new_digit);
	}

	// a had less digits than b. 
	// iterate through remainder of b's digits, adding possible carries
	while (j < b_num_digits) {
		ulong true_add = (ulong)b.digits[j++] + overflow;
		uint new_digit = (uint)true_add; // gives result mod 2^32
		overflow = true_add >> BITS_IN_UINT; // 1 if true_add > BASE - 1 else 0
		result_digits.push_back(new_digit);
	}

	// take care of possible remaining carry
	if (overflow) {
		result_digits.push_back((uint)overflow);
	}

	return BigInt(result_digits, a.positive);
}

// adds two BigInts with different signs
BigInt BigInt::add_diff_signs(const BigInt& big, const BigInt& small) {
	std::vector<uint> result_digits;
	size_t big_num_digits = big.num_digits();
	size_t small_num_digits = small.num_digits();
	result_digits.reserve(small_num_digits + 1);

	size_t i = 0;
	size_t j = 0;
	uint underflow = 0;
	while (i < big_num_digits && j < small_num_digits) { // iterate pairwise through a's and b's digits, subtracting - underflow
		uint new_digit = big.digits[i] - small.digits[j] - underflow; // gives result mod 2^32
		underflow = (big.digits[i++] < small.digits[j++]) ? 1 : 0;
		result_digits.push_back(new_digit);
	}

	// finish off subtracting underflow from big's remaining digits
	while (i < big_num_digits) {
		uint new_digit = big.digits[i++] - underflow; // gives result mod 2^32
		underflow = 0;
		result_digits.push_back(new_digit);
	}

	// finish off subtracting underflow from small's remaining digits
	while (j < small_num_digits) {
		uint new_digit = small.digits[j++] - underflow; // gives result mod 2^32
		underflow = 0;
		result_digits.push_back(new_digit);
	}

	// remove possible zero-digits in MSB positions from result
	for (i = result_digits.size() - 1; i > 0; --i) {
		if (result_digits[i] != 0) {
			result_digits.resize(i + 1);
			break;
		}
	}

	return BigInt(result_digits, big.positive);
}

// performs long multiplication (gradeschool multiplication) on two BigInts a and b 
BigInt BigInt::long_mult(const BigInt& a, const BigInt& b) {
	bool sign = !(a.positive ^ b.positive); // just think of truth table for mult of neg and pos
	const BigInt* longer = &a;
	const BigInt* shorter = &b;
	size_t longer_size = longer->num_digits();
	size_t shorter_size = shorter->num_digits();
	if (shorter_size > longer_size) { // want the BigInt with more digits on top for mult, gives less additions
		std::swap(longer, shorter);
		std::swap(longer_size, shorter_size);
	}

	// perform multiplication algorithm
	BigInt result;
	size_t pad_zeros = 0;
	for (size_t i = 0; i < shorter_size; ++i) {
		uint overflow = 0;
		std::vector<uint> add_digits(pad_zeros); // pad successive BigInts to add with zeros
		add_digits.reserve(longer_size + 1 + pad_zeros);

		// multiply each digit of top BigNum by single digit of bottom BigNum
		for (size_t j = 0; j < longer_size; ++j) {
			ulong true_mult = ((ulong)shorter->digits[i] * longer->digits[j]) + overflow; // yes, 2^64-1 > (2^32-1)^2 + (2^32-1)
			uint new_digit = (uint)true_mult; // gives result mod 2^32
			overflow = true_mult >> BITS_IN_UINT; // overflow = true_mult / (2^32)
			add_digits.push_back(new_digit);
		}

		// take care of possible remaining carry
		if (overflow) {
			add_digits.push_back(overflow);
		}

		// keep running sum instead of storing
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

// returns BigInt a where a = this * right
// todo: pick optimal ranges in which to use different multiplication algos
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

// copy-assignment operator
BigInt& BigInt::operator= (const BigInt& right) {
	digits = std::vector<uint>(right.digits);
	positive = right.positive;
	return *this;
}

// move-assignment operator
BigInt& BigInt::operator= (BigInt&& right) noexcept {
	digits = std::move(right.digits);
	positive = std::exchange(right.positive, 0);
	return *this;
}

// is this == right?
bool BigInt::operator== (const BigInt& right) const {
	size_t my_size = this->num_digits();
	if (this->positive == right.positive) { // signs are the same, ok...
		if (my_size == right.num_digits()) { // hmm, same # of digits...
			for (size_t i = 0; i < my_size; ++i) {
				if (digits[i] != right.digits[i]) { // nevermind, we're different numbers
					return false;
				}
			}
			return true;
		}
	}
	return false;
}
 
// is this < right?
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
	while (i > 0 && j > 0) { // go from MSB -> LSB, first num with smaller digit is smaller
		if (digits[i] < right.digits[j]) {
			return true;
		}
		--i; --j;
	}
	return digits[i] < right.digits[j];
}

// is this > right?
bool BigInt::operator> (const BigInt& right) const {
	if (!(*this == right) && !(*this < right)) {
		return true;
	}
	return false;
}

// is this <= right?
bool BigInt::operator<= (const BigInt& right) const {
	if (!(*this > right)) {
		return true;
	}
	return false;
}

// is this >= right?
bool BigInt::operator>= (const BigInt& right) const {
	if (!(*this < right)) {
		return true;
	}
	return false;
}

// returns BigInt a where a = this + right
BigInt BigInt::operator+ (const BigInt& right) const {
	// same sign
	if (this->positive == right.positive) {
		return add_like_signs(*this, right);
	}

	// diff signs
	BigInt me(*this);
	bool positive_save = me.positive;
	me.positive = right.positive;
	if (me < right) { // compare magnitude of this and right
		me.positive = positive_save;
		return add_diff_signs(right, me); // want to send higher mag BigNum as first arg
	}
	me.positive = positive_save;
	return add_diff_signs(me, right);
}

// returns BigInt a where a = this - right
BigInt BigInt::operator- (const BigInt& right) const {
	BigInt opp_right = BigInt(right.digits, !right.positive);
	return *this + opp_right;
}

// return reference to this after adding right to its current value
BigInt& BigInt::operator+= (const BigInt& right) {
	*this = *this + right;
	return *this;
}







/* *************************************************** 
   *              BITSET CLASS METHODS               * 
   ***************************************************  */

// default constructor
Bitset::Bitset()
	: bytes(std::vector<byte>(1, 0))
{
	bytes.reserve(BS_DEFAULT_BYTES);
}

// construct bitset with enough memory allocated for size bytes
Bitset::Bitset(const size_t size)
	: bytes(std::vector<byte>(1, 0))
{
	bytes.reserve(size);
}

// copy constructor
Bitset::Bitset(const Bitset& other)
	: bytes(other.bytes)
{  }

// move constructor
Bitset::Bitset(Bitset&& other) noexcept
	: bytes(std::move(other.bytes))
{  }

// construct Bitset from BigInt
Bitset::Bitset(const BigInt& bi) {
	const size_t bi_num_digits = bi.num_digits();
	bytes = std::vector<byte>(bi_num_digits * UCHARS_IN_UINT); // each uint is 4 bytes

	std::vector<uint> bi_digits = bi.get_digits();
	for (size_t i = 0; i < bi_num_digits; ++i) { // for each uint digit in bi,
		for (size_t j = 0; j < UCHARS_IN_UINT; ++j) { // for each byte in uint digit
			bytes[(i * UCHARS_IN_UINT) + j] = Bitset::get_byte(bi_digits[i], j); // get (j+1)th byte from uint digit
		}
	}

	// remove 0-bytes
	for (size_t i = bytes.size() - 1; i > 0; --i) {
		if (bytes[i] != 0) {
			break;
		}
		bytes.pop_back();
	}
}

// returns the (which+1)th byte from num
byte Bitset::get_byte(uint num, const uint which) {
	if (which < 4) { // only 4 bytes in a uint
		uint shift = BITS_IN_BYTE * which;
		uint mask = 0xFF << shift; // do we want bits 0-7, 8-15, 16-23, or 24-31?
		num &= mask; // get rid of all bits we don't care about
		return (num >> shift); // shift relevant bits back down to end of number to store in byte and return
	}

	std::cout << "Error: requested byte #" << which + 1
		<< " of " << num << ", which does not exist\n";
	return -1;
}

// get number of bytes currently in Bitset
size_t Bitset::num_bytes() const {
	return bytes.size();
}

// convert bitset to binary string
std::string Bitset::to_string() const {
	const int ASCII_ZERO = 48;
	const size_t my_num_bytes = this->num_bytes();
	std::string bit_str = "";
	bit_str.reserve(my_num_bytes * BITS_IN_BYTE); // need one character in string for each bit in byte

	uint mask = 1;
	for (size_t i = 0; i < my_num_bytes; ++i) { // for each of my bytes,
		byte byte_copy = bytes[i];
		for (size_t j = 0; j < BITS_IN_BYTE; ++j) { // append '0' to string if last bit of byte_copy is 0, else append '1'
			bit_str.push_back((char)(ASCII_ZERO + (byte_copy & mask)));
			byte_copy >>= 1;
		}
	}
	std::reverse(bit_str.begin(), bit_str.end()); // bit string was built backwards, reverse it

	// remove extraneous 0 bits from string
	size_t zero_count = 0;
	for (zero_count; zero_count < bit_str.length(); ++zero_count) {
		if (bit_str[zero_count] != '0') {
			break;
		}
	}
	bit_str.erase(0, zero_count);

	return bit_str;
}

// returns a bitset containing the binary-coded-decimal representation of *this
Bitset Bitset::to_bcd() const {
	const size_t my_size = this->num_bytes();
	const ulong my_size_bits = (ulong)my_size * BITS_IN_BYTE;
	const ulong bcd_bits_needed = (my_size_bits + 4 * ((my_size_bits / 3) + 1));
	Bitset bcd((size_t)(bcd_bits_needed / BITS_IN_BYTE) + 1);



	return bcd;
}