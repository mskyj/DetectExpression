#include"Calc.h"

string str;

double Factor();
double Expression();
double Formula();

bool isDigit(char c){
	if ( (unsigned int)c - '0' < 10)
		return true;
	else
		return false;
}

double Factor(){

	double value;
	if( str[0] == '(' ){
		str.erase(0,1);
		value = Formula();
		str.erase(0,1);
	}
	else if( isDigit(str[0]) ){
		int temp,temp2=0;
		do{
			temp = (int)(str[0]-'0');
			temp2 *= 10;
			temp2 += temp;
			str.erase(0,1);
		}while( str.size() > 0 && isDigit(str[0]) );
		value = (double)temp2;
	}
	return value;
}

double Expression(){

	double value = Factor();
	while( str.size() > 0 && (str[0] == '*' || str[0] == '/' || str[0] == '^') ){
		char operation = str[0];
		str.erase(0,1);
		double value2 = Factor();

		switch(operation){
		case '*':
			value *= value2;
			break;
		case '/':
			value /= value2;
			break;
		case '^':
			value = pow(value,value2);
		}
	}
	return value;
}

double Formula(){

	double value = Expression();
	while( str.size() > 0 && (str[0] == '+' || str[0] == '-') ){
		char operation = str[0];
		str.erase(0,1);
		double value2 = Expression();

		switch(operation){
		case '+':
			value += value2;
			break;
		case '-':
			value -= value2;
			break;
		}
	}
	return value;
}

double Calc(string s){
	str = s;
	return Formula();
}