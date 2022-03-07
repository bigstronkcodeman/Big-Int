#include "../include/BigInt.hpp"

int main() {
	BigInt b1 = 0;
	BigInt b2 = 1;
	BigInt b3 = 1;
	std::cout << "fib[0]: " << b1.to_string() << '\n';
	std::cout << "fib[1]: " << b2.to_string() << '\n';
	for (int i = 2; i <= 100000; ++i) {
		std::cout << "fib[" << i << "]: " << b3.to_string() << '\n';
		std::cout << "fib[" << i << "] generated\n";
		b1 = b2;
		b2 = b3;
		b3 = b1 + b2;
	}
	std::cout << "done!\n";
	std::cout << b3.to_string() << '\n';

	return 0;
}
