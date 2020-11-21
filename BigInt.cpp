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
	if (digits.size() == 0) digits.push_back(0);
}

// return binary string representation of BigInt
std::string BigInt::to_binary_string() const {
	std::string bin_str = "";
	uint mask = 1 << (BITS_IN_UINT - 1); // mask to extract leftmost bit (MSB) of uint digit
	bool still_leading_zeros = true; // want to flag when we are done with leading zeros so we can exclude them
	for (_long i = this->num_digits() - 1; i >= 0; --i) { // for each digit in this BigInt
		uint digit_copy = this->digits[size_t(i)];
		for (size_t j = 0; j < BITS_IN_UINT; ++j) { // for each bit in this uint digit
			char bit = (char)('0' + ((digit_copy & mask) >> (BITS_IN_UINT - 1))); // extract bit and convert to character representation
			if (!still_leading_zeros) {
				bin_str.push_back(bit); // append bit to string if we're done with leading zeros
			}
			else if (bit == '1') {
				bin_str.push_back(bit); // append first non-zero bit and flag that leading zeros are passed
				still_leading_zeros = false;
			}
			digit_copy <<= 1;
		}
	}
	return bin_str;
}

// convert number to decimal string by converting from binary to BCD, then BCD to decimal - complexity O(n^2), n is # of bits
std::string BigInt::to_string() const {
	// an n-bit binary number has at most d = nlog10(2) + 1 decimal digits, so reserve 4d bits for bcd representation
	const size_t num_digits = this->num_digits();
	std::vector<uint> bcd((BITS_IN_NIBBLE * (log10(2) * num_digits * BITS_IN_UINT + 1)) / BITS_IN_UINT + 1, 0);

	// perform double-dabble algorithm to convert *this from binary to BCD
	for (_long i = num_digits - 1; i >= 0; --i) { // for each uint digit in this BigInt's digits (going from MSD -> LSD)
		uint digit = this->digits[i];

		uint bit_iters = 0; // want to keep track of how many bits we've iterated through on current uint digit to shift down to LSB after masking
		for (uint mask = 1 << (BITS_IN_UINT - 1); mask > 0; mask >>= 1) { // for each bit in digit (going from MSB -> LSB)
			uint insert_bit = (digit & mask) >> (BITS_IN_UINT - 1 - bit_iters++); // grab bit, shift down to LSB 

			for (_long j = bcd.size() - 1; j >= 0; --j) { // for each bcd byte
				if (bcd[j] != 0) { // dabble if bcd[j] is non-zero
					uint nibble_iters = 0; // want to keep track of which nibble we are on to shift down accordingly
					for (uint nibble_mask = 0xF; nibble_mask > 0; nibble_mask <<= BITS_IN_NIBBLE) { // for each nibble in this uint digit
						uint nibble = (bcd[j] & nibble_mask) >> nibble_iters; // extract it from digit, shift down to four LSBs
						if (nibble > 4) {
							nibble += 3; // adjust nibble per rules of double dabble algorithm
							bcd[j] = (bcd[j] & ~nibble_mask) | (nibble << nibble_iters); // re-insert modified nibble into uint digit at initial position
						}
						nibble_iters += BITS_IN_NIBBLE;
					}
				}
			}

			uint lmb_mask = 1 << (BITS_IN_UINT - 1); // mask for getting the leftmost bit of a uint digit
			for (_long k = bcd.size() - 1; k >= 0; --k) { // for each bcd uint digit
				if (k == 0 || bcd[k - 1] != 0) {
					uint lmb = (k > 0) ? ((bcd[k - 1] & lmb_mask) >> (BITS_IN_UINT - 1)) : (insert_bit); // grab leftmost bit from right adjacent uint digit OR use insert_bit from non-bcd if we've reached the ones column
					bcd[k] = (bcd[k] << 1) | lmb; // insert the desired bit as LSB
				}
			}
		}
	}

	// build decimal string from bcd digit vector
	std::string decimal_str = "0";
	bool leading_zeros = true; // want to flag when leading zeros end so they can be excluded
	for (_long i = bcd.size() - 1; i >= 0; --i) { // for each bcd uint digit 
		uint nibble_iters = 0; // want to keep track of how many nibbles have been iterated through in current digit so we can shift accordingly
		for (uint mask = 0xF << (BITS_IN_UINT - BITS_IN_NIBBLE); mask > 0; mask >>= BITS_IN_NIBBLE) { // for every nibble in this uint digit (going from MSN -> LSN)
			char digit = '0' + ((bcd[i] & mask) >> ((BITS_IN_UINT - BITS_IN_NIBBLE) - nibble_iters)); // use nibble as ascii offset from '0' to get digit
			if (!leading_zeros) {
				decimal_str.push_back(digit); // append the digit if there are no more leading zeros
			}
			else if (digit != '0') {
				decimal_str.pop_back();
				if (leading_zeros && !this->positive) {
					decimal_str.push_back('-'); // negative number needs sign
				}
				decimal_str.push_back(digit); // append the first non-zero digit and flag that leading zeros are done
				leading_zeros = false;
			}
			nibble_iters += BITS_IN_NIBBLE;
		}
	}

	return decimal_str;
}

std::string BigInt::to_string2() const {
	if (*this == 0) {
		return "0";
	}
	const uint BIGGEST_POW10 = uint(1000000000); // 1000000000, i.e. 10^9, is the max power of 10 we can store in 32 bits
	std::vector<ulong> base10_chunks;
	//base10_digits.reserve(1 + int((this->num_digits() * BITS_IN_UINT) / log2(BIGGEST_POW10)));
	size_t size = 0;
	for (_long i = this->num_digits() - 1; i >= 0; --i) {
		uint hi = this->digits[i];
		for (size_t j = 0; j < size; ++j) {
			ulong z = ulong(base10_chunks[j]) << BITS_IN_UINT | hi;
			hi = uint(z / BIGGEST_POW10); 
			base10_chunks[j] = uint(z - ulong(hi) * BIGGEST_POW10);
			//hi = (z / BIGGEST_POW10) % BASE;
			//base10_chunks[j] = (z - ulong(hi) * BIGGEST_POW10) % BASE;
		}
		while (hi) {
			base10_chunks.push_back(hi % BIGGEST_POW10);
			++size;
			hi /= BIGGEST_POW10;
		}
	}

	std::string base10 = "";
	for (_long i = base10_chunks.size() - 1; i >= 0; --i) {
		base10 += std::to_string(base10_chunks[i]);
	}

	return base10;
}

// get list of digits
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

	if (pad_uint) { // pad vector with 0s if we want 32-bit number
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

// get number of base 2^32 (BASE) digits in BigInt
size_t BigInt::num_digits() const {
	return this->digits.size();
}

// adds two BigInts with like signs (i.e. a+b where a,b >= 0 OR a,b < 0), complexity O(n)
BigInt BigInt::add_like_signs(const BigInt& a, const BigInt& b) {
	std::vector<uint> result_digits;
	size_t a_num_digits = a.num_digits();
	size_t b_num_digits = b.num_digits();
	result_digits.reserve(std::max(a_num_digits, b_num_digits) + 16);

	size_t i = 0;
	size_t j = 0;
	ulong overflow = 0;
	while (i < a_num_digits && j < b_num_digits) { // add a's and b's digits pair-wise + carry
		ulong true_add = ulong(a.digits[i++]) + b.digits[j++] + overflow;
		uint new_digit = uint(true_add); // gives result mod BASE
		overflow = true_add >> BITS_IN_UINT; // 1 if true_add > BASE - 1 else 0
		result_digits.push_back(new_digit);
	}

	// b had less digits than a. 
	// iterate through remainder of a's digits, adding possible carries
	while (i < a_num_digits) {
		ulong true_add = ulong(a.digits[i++]) + overflow;
		uint new_digit = uint(true_add); // gives result mod BASE
		overflow = true_add >> BITS_IN_UINT; // 1 if true_add > BASE - 1 else 0
		result_digits.push_back(new_digit);
	}

	// a had less digits than b. 
	// iterate through remainder of b's digits, adding possible carries
	while (j < b_num_digits) {
		ulong true_add = ulong(b.digits[j++]) + overflow;
		uint new_digit = uint(true_add); // gives result mod BASE
		overflow = true_add >> BITS_IN_UINT; // 1 if true_add > BASE - 1 else 0
		result_digits.push_back(new_digit);
	}

	// take care of possible remaining carry
	if (overflow) {
		result_digits.push_back(uint(overflow));
	}

	return BigInt(result_digits, a.positive);
}

// adds two BigInts with different signs, complexity O(n)
BigInt BigInt::add_diff_signs(const BigInt& big, const BigInt& small) {
	std::vector<uint> result_digits;
	size_t big_num_digits = big.num_digits();
	size_t small_num_digits = small.num_digits();
	result_digits.reserve(small_num_digits + 1);

	size_t i = 0;
	size_t j = 0;
	uint underflow = 0;
	while (i < big_num_digits && j < small_num_digits) { // iterate pairwise through a's and b's digits, subtracting - underflow
		uint new_digit = big.digits[i] - small.digits[j] - underflow; // gives result mod BASE
		underflow = (big.digits[i++] < small.digits[j++]) ? 1 : 0;
		result_digits.push_back(new_digit);
	}

	// finish off subtracting underflow from big's remaining digits
	while (i < big_num_digits) {
		uint new_digit = big.digits[i++] - underflow; // gives result mod BASE
		underflow = 0;
		result_digits.push_back(new_digit);
	}

	// finish off subtracting underflow from small's remaining digits
	while (j < small_num_digits) {
		uint new_digit = small.digits[j++] - underflow; // gives result mod BASE
		underflow = 0;
		result_digits.push_back(new_digit);
	}

	// remove possible zero-digits in MSB positions from result
	for (_long ind = result_digits.size() - 1; ind >= 0; --ind) {
		if (result_digits[ind] != 0) {
			result_digits.resize(ind + 1);
			break;
		}
	}

	return BigInt(result_digits, big.positive);
}

// performs long multiplication (gradeschool multiplication) on two BigInts a and b, complexity O(n^2)
BigInt BigInt::long_mult(const BigInt& a, const BigInt& b) {
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
		std::vector<uint> add_digits(pad_zeros, 0); // pad successive BigInts to add with zeros
		add_digits.reserve(longer_size + 1 + pad_zeros);

		// multiply each digit of top BigNum by single digit of bottom BigNum
		for (size_t j = 0; j < longer_size; ++j) {
			ulong true_mult = (ulong(shorter->digits[i]) * longer->digits[j]) + overflow; // yes, 2^64-1 > (2^32-1)^2 + (2^32-1)
			uint new_digit = uint(true_mult); // gives result mod BASE
			overflow = true_mult >> BITS_IN_UINT; // overflow = true_mult / BASE
			add_digits.push_back(new_digit);
		}

		// take care of possible remaining carry
		if (overflow) {
			add_digits.push_back(overflow);
		}

		// keep running sum instead of storing
		result += BigInt(add_digits, true);
		++pad_zeros;
	}
	return result;
}

// performs karatsuba multiplication (fast multiplication) on two BigInts a and b, complexity O(n^(log2-(3)))
BigInt BigInt::karatsuba_mult(const BigInt& x, const BigInt& y) {
	const size_t x_n = x.num_digits();
	const size_t y_n = y.num_digits(); 

	if (x_n < KARATSUBA_CUTOFF && y_n < KARATSUBA_CUTOFF) {
		return long_mult(x, y); // karatsuba overhead not worth it, just do long multiplication
	}

	if (x_n == y_n) {
		const size_t m = (x_n / 2) + (x_n % 2); // x_n/2 if x_n is even, (x_n+1)/2 if x_n is odd
		const BigInt a(std::vector<uint>(x.digits.begin() + m, x.digits.end()), true);   // initialize a to left half of x
		const BigInt b(std::vector<uint>(x.digits.begin(), x.digits.begin() + m), true); // initialize b to right half of x
		const BigInt c(std::vector<uint>(y.digits.begin() + m, y.digits.end()), true);   // initialize c to left half of y
		const BigInt d(std::vector<uint>(y.digits.begin(), y.digits.begin() + m), true); // initialize d to right half of y
		
		BigInt ac = karatsuba_mult(a, c);
		BigInt bd = karatsuba_mult(b, d);
		BigInt ad_plus_bc = karatsuba_mult(a + b, c + d) - ac - bd; // (a+b)(c+d)=ac+ad+bc+bd ==> ac+ad+bc+bd-ac-bd=ad+bc

		if (ac > 0) ac.digits.insert(ac.digits.begin(), 2 * m, 0); // multiply by BASE^(2*m)
		if (ad_plus_bc > 0) ad_plus_bc.digits.insert(ad_plus_bc.digits.begin(), m, 0); // multiply by BASE^m

		return ac + ad_plus_bc + bd;
	}
	else {
		const BigInt* biggest = &x;
		const BigInt* smallest = &y;
		if (biggest->num_digits() < smallest->digits.size()) { // x and y have a different number of digits, need to know which number has least digits to pad it
			std::swap(biggest, smallest);
		}
		BigInt padded(*smallest);
		padded.digits.insert(padded.digits.end(), biggest->num_digits() - smallest->num_digits(), 0); // pad the smaller number with leading 0s to equalize lengths
		return karatsuba_mult(*biggest, padded); // restart karatsuba with our now same-length inputs
	}
}

// todo
BigInt BigInt::schonhage_strassen_mult(const BigInt& a, const BigInt& b) {
	return BigInt();
}

// returns BigInt a where a = this * right
// todo: pick optimal ranges in which to use different multiplication algos
BigInt BigInt::operator* (const BigInt& right) const {
	BigInt product;
	product = karatsuba_mult(*this, right);
	product.positive = !(this->positive ^ right.positive); // just think of truth table for mult of neg and pos
	return product;
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

// is *this == right?
bool BigInt::operator== (const BigInt& right) const {
	size_t my_size = this->num_digits();
	if (my_size == right.num_digits()) { // same # of digits, ok...
		if (this->positive == right.positive) { // hmm, signs are the same too...
			for (size_t i = 0; i < my_size; ++i) {
				if (digits[i] != right.digits[i]) {
					return false; // nevermind, we have a different digit so we're different numbers
				}
			}
			return true;
		}
	}
	return false;
}

// is *this < right?
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

// is *this > right?
bool BigInt::operator> (const BigInt& right) const {
	if (!(*this == right) && !(*this < right)) {
		return true;
	}
	return false;
}

// is *this <= right?
bool BigInt::operator<= (const BigInt& right) const {
	if (!(*this > right)) {
		return true;
	}
	return false;
}

// is *this >= right?
bool BigInt::operator>= (const BigInt& right) const {
	if (!(*this < right)) {
		return true;
	}
	return false;
}

// returns BigInt a where a = *this + right
BigInt BigInt::operator+ (const BigInt& right) const {
	// same sign
	if (this->positive == right.positive) {
		return add_like_signs(*this, right);
	}

	// diff signs
	BigInt me(*this);
	BigInt them(right);
	bool my_sign = me.positive;
	bool their_sign = them.positive;
	me.positive = true;
	them.positive = true;
	if (me < right) { // compare magnitude of *this and right
		me.positive = my_sign;
		them.positive = their_sign;
		return add_diff_signs(them, me); // want to send higher mag BigNum as first arg
	}
	them.positive = their_sign;
	me.positive = my_sign;
	return add_diff_signs(me, them);
}

// returns BigInt a where a = *this - right
BigInt BigInt::operator- (const BigInt& right) const {
	BigInt opp_right = BigInt(right.digits, !right.positive);
	return *this + opp_right;
}

// return reference to *this after adding right to it
BigInt& BigInt::operator+= (const BigInt& right) {
	*this = *this + right;
	return *this;
}

// return reference to *this after subtracting right from it
BigInt& BigInt::operator-= (const BigInt& right) {
	*this = *this - right;
	return *this;
}

// return reference to *this after multiplying by right
BigInt& BigInt::operator*= (const BigInt& right) {
	*this = *this * right;
	return *this;
}
