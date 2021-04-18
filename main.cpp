#include "../include/BigInt.hpp"

void print_base_sum_form(const BigInt& n) {
	std::vector<uint> digits = n.get_digits();
	for (int i = digits.size() - 1; i >= 0; --i) {
		std::cout << digits[i] << "*(2^32)^" << i << ((i > 0) ? (" + ") : (""));
	}
	std::cout << '\n';
}

void cmp(const std::string& correct, const std::string& possibly_correct) {
	size_t ci = 0, pci = 0;
	for (size_t i = 0; i < correct.length(); ++i) {
		if (correct[ci] != possibly_correct[pci]) {
			std::cout << "missing a " << correct[ci] << " at index " << pci-- << '\n';
		}
		++ci;
		++pci;
	}
}

int main() {
	// BigInt b1 = 10940919;
	// BigInt b2 = 1023401654;
	// BigInt b3 = 4294967296;
	// b3 = b3 * b3;
	// for (int i = 0; i < 25; ++i) {
	// 	std::cout << "iteration #" << i << '\n';
	// 	b1 = b1 * b1;
	// 	std::string crct = b1.to_string();
	// 	//std::string crct = b1.to_string2();
	// 	//std::cout << "10940919^(2^" << i + 1 << ") = " << crct << '\n';
	// 	std::cout << "10940919^(2^" << i + 1 << ") = " << crct << '\n';
	// 	//if (crct != crct_maybe) {
	// 		//std::cout << "Result was incorrect!\n";
	// 	//}
	// 	//cmp(crct, crct_maybe);
	// 	std::cout << '\n';
	// }

	// std::cout << '\n' << b1.to_string2();
	// b1 = b1 * b1;
	// std::cout << "^2 = " << b1.to_string2() << '\n';

	BigInt b1 = 0;
	BigInt b2 = 1;
	BigInt b3 = 1;
	//std::cout << "fib[0]: " << b1.to_string() << '\n';
	//std::cout << "fib[1]: " << b2.to_string() << '\n';
	for (int i = 2; i <= 100000; ++i) {
		//std::cout << "fib[" << i << "]: " << b3.to_string() << '\n';
		//std::cout << "fib[" << i << "] generated\n";
		b1 = b2;
		b2 = b3;
		b3 = b1 + b2;
	}
	std::cout << "done~!\n";
	std::cout << b3.to_string() << '\n';

	// BigInt b1 = 1;
	// BigInt b2 = -1;
	// for (int i = 0; i < 100; ++i) {
	// 	std::cout << "iteration " << i+1 << ": " << b1.to_string() << '\n';
	// 	b1 *= b2;
	// }

	return 0;
}
