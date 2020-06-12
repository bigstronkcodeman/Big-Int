
#include "BigInt.h"
#include "Timer.h"


std::ostream& operator<< (std::ostream& os, const std::vector<bool>& v) {
	for (_long i = v.size() - 1; i >= 0; --i) {
		os << v[(uint)i];
		if (i == 0) {
			os << '\n';
		}
	}
	return os;
}

int main() {
	BigInt b1(-32784189);
	for (int i = 0; i < 13; ++i) {
		b1 = b1 * b1;
	}

	std::string str = b1.to_binary_string();
	std::cout << str << "\n";

	return 0;
}