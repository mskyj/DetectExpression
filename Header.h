#include <sstream>
#include<iomanip>
#include<iostream>
using namespace::std;

char* chars = "0123456789-+*";

class Symbol{
public:
	char label;
	int left,right,top,bottom;
	int width,height;
	Symbol(char label,int l,int r,int t,int b):label(label),left(l),right(r),top(t),bottom(b){
		width = right - left;
		height = bottom - top;
	}

	bool operator<(const Symbol& operand){
		return left < operand.left;
	}

	friend std::ostream& operator<<(std::ostream& os, const Symbol& symbol);
};

std::ostream& operator<<(std::ostream& os, const Symbol& s){
	os << chars[s.label] << ": " << s.left << "," << s.right << ","  << s.top << ","  << s.bottom;
	return os;
};

class Express{
public:
	string str;
	int left,right,top,bottom;
	int fontSize;
	int width,height;
	int fractionFlag;
	int fractionUp;
	int fractionDown;
	bool partFlag;

	Express(Symbol s):left(s.left),right(s.right),top(s.top),bottom(s.bottom),
					  fontSize(s.height),width(s.width),height(s.height){
		str = chars[s.label];
		if( chars[s.label] == '-' )
			fractionFlag = true;
		else
			fractionFlag = false;
		fractionUp = -1;
		fractionDown = -1;
		partFlag = false;
	}

	bool isEx(Symbol s){
		if( chars[s.label] == '*' )
			return false;
		if( height*0.7 < s.height )
			return false;
		if( abs((s.top+s.bottom)/2-top) > height/3 )
			return false;
		return true;
	}

	bool isSymbol(Symbol s){
		if( s.label < 10 && (s.height > height*1.25 || s.height < height * 0.7) )
			return false;
		if( s.label > 9 && s.height > height*1.2 )
			return false;
		if( abs((s.top+s.bottom)/2-(top+bottom)/2) > height/3 )
			return false;
		if( (s.left - right) > height*1.5 )
			return false;
		return true;
	}

	bool isFraction(Symbol s){
		if( !fractionFlag )
			return false;
		int center = (bottom+top)/2;
		if( center - s.bottom > 0 ){
			if( center-s.bottom > s.height ){
				return false;
			}
		}
		else if( s.top - center > 0 ){
			if( s.top-center > s.height ){
				return false;
			}
		}
		else if( abs(center - s.top)>s.height/4 && abs(center-s.bottom)>s.height/4 )
			return false;
		return true;
	}

	void addEx(Symbol s, int expIndex){
		ostringstream stream;
		stream << "^(exp" << setfill('0') << setw(3) << expIndex << ")";
		str += stream.str();
		right = s.right;
		width = right - left;
	}

	void addSymbol(Symbol s){
		if( chars[s.label] == '-' )
			fractionFlag = true;
		else
			fractionFlag = false;
		str += chars[s.label];
		right = max(right,s.right);
		top = min(top,s.top);
		bottom = max(bottom,s.bottom);
		fontSize = bottom - top;
		width = right - left;
		height = bottom - top;
	}

	void addFraction(Symbol s, int expIndex){
		if( (bottom+top)/2 - (s.top+s.bottom)/2 > 0 )
			fractionUp = expIndex;
		else
			fractionDown = expIndex;
		if( fractionUp!=-1 && fractionDown!=-1 ){
			str.erase( --str.end() );
			ostringstream stream;
			stream << "(exp" << setfill('0') << setw(3) << fractionUp << ")/(exp" << setfill('0') << setw(3) << fractionDown << ")";
			str += stream.str();
			fractionUp = -1;
			fractionDown = -1;
			if( height < width )
				height = width;
		}
	}

	void updateSize(Express ex){
		left = min(left,ex.left);
		right = max(right,ex.right);
		top = min(top,ex.top);
		bottom = max(bottom,ex.bottom);
	}

	friend std::ostream& operator<<(std::ostream& os, const Express& ex);
};

std::ostream& operator<<(std::ostream& os, const Express& ex){
	os << ex.str;
	return os;
}

