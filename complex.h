#pragma once
#include "highPrec.h"


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
	double real() const{
		return a;
	}
	double imag() const{
		return b;
	}
	complex& operator+=(const complex& other){
		a += other.a;
		b += other.b;
		return *this;
	}
	complex& operator-=(const complex& other){
		a -= other.a;
		b -= other.b;
		return *this;
	}
	complex& operator*=(const complex& other){
		double temp = a * (other.a) - b * (other.b);
		b = a * (other.b) + b * (other.a);
		a = temp;
		return *this;
	}
	complex& operator*=(double other){
		a *= other;
		b *= other;
		return *this;
	}
	complex conjugate(){
		return complex(a, -b);
	}
	double magnitudeSquared() const{
		return a * a + b * b;
	}
	double magnitude() const{
		return sqrt(magnitudeSquared());
	}
	string toString() const{
		return to_string(a) + " + i" + to_string(b);
	}
};

complex operator+(complex base, const complex& other){
	return base += other;
}

complex operator-(complex base, const complex& other){
	return base -= other;
}

complex operator*(complex base, const complex& other){
	return base *= other;
}

complex operator*(complex base, double other){
	return base *= other;
}
