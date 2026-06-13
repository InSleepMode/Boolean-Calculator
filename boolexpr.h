#ifndef BOOLEXPR_H
#define BOOLEXPR_H

#include <vector>
#include <string>

struct Symbol {
	
	char type;
	int varNum;
	
	Symbol(char t) { type = t; varNum = 0; } // для операций
	Symbol(char t, int num) { type = t; varNum = num; } // для переменных

};

class BooleanExpression {

private:
	std::string initial;
	std::vector<Symbol> postfix;
	std::vector<int> variables;

public:

	BooleanExpression(const char* expr);
	
	std::string table() const;
	std::string dnf() const;
	std::string cnf() const;
	std::string zhegalkin() const;
	static void help();

	bool isSave0() const;
	bool isSave1() const;
	bool selfDual() const;
	bool isMonotonous() const;
	bool isLineal() const;

	operator std::string() const { return initial; }

};

bool isFullSystem(const std::vector<BooleanExpression>& someFunctions);

#endif
