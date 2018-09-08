#pragma once
#include <string>
#include <vector>

class highPrec{
	//when exponent = 0, value = sigFigs[0] + (2^32)^-1 * sigFigs[1]...
	//else, value = (value from above) * (2^32)^exponent
	//negative acts as the sign bit
	bool negative;
	std::vector<unsigned int> sigFigs;
	int exponent;
	static highPrec approxRecip(unsigned int denom);
	void prune();
public:
	highPrec();
	highPrec(double initialValue);
	highPrec(std::vector<double> precisionSum);
	highPrec& forcePrec(int size);
	int getPrec() const;
	unsigned int operator[](int index) const;
	highPrec& operator+=(const highPrec& other);
	highPrec& operator-=(const highPrec& other);
	highPrec& operator*=(const highPrec& other);
	highPrec& operator*=(const unsigned int& other);
	highPrec& operator/=(const highPrec& other);
	highPrec& operator/=(const unsigned int& other);
	highPrec operator-() const;
	highPrec& removeInt();
	friend unsigned int operator%(const highPrec& base, const unsigned int& other);
	friend bool operator<(const highPrec& base, const highPrec& other);
	void negate();
	double toDouble() const;
	double lowMult(double value) const;
	std::string toString() const;
};

highPrec operator+(const highPrec& base, const highPrec& other);
highPrec operator-(const highPrec& base, const highPrec& other);
highPrec operator*(const highPrec& base, const highPrec& other);
highPrec operator*(const highPrec& base, const unsigned int& other);
highPrec operator/(const highPrec& base, const highPrec& other);
highPrec operator/(const highPrec& base, const unsigned int& other);
unsigned int operator%(const highPrec& base, const unsigned int& other);
bool operator<(const highPrec& base, const highPrec& other);
bool operator>(const highPrec& base, const highPrec& other);
bool operator<=(const highPrec& base, const highPrec& other);
bool operator>=(const highPrec& base, const highPrec& other);
std::string to_string(const highPrec& input);
