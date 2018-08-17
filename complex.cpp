#include <math.h>
#include <string>

class complex{
	double a;
	double b;
public:
	complex(double real, double imag){
		a = real;
		b = imag;
	}
	complex(double real){
		a = real;
		b = 0;
	}
	complex(){
		a = 0;
		b = 0;
	}
	double real(){
		return a;
	}
	double imag(){
		return b;
	}
	complex& operator+=(const complex& other){
		a += other.a;
		b += other.b;
	}
	friend complex operator+(complex base, const complex& other);
	complex& operator*=(const complex& other){
		double temp = a * (other.a) - b * (other.b);
		b = a * (other.b) + b * (other.a);
		a = temp;
	}
	friend complex operator*(complex base, const complex& other);
	complex conjugate(){
		return complex(a, -b);
	}
	double magnitudeSquared(){
		return a * a + b * b;
	}
	double magnitude(){
		return sqrt(magnitudeSquared());
	}
	string toString(){
		return to_string(a) + " + i" + to_string(b);
	}
};

complex operator+(complex base, const complex& other){
	base += other;
	return base;
}

complex operator*(complex base, const complex& other){
	base *= other;
	return base;
}
