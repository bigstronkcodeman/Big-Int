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

#include "BigInt.h"
//#include "Timer.h"
//
//void print_base_sum_form(const BigInt& n) {
//	std::vector<uint> digits = n.get_digits();
//	for (int i = digits.size() - 1; i >= 0; --i) {
//		std::cout << digits[i] << "*(2^32)^" << i << ((i > 0) ? (" + ") : (""));
//	}
//	std::cout << '\n';
//}
//
//int main() {
//	//BigInt f1 = 0;
//	//BigInt f2 = 1;
//	//int i = 1;
//	//for (BigInt f3 = f1 + f2; i < 50000; f3 = f1 + f2) {
//	//	//std::cout << "fibonacci term " << ++i << ": " << f3.to_string() << '\n';
//	//	f1 = f2;
//	//	f2 = f3;
//	//	++i;
//	//}
//
//	//std::vector<bool> bits = f2.get_bits();
//	//std::string bit_str = f2.to_binary_string();
//	//std::string decimal_str = f2.to_string();
//	//int diff = (x_n - y_n) * (((x_n - y_n < 0) * -1) + (x_n - y_n >= 0));
//
//	BigInt b1 = 10940919;
//	BigInt b2 = 1023401654;
//	BigInt b3 = 4294967296;
//	b3 = b3 * b3;
//	for (int i = 0; i < 25; ++i) {
//		std::cout << "iteration #" << i << '\n';
//		{
//			std::cout << "'Suba mult time: ";
//			Timer<Milliseconds> t;
//			b1 = b1 * b1;
//		}
//		{
//			/*std::cout << "Long mult time: ";
//			Timer<Milliseconds> t;
//			b3 = b3 * b3;*/
//		}
//		//std::cout << "10940919 * (1023401654^" << i + 1 << ") = " << b1.to_string() << '\n';
//		//print_base_sum_form(b1);
//		std::cout << "10940919^(2^" << i + 1 << ") = " << b1.to_string2() << '\n';
//	}
//
//	std::cout << '\n' << b1.to_string2();
//	b1 = b1 * b1;
//	std::cout << "^2 = " << b1.to_string2() << '\n';
//
//	return 0;
//}
