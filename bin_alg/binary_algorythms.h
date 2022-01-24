#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include "settings.h"

using namespace std;

int p = 9, q = 5, BIAS = (1 << (p-1))-1; //for IEEE754, p - bits in characteristic, q - bits in matissa
static int global_of = 0; //transfer to next digit

vector<int> operator<<(vector<int> a, int shift) {

	if (shift > 0) {

		for (int i=a.size()-1; i>=shift; --i)
			a[i] = a[i-shift];
		for (int i=shift-1; i>=0; --i)
			a[i] = 0;

	} else {

		shift = -shift;
		for (int i=0; i<a.size()-shift; ++i)
			a[i] = a[i+shift];
		for (int i=a.size()-shift; i<a.size(); ++i)
			a[i] = 0;

	}

	return a;
}

vector<int> vector_sum(vector<int> a, vector<int> b, int &of=global_of, int base=10) {
	if (a.size() < b.size())
		a.swap(b);

	int shift=0, sum=0;
	of = 0;
	vector<int> res(a.size(), 0);

	shift = a.size() - b.size();
	for (int i=a.size()-1; i>=shift; --i){
		sum = a[i] + b[i-shift] + of;
		res[i] = sum%base;
		of = sum/base;
	}

	for (int i=shift-1; i>=0; --i){
		sum = a[i] + of;
		res[i] = sum%base;
		of = sum/base;
	}

	return res;
}

vector<int> unsign_bin_sum(vector<int> a, vector<int> b, int &of=global_of) {
	return vector_sum(a, b, of, 2);
}

vector<int> sign_bin_sum(vector<int> a, vector<int> b, int &of=global_of) {
	if (a.size() < b.size())
		a.swap(b);

	int shift=0, sum=0;
	of = 0;
	vector<int> res(a.size(), 0);

	shift = a.size() - b.size();
	res = unsign_bin_sum(a, b, of);
	for (int i=shift-1; i>=0; --i){
		sum = res[i] + b[0] + of;
		res[i] = sum%2;
		of = sum/2;
	}

	return res;
}

void bin_invertion(vector<int> &number) {
	vector<int> invertion(2, 0);
	invertion[0] = 1;
	//invertion[1] = 0;
	for (int i=0; i<number.size(); ++i)
		number[i] = invertion[number[i]];
}

void addition_code(vector<int> &number) {
	bin_invertion(number);
	number = unsign_bin_sum(number, {0, 1});
}

void straight_code(vector<int> &number) {
	number = sign_bin_sum(number, {1, 1});
	bin_invertion(number);
}

vector<int> dec_to_bin(int number, int dim=0) {
	vector<int> bin(dim+1, 0);
	int sign = 1 + (number/abs(number));
	number = abs(number);

	if (dim != 0){

		for (int i=0; i<dim; ++i){
			bin[dim-1-i] = number%2;
			number /= 2;
		}
		bin[dim] = number;

	} else {

		bin.pop_back();
		while (number > 0){
			bin.push_back(number%2);
			number /= 2;
		}
		reverse(bin.begin(), bin.end());
	}

	if (!sign) {
		addition_code(bin);
	}

	return bin;
}

int bin_to_dec(vector<int> number, bool sign=0) {
	int res=0, s=1;
	if (number[0] && sign) {
		straight_code(number);
		s = -1;
	}
	for (int i=sign; i<number.size(); ++i)
		res += number[i] << (number.size()-1-i);
	
	return s*res;
}

vector<int> sign_bin_sum(vector<int> a, int b, int start=0) {
	return sign_bin_sum(a, dec_to_bin(b, 0));
}

class IEEE754{

	private:
	vector<int> n = vector<int> (p+q+2, 0);

	public:
	bool CF = 0;

	int exp_k() {
		int res=0;
		for (int i=0; i<p; ++i){
			res += n[i+1] << (p-i-1);
		}
		return (res - BIAS);
	}

	void characteristic(int chrctr) {
		vector <int> interim_bin;

		interim_bin = dec_to_bin(chrctr, p);
		if (interim_bin[p] != 0){
			CF = 1;
		}
		for (int i=0; i<p; ++i){
				n[i+1] = interim_bin[i];
		}
	}

	double mantisa() {
		long double res=n[p+1]*DIRECT_MANTISA_CALCULATION, exp=1;
		for (int i=0; i<q; ++i){
			if (DIRECT_MANTISA_CALCULATION){
				exp /= 2.0;
				res += n[i+p+2]*exp;
			} else {
				res += n[i+p+2] << (q-i-1);
			}
		}
		if (!DIRECT_MANTISA_CALCULATION) res = n[p+1] + (res / pow(2, q));
		return res;
	}

	vector<int> mantisa_bin() {
		vector<int> res(q+1, 0);
		res[0] = 0;

		for (int i=1; i<=q; ++i)
			res[i] = n[p+1+i];

		if (n[0])
			addition_code(res);
		return res;
	}

	void mantisa(vector<int> m) {
		if (m.size() > (q+1))
			CF = 1;

		n[0] = m[0];
		if (n[0])
			straight_code(m);

		for (int i=0; i<q; ++i) 
			n[p+2+i] = m[i+1];
	}

	double to_double() {
		double exp=1, mant=0, number=0;
		int chrctr=0;

		mant = mantisa();
		chrctr = exp_k();

		if (chrctr == ((1 << (p+1))-1)){
			if (mant != 0){
				number = NAN;
				return number;
			} else {
				if (n[0]){
					number = -INFINITY;
					return number;
				} else {
					number = INFINITY;
					return number;
				}
			}
		}

		number = mant;
		
		if (chrctr < 0){
			chrctr *= -1;
			number *= pow(2,chrctr);
		} else {
			number /= pow(2,chrctr);
		}

		if (n[0] ^ !number) number = -number;

		return number;
	}

	void input(double number) {
		int k = 0;
		CF = 0;

		if (number == 0){
			return;
		}

		if (number > 0){
			n[0] = 0;
		} else {
			n[0] = 1;
			number *= -1;
		}
		
		while (number >= 2){
			number /= 2.0;
			k -= 1;
		}
		while (number < 1){
			number *= 2.0;
			k += 1;
		}

		number -= 1;
		n[p+1] = 1; //мнимая единица

		characteristic(k + BIAS);

		for (int i=0; i < q; ++i){
			number *= 2;
			if (number >= 1){
				n[p+2+i] = 1;
				number -= 1;
			} else {
				n[p+2+i] = 0;
			}
		}
	}

	void input(vector<int> number) {
		CF = 0;
		if (number.size() == (p+q+2)){
			n = number;
		} else {
			for (int i=0; i<p+q+2; ++i){
				if (i<number.size()){
					n[i] = number[i];
				} else n[i] = 0;
			}
		}
	}

	void print() {
		cout << n[0] << " ";
		for (int i=0; i<p; ++i){
			cout << n[i+1];
		}
		cout << " " << n[p+1] << " ";
		for (int i=0; i<q; ++i){
			cout << n[p+2+i];
		}
		cout << "\n";
		cout << "\t" << to_double() << "\n";
	}

	IEEE754 operator+(IEEE754 other) {
		IEEE754 res;
		res.input(to_double() + other.to_double());
		return res;
	}

	IEEE754 operator+(double other) {
		IEEE754 res;
		res.input(to_double() + other);
		return res;
	}

	IEEE754 operator-(IEEE754 other) {
		IEEE754 res;
		res.input(to_double() - other.to_double());
		return res;
	}

	IEEE754 operator-(double other) {
		IEEE754 res;
		res.input(to_double() - other);
		return res;
	}

	IEEE754 operator*(IEEE754 other) {
		IEEE754 res;
		res.input(to_double() * other.to_double());
		return res;
	}

	IEEE754 operator/(IEEE754 other) {
		IEEE754 res;
		res.input(to_double() / other.to_double());
		return res;
	}
};

IEEE754 log(IEEE754 a) {
	IEEE754 res;
	res.input(log(a.to_double()));
	return res;
}
