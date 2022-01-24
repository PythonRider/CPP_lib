#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include "binary_algorythms.h"

using namespace std;

class FPU{
	//with stack registers
	private:
	vector <IEEE754> stack = vector<IEEE754> (8);

	void inc_register_count() {
		if (register_count < 8){
			register_count += 1;
		} else{
			SO = 1;
		}
	}

	void dec_register_count() {
		if (register_count > 0){
			register_count -= 1;
		} else{
			SO = 1;
		}
	}

	public:
	bool 	SO = 0 /*stack overflow flag*/,\
			OF = 0 /*overflow flag, if number overflows variable*/,\
			SF = 0 /*sign flag, 1 if result of operation < 0*/,\
			ZF = 0 /*zero flag, if result of operation = 0*/;
	int register_count = 0;

	void swap(int i, int j) {
		IEEE754 a = stack[i];
		stack[i] = stack[j];
		stack[j] = a;
	}

	void push(double number) {
		inc_register_count();
		stack[register_count-1].input(number);
	}

	void push(vector<int> number) {
		inc_register_count();
		stack[register_count-1].input(number);
	}

	void push(IEEE754 number) {
		inc_register_count();
		stack[register_count-1] = number;
	}

	IEEE754 pop() {
		dec_register_count();
		return stack[register_count];
	}

	void input_file(string file_name) {
		double number;
		ifstream file(file_name);
		while (file >> number){
        	push(number);
    	}
		file.close();
	}

	void input_ieee754(string file_name) {
		int a=0, j=0;
		vector<int> number(p+q+2, 0);
		string line;
		ifstream file(file_name);
		while (!file.eof()){
			j = 0;
            getline(file, line);
			for (int i=0; i<p+q+2; ++i){
				a = line[j] - '0';
				while ( (a < 0) || (a > 1)){
					++j;
					a = line[j] - '0';
				}
				number[i] = a;
				++j;
			}
			push(number);
        }
		file.close();
	}

	void add() {
		push(pop() + pop());
	}

	void operation() {
		IEEE754 x, y;
		cout << "Doing operation ln(y-x)/(x+3) in 6 'ticks'.\n"\
		"x from ST(0) and y from ST(1), result will be pushed to stack - ST(0)\n";
		x = pop();
		y = pop();
		cout << "x = " << x.to_double() << "\ty = " << y.to_double() << "\n";
		push(log(y-x));
		push(x);
		push(pop() + 3);
		push((1/pop().to_double()) * pop().to_double());
		cout << "ln(y-x)/(x+3) = " << log(y.to_double()-x.to_double())/(x.to_double()+3) << "\n";
		cout << "===========================\n";		
	}

	void output() {
		cout << "Current flags:\nSO: " << SO << "\tCF: " << OF << "\n";
		cout << "Current stack:\n";
		for (int i=1; i<=register_count; ++i){
			cout << "ST(" << i-1 << ") = ";
			stack[register_count-i].print();
		}
		cout << "===========================\n";
	}
};

int main() {
	FPU model;
	IEEE754 a, b;

	model.input_file("input_doubles.txt");
	model.input_ieee754("input_ieee754.txt");
	model.output();
	model.operation();
	model.output();

	system("PAUSE");

	return 0;
}