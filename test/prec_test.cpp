#include <iostream>
#include <string>

#include "highPrec.h"

using namespace std;

#define print(input) cout << input << endl;

int main()
{
	double testNumber1 = 1.234567891;
	double testNumber2 = 3.943157204e-10;

	highPrec test1(testNumber1);
	highPrec test2(testNumber2);
	highPrec test3({testNumber1, testNumber2});
	// print(test1.toString());
	// print(test2.toString());
	// print((test1 + test2).toString());
	// print(test3.toString());
	string temp("1.2345678913943157204083498570");
	highPrec test4(temp, 50);
	print(temp);
	print(test4.toString());
}