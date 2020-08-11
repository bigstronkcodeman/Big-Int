#include "BigInt.h"
#include "Timer.h"

int main() {
	BigInt b1(418);
	for (int i = 0; i < 10; ++i) { // calculates 418^(2^10)
		b1 = b1 * b1;
	}

	std::cout << "b1 = " << b1.to_binary_string() << '\n'
			 << "b1 decimal: " << b1.to_string() << '\n';

	return 0;
}