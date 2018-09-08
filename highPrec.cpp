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
	negative = initialValue < 0;
	ull equiv =
		*(ull*)&initialValue;//convert to ull in order to do bit manipulation
	ull expMask = 0x7ff0000000000000;//mask to collect exponent bits from double
	exponent = (equiv & expMask) >> 52;//bit shift out the trailing zeros from exp
	///////////////////////////////////////////
	exponent -= 1023 + 52 - 32;
	//1023 acts as zero for the double exponent, so subtract it
	//-52 places the decimal point at the end of the double data
	//rather than at the start,
	//and the +32 moves the decimal back by what our base is (base 2^32)
	///////////////////////////////////////////
	int extra = exponent % 32;
	//see how far off the double exponent is from a base 2^32 exponent
	if (extra < 0){
		extra += 32;//modulo (%) can return negative, so correct for that
	}
	ull sigFig =
		(equiv << 12) >> 12;//first bit shift chops off sign bit and exponent bits,
		//second bit shift moves back to be flush with right most bits
	sigFig += 1ull << 52;//adds in the implied bit
	if (extra < 12){
		//////////////////////////////////////////////////////
		sigFig <<= extra;//since extra < 12, we have enough space to do this without loss of data
		//this will line up our 2^32 chunks with the decimal point
		///////////////////////////////////////////////////////
		exponent -= extra;
		exponent /= 32;
		//change exponent into base 2^32 exponent
		///////////////////////////////////////////////////
		sigFigs.push_back(sigFig >> 32);//left most chunk first (right shift chops off right)
		ull temp = sigFig << 32;//left shift chops of left
		temp >>= 32;//fix to be flush with right most bit
		sigFigs.push_back(temp);
	}
	else{
		//in contrast to before, we must now collect three ints in order to get
		//the chunks to line up with the decimal point. First is the one
		//with fewer than (extra) significant bits:
		unsigned int first = sigFig >> (64 - extra);
		//////////////////////////////////////////////////////////
		ull second = sigFig << extra;
		second >>= 32;
		//must chop off both right and left bits, saving only 32 inbetween bits
		///////////////////////////////////////////////////////////
		ull third = sigFig << (extra + 32);//chop off left bits
		third >>= 32;//correct position to be flush with left bits (in an int)
		sigFigs.push_back(first);//left most first
		sigFigs.push_back(second);
		sigFigs.push_back(third);
		exponent -= extra;
		exponent /= 32;
		exponent++;//due to the extra int needed to store the data, the exponent is shifted by one.
	}
	prune();//check for any leading or trailing zeros
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
		//add trailing zeros until force level reached
		sigFigs.push_back(0);
	}
	while (sigFigs.size() > size){
		//remove trailing vals until force level reached
		sigFigs.pop_back();
	}
	return *this;
}
void highPrec::prune(){
	while (sigFigs.size() > 1){
		//remove leading zeros
		if (sigFigs[0] == 0){
			sigFigs.erase(sigFigs.begin());
			exponent--;
		}
		else{
			break;
		}
	}
	while (sigFigs.size() > 1){
		//remove trailing zeros
		if (sigFigs.back() == 0){
			sigFigs.pop_back();
		}
		else{
			break;
		}
	}
	if (sigFigs.size() == 1 && sigFigs[0] == 0)
	{
		exponent = 0;
		negative = false;
	}
}

int highPrec::getPrec() const{
	return sigFigs.size();
}
unsigned int highPrec::operator[](int index) const{
	//return indexth place. For example,
	//103.25 stored as {1,0,3,2,5} exp = 2,
	//index = 0 would return 3,
	//index = -1 would return 2,
	//index = 1 would return 0
	//index = 5 would return 0 (defaults to zero)
	index -= exponent;
	index *= -1;
	if (index < 0) return 0;
	if (index >= sigFigs.size()) return 0;
	return sigFigs[index];
}
highPrec& highPrec::operator+=(const highPrec& other){
	if (negative != other.negative){
		//return -((-this) - (other))
		//insures similar sign in subtraction function
		negate();
		operator-=(other);
		negate();
		return *this;
	}
	int lowest = min(exponent - sigFigs.size(),
		other.exponent - other.sigFigs.size()) + 1;//lowest significant figure
	//add one to compensate for zero indexing (actually want (exponent - maxIndexValue))
	int highest = max(exponent, other.exponent);//hightest significant figure
	vector<unsigned int> newSigs(highest - lowest + 1);
	//total places spanned. We're counting posts not fences so add one
	long long rollOver = 0;
	for (int i = lowest; i <= highest; i++){
		rollOver += operator[](i);
		rollOver += other[i];
		newSigs[(i - highest) * -1] = rollOver;//extra bits will be automatically removed
		rollOver >>= 32;//bit shift what didn't stick for the next go round
	}
	exponent = highest;
	if (rollOver != 0){
		//means new highest value. like 9+9 = 18, we need more sig figs and higher exponent
		newSigs.insert(newSigs.begin(), rollOver);
		exponent += 1;
	}
	sigFigs = newSigs;
	return *this;
}
highPrec& highPrec::operator-=(const highPrec& other){
	if (negative != other.negative){
		//return -((-this) + (other))
		negate();
		operator+=(other);
		negate();
		return *this;
	}
	unsigned long long intMask = 0xffffffff;//largest int value
	int lowest = min(exponent - sigFigs.size(),
		other.exponent - other.sigFigs.size()) + 1;//see addition
	int highest = max(exponent, other.exponent);
	vector<unsigned int> newSigs(highest - lowest + 1);//see addition
	unsigned long long rollOver = 0;
	for (int i = lowest; i <= highest; i++){
		rollOver += operator[](i);
		rollOver -= other[i];
		if (rollOver > intMask){
			//indicated value went negative, need to carry from the right
			newSigs[(i - highest) * -1] = rollOver;
			rollOver = -1;//carry from right === subtract one from right
		}
		else{//no carrying
			newSigs[(i - highest) * -1] = rollOver;
			rollOver = 0;
		}
	}
	exponent = highest;
	if (rollOver != 0){
		//subtracting what we don't have, go negative & twos compliment
		negate();
		for (int i = 0; i < newSigs.size(); i++){
			newSigs[i] *= -1;
			if (i != newSigs.size() - 1){
				newSigs[i]--;
			}
		}
		while(newSigs.size() > 1){//remove leading zeros
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
	//123*456
	//= (123 * 4) * 10^2
	//+ (123 * 5) * 10^1
	//+ (123 * 6) * 10^0

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

	forcePrec(targetPrecision);

	return *this;
}
highPrec& highPrec::operator*=(const unsigned int& other){
	ull overflow = 0ull;
	for (int i = sigFigs.size() - 1; i >= 0; i--){
		overflow += sigFigs[i] * (ull)other;
		sigFigs[i] = overflow;
		overflow >>= 32;//see addition function
	}
	if (overflow != 0){
		sigFigs.insert(sigFigs.begin(), overflow);
		exponent++;//see addition function
	}
	return *this;
}
highPrec highPrec::approxRecip(unsigned int denom){
	//approximate 1 / denom (this is a static private function)
	if (denom == 1){
		return highPrec(1.0);
	}
	else{
		ull temp = 1ull;
		temp <<= 32;//use the same size as our base (base 2^32)
		temp /= denom;
		highPrec output;
		output.exponent = -1;
		output.sigFigs.push_back(temp);//left most bits will automatically be removed
		return output;
	}
}
highPrec& highPrec::operator/=(const unsigned int& other){
	//essentially the quotient algorithm learned in elementary school
	vector<unsigned int> result;
	ull overhead = 0;
	bool sigFigFound = false;
	for (int i = 0; i < sigFigs.size(); i++){
		overhead += sigFigs[i];
		ull quotient = overhead / other;
		overhead -= other * quotient;
		overhead <<= 32;//next number will slot in on the right
		if (quotient != 0 || sigFigFound){
			//ensures no leading zeros
			sigFigFound = true;
			result.push_back(quotient);
		}
	}
	if (result.size() == sigFigs.size()){
		sigFigs = result;
		return *this;
	}
	else{//there were leading zeros, must compensate
		exponent -= sigFigs.size() - result.size();
		while (result.size() < sigFigs.size()){
			//use trailing zeros to fill in space
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
	//We start with an estimate of 1/other (using approxRecip)
	//Let's say we have x = other, y = 1/x, and y0 = current estimate
	//And let's say dy = y - y0 and ds = x*y0 - 1 (note that x*y = 1)
	//We want to solve for dy so that we can correct y0 to y
	//x*y = 1 -> x*(y0 + dy) = 1 -> x*y0 + x*dy = 1 -> ds + 1 + x*dy = 1
	//-> ds + x*dy = 0 -> dy = -ds / x
	//now we use 1/x ~ y0 (we use our estimate of y in place of y)
	//This gives dy = -y0 * ds = -y0 * (x*y0 - 1) = y0 * (1 - x*y0)
	//Going through more math shows that the error will dicrease by x * dy
	//on each iteration. So if x*dy < .5 initially, then the error will go as
	//1/2 -> 1/4 -> 1/16 -> 1/256, or the log2 will go as
	//-1 -> -2 -> -4 -> -8 ... So the iteration count needed to reach a
	//precision of (2^32)^n is log2(log2((2^32)^n)) = log2(32*n)
	//=log2(32) + log2(n) = 5 + log2(n) rounded up. This rounding may be done
	//by changing to 6 + log2(n - 1) rounded down
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

highPrec highPrec::operator-() const{
	highPrec temp(*this);
	temp.negate();
	return temp;
}

highPrec& highPrec::removeInt()
{
	while (exponent >= 0 && sigFigs.size() > 1)
	{
		sigFigs.erase(sigFigs.begin());
		exponent--;
	}
	if (exponent >= 0)
	{
		sigFigs[0] = 0;
		negative = false;
		exponent = 0;
	}
	return *this;
}

double highPrec::toDouble() const{
	highPrec temp(*this);
	temp.prune();
	ull outputData = 0ull;
	bool found = false;
	for (int i = 0; i < temp.sigFigs.size(); i++)
	{
		outputData += temp.sigFigs[i];
		if (found) break;
		found = true;
		outputData <<= 32;
	}
	int offsetCount = 0;
	while ((outputData >> 63) == 0)
	{
		outputData <<= 1;
		offsetCount++;
		if (offsetCount == 64) break;
	}
	outputData <<= 1;//remove implied bit
	outputData >>= 12;//reposition significant bits
	ull exp = 1023 + temp.exponent * 32 + 31;
	exp -= offsetCount;
	exp <<= 53;//remove left bit
	exp >>= 1;//reposition
	outputData += exp;
	if (negative) outputData += 1ull << 63;//add sign bit;
	return *(double*)(void*)&outputData;
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

unsigned int operator%(const highPrec& base, const unsigned int& other)
{
	highPrec temp(base);
	temp.forcePrec(temp.sigFigs.size() + 2);
	temp /= other;
	if (temp.exponent < 0)
	{
		if (base < 0)
		{
			return (unsigned int)(base.toDouble() + other + 0.5);
		}
		else
		{
			return (unsigned int)(base.toDouble() + 0.5);
		}
	}
	if (temp.sigFigs.size() - 1 <= temp.exponent) return 0;
	temp.removeInt();
	temp *= other;
	if (base < 0)
	{
		return (unsigned int)(temp.toDouble() + other + 0.5);
	}
	else
	{
		return (unsigned int)(temp.toDouble() + 0.5);
	}
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
