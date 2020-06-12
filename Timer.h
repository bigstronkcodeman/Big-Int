#pragma once
#include <chrono>

// these are the only classes T for which Timer<T> is defined
typedef std::chrono::duration<int, std::ratio<60i64>> Minutes;
typedef std::chrono::duration<long long> Seconds;
typedef std::chrono::duration<long long, std::milli> Milliseconds;
typedef std::chrono::duration<long long, std::micro> Microseconds;
typedef std::chrono::duration<long long, std::nano> Nanoseconds;

template <class T>
struct Timer {
	std::chrono::steady_clock::time_point start;
	Timer() : start(std::chrono::high_resolution_clock::now())
	{  }

	~Timer() {
		std::chrono::steady_clock::time_point finish = std::chrono::high_resolution_clock::now();
		std::cout << std::chrono::duration_cast<T>(finish - start).count();

		const std::type_info& ti_T = typeid(T);
		if (ti_T == typeid(Minutes)) {
			std::cout << "min\n";
		}
		else if (ti_T == typeid(Seconds)) {
			std::cout << "s\n";
		}
		else if (ti_T == typeid(Milliseconds)) {
			std::cout << "ms\n";
		}
		else if (ti_T == typeid(Microseconds)) {
			std::cout << "us\n";
		}
		else if (ti_T == typeid(Nanoseconds)) {
			std::cout << "ns\n";
		}
	}
};