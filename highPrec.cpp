#include <vector>
#include <string>
#include <iostream>
#include <math.h>

#include "highPrec.h"

using namespace std;

#define print(input) cout << input << endl

#define ull unsigned long long

string intToHex(unsigned int value){
	string output;
	for (int i = 0; i < 8; i++){
		unsigned int temp = value & 0xf0000000;
		temp >>= 28;
		if (temp < 10){
			output += to_string(temp);
		}
		else{
			char thing = temp - 9 + 96;
			output += thing;
		}
		value <<= 4;
	}
	return output;
}
string intToHex(int value){
	if (value < 0){
		string output = "-";
		output += intToHex((unsigned int)(-value));
		return output;
	}
	else{
		return intToHex((unsigned int)value);
	}
}
highPrec::highPrec(){
	exponent = 0;
	negative = false;
	sigFigs = vector<unsigned int>({0});
}
highPrec::highPrec(double initialValue){
	if (initialValue == 0){
		sigFigs = vector<unsigned int>({0});
		exponent = 0;
		negative = false;
		return;
	}
	sigFigs = vector<unsigned int>();
	if (initialValue < 0){
		negative = true;
	}
	else{
		negative = false;
	}
	ull equiv =
		*(ull*)&initialValue;
	ull expMask = 0x7ff0000000000000;
	exponent = (equiv & expMask) >> 52;
	exponent -= 1023 + 52 - 32;
	int extra = exponent % 32;
	if (extra < 0){
		extra += 32;
	}
	ull sigFig =
		(equiv << 12) >> 12;
	sigFig += 1ull << 52;
	if (extra < 12){
		sigFig <<= extra;
		exponent -= extra;
		exponent /= 32;
		sigFigs.push_back(sigFig >> 32);
		ull temp = sigFig << 32;
		temp >>= 32;
		if (temp != 0){
			sigFigs.push_back(temp);
		}
	}
	else{
		unsigned int first = sigFig >> (64 - extra);
		ull second = sigFig << extra;
		second >>= 32;
		ull third = sigFig << (extra + 32);
		third >>= 32;
		sigFigs.push_back(first);
		if (second != 0){
			sigFigs.push_back(second);
			if (third != 0){
				sigFigs.push_back(third);
			}
		}
		exponent -= extra;
		exponent /= 32;
		exponent++;
	}
	prune();
}
highPrec::highPrec(vector<double> doubleSum){
	highPrec temp(doubleSum[0]);
	for (int i = 1; i < doubleSum.size(); i++){
		temp += highPrec(doubleSum[i]);
	}
	negative = temp.negative;
	sigFigs = temp.sigFigs;
	exponent = temp.exponent;
}
highPrec& highPrec::forcePrec(int size){
	while (sigFigs.size() < size){
		sigFigs.push_back(0);
	}
	while (sigFigs.size() > size){
		sigFigs.pop_back();
	}
	return *this;
}
void highPrec::prune(){
	while (sigFigs.size() > 1){
		if (sigFigs[0] == 0){
			sigFigs.erase(sigFigs.begin());
			exponent--;
		}
		else{
			break;
		}
	}
	while (sigFigs.size() > 1){
		if (sigFigs.back() == 0){
			sigFigs.pop_back();
		}
		else{
			break;
		}
	}
}

int highPrec::getPrec() const{
	return sigFigs.size();
}
unsigned int highPrec::operator[](int index) const{
	index -= exponent;
	index *= -1;
	if (index < 0) return 0;
	if (index >= sigFigs.size()) return 0;
	return sigFigs[index];
}
highPrec& highPrec::operator+=(const highPrec& other){
	if (negative != other.negative){
		negate();
		operator-=(other);
		negate();
		return *this;
	}
	int lowest = min(exponent - sigFigs.size(),
		other.exponent - other.sigFigs.size()) + 1;
	int highest = max(exponent, other.exponent);
	vector<unsigned int> newSigs(highest - lowest + 1);
	long long rollOver = 0;
	for (int i = lowest; i <= highest; i++){
		rollOver += (*this)[i];
		rollOver += other[i];
		newSigs[(i - highest) * -1] = rollOver;
		rollOver >>= 32;
	}
	exponent = highest;
	if (rollOver != 0){
		newSigs.insert(newSigs.begin(), rollOver);
		exponent += 1;
	}
	sigFigs = newSigs;
	return *this;
}
highPrec& highPrec::operator-=(const highPrec& other){
	if (negative != other.negative){
		negate();
		operator+=(other);
		negate();
		return *this;
	}
	unsigned long long intMask = 0xffffffff;
	int lowest = min(exponent - sigFigs.size(),
		other.exponent - other.sigFigs.size()) + 1;
	int highest = max(exponent, other.exponent);
	vector<unsigned int> newSigs(highest - lowest + 1);
	unsigned long long rollOver = 0;
	for (int i = lowest; i <= highest; i++){
		rollOver += operator[](i);
		rollOver -= other[i];
		if (rollOver > intMask){
			newSigs[(i - highest) * -1] = rollOver;
			rollOver = -1;
		}
		else{
			newSigs[(i - highest) * -1] = rollOver;
			rollOver = 0;
		}
	}
	exponent = highest;
	if (rollOver != 0){
		negate();
		for (int i = 0; i < newSigs.size(); i++){
			newSigs[i] *= -1;
			if (i != newSigs.size() - 1){
				newSigs[i]--;
			}
		}
		while(newSigs.size() > 1){
			if (newSigs[0] == 0){
				newSigs.erase(newSigs.begin());
				exponent--;
			}
			else{
				break;
			}
		}
		if (newSigs[0] == 0) exponent = 0;
	}
	sigFigs = newSigs;
	return *this;
}
highPrec& highPrec::operator*=(const highPrec& other){

	int targetPrecision = max(sigFigs.size(), other.sigFigs.size());

	highPrec total;

	for (int i = 0; i < other.sigFigs.size(); i++){
		highPrec temp(*this);
		temp *= other.sigFigs[i];
		temp.exponent += other.exponent - i;
		total += temp;
	}

	sigFigs = total.sigFigs;
	exponent = total.exponent;
	negative = negative != other.negative;

	while(sigFigs.size() > targetPrecision){
		sigFigs.pop_back();
	}

	return *this;
}
highPrec& highPrec::operator*=(const unsigned int& other){
	ull overflow = 0ull;
	for (int i = sigFigs.size() - 1; i >= 0; i--){
		overflow += sigFigs[i] * (ull)other;
		sigFigs[i] = overflow;
		overflow >>= 32;
	}
	if (overflow != 0){
		sigFigs.insert(sigFigs.begin(), overflow);
		exponent++;
	}
	return *this;
}
highPrec highPrec::approxRecip(unsigned int denom){
	if (denom == 1){
		return highPrec(1.0);
	}
	else{
		ull temp = 1ull;
		temp <<= 32;
		temp /= denom;
		highPrec output;
		output.exponent = -1;
		output.sigFigs.push_back(temp);
		return output;
	}
}
highPrec& highPrec::operator/=(const unsigned int& other){
	vector<unsigned int> result;
	ull overhead = 0;
	bool sigFigFound = false;
	for (int i = 0; i < sigFigs.size(); i++){
		overhead += sigFigs[i];
		ull quotient = overhead / other;
		overhead -= other * quotient;
		overhead << 32;
		if (quotient != 0 || sigFigFound){
			sigFigFound = true;
			result.push_back(quotient);
		}
	}
	if (result.size() == sigFigs.size()){
		sigFigs = result;
		return *this;
	}
	else{
		exponent -= sigFigs.size() - result.size();
		while (result.size() < sigFigs.size()){
			ull quotient = overhead / other;
			overhead -= other * quotient;
			overhead << 32;
			result.push_back(quotient);
		}
		sigFigs = result;
		return *this;
	}
}
highPrec& highPrec::operator/=(const highPrec& other){
	highPrec unionValue = highPrec(1.0);
	highPrec denominatorRecip =
		highPrec::approxRecip(other.sigFigs[0]);
	denominatorRecip.exponent -= other.exponent;
	int iterCount;
	int targetPrecision = max(sigFigs.size(), other.sigFigs.size());
	denominatorRecip.forcePrec(targetPrecision);
	if (targetPrecision == 1){
		iterCount = 6;
	}
	else{
		iterCount = 6 + (int)(log2(targetPrecision - 1));
	}
	for (int i = 0; i < iterCount; i++){
		denominatorRecip += denominatorRecip * (
			unionValue - denominatorRecip * other);
	}
	denominatorRecip.forcePrec(targetPrecision);
	return *this *= denominatorRecip;
}

void highPrec::negate(){
	negative = negative == false;
}

double highPrec::toDouble() const{
	vector<char> otherDataSpace;
	for (int i = 0; i < 8; i++){
		otherDataSpace.push_back(0);
	}
	for (int i = sigFigs.size() - 1; i >= 0; i--){
		for (int j = 0; j < 4; j++){
			otherDataSpace.push_back((sigFigs[i] >> 8 * j) & 255);
		}
	}
	int i = otherDataSpace.size() - 1;
	while(i > 7){
		if (otherDataSpace[i] != 0){
			break;
		}
		else{
			i--;
		}
	}
	if (i == 7){
		return 0.0;
	}
	else{
		double equiv = (double)*(unsigned long long*)&(otherDataSpace[i - 7]);
		int temp = 32 * (exponent - 1) -
			8 * (otherDataSpace.size() - i - 1);
		// equiv *= twoPow(temp);
		equiv *= exp(2.0, temp);
		if (negative){
			equiv *= -1;
		}
		return equiv;
	}
}

double highPrec::lowMult(double value) const{
	return toDouble() * value;
}

string pruneHexString(string input){
	while(input.length() > 1){
		if (input[0] == '0'){
			input.erase(input.begin());
		}
		else{
			if (input[0] == '-' && input[1] == '0' &&
					input.size() > 2){
				input.erase(input.begin() + 1);
			}
			else{
				break;
			}
		}
	}
	return input;
}

string highPrec::toString() const{
	string output;
	if (negative){
		output += "-";
	}
	if (exponent != 0){
		output += "(16^8)^";
		output += pruneHexString(intToHex(exponent));
		output += " * ";
	}
	output += pruneHexString(intToHex(sigFigs[0]));
	if (sigFigs.size()){
		output += ".";
	}
	for (int i = 1; i < sigFigs.size(); i++){
		output += intToHex(sigFigs[i]);
	}
	while(true){
		if (output.back() == '0'){
			output.pop_back();
		}
		else{
			if (output.back() == '.'){
				output.pop_back();
			}
			break;
		}
	}
	return output;
}

highPrec operator+(const highPrec& base, const highPrec& other){
	highPrec output(base);
	return output += other;
}
highPrec operator-(const highPrec& base, const highPrec& other){
	highPrec output(base);
	return output -= other;
}
highPrec operator*(const highPrec& base, const highPrec& other){
	highPrec output(base);
	return output *= other;
}
highPrec operator*(const highPrec& base, const unsigned int& other){
	highPrec output(base);
	return output *= other;
}
highPrec operator/(const highPrec& base, const highPrec& other){
	highPrec output(base);
	return output /= other;
}
highPrec operator/(const highPrec& base, const unsigned int& other){
	highPrec output(base);
	return output /= other;
}

bool operator<(const highPrec& base, const highPrec& other){
	if (base.negative == other.negative){
		if (base.exponent == other.exponent){
			bool magLess = false;
			int minVal = min(base.sigFigs.size(),
				other.sigFigs.size());
			for (int i = 0; i < minVal; i++){
				if (base.sigFigs[i] < other.sigFigs[i]){
					magLess = true;
					break;
				}
				else if (base.sigFigs[i] > other.sigFigs[i]){
					break;
				}
				if (i == minVal - 1 && !magLess){
					magLess = base.sigFigs.size() < other.sigFigs.size();
				}
			}
			return magLess != base.negative;
		}
		else{
			return (base.exponent < other.exponent) != base.negative;
		}
	}
	else{
		return base.negative;
	}
}

bool operator>(const highPrec& base, const highPrec& other){
	return other < base;
}
bool operator<=(const highPrec& base, const highPrec& other){
	return !(other < base);
}
bool operator>=(const highPrec& base, const highPrec& other){
	return !(base < other);
}

string to_string(const highPrec& input){
	return input.toString();
}
