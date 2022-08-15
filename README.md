# Big-Int
A performant library that provides functionality for arbitrary precision integer arithmetic, handling numbers with potentially millions of decimal digits. (WIP)

This library is designed to handle arithmetic operations on huge numbers that cannot normally be stored in native types. Numbers are stored as sequences of base 2^32 "digits" (or in binary, operating on 32-bit chunks at a time if you like to think about it that way) so as to maximize the magnitude of the number that can be stored within memory. Currently only supports basic operations such as addition, subtraction, multiplication, and exponentiation, but more operations will be added as time progresses.
