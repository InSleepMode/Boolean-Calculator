#include <iostream>
#include <fstream>
#include "boolexpr.h"
#include <stack>
#include <set>
#include <algorithm> 

int prioritise(char op) {

	switch (op) {
		case '~': return 5;
		case '&': return 4;
		case '|': 
		case '+': 
		case 'v': 
		case '^': return 3;
		case '>': 
		case '<': return 2;
		case '=': return 1;
	default: return 0;
	}
}

std::vector<Symbol> parcer(const std::string& expr) {
	std::vector<Symbol> result;

	for (int i = 0; i < expr.size(); ++i) {
		char ch = expr[i];

		if (ch == ' ') {
			continue;
		}

		if (ch == 'x') {
			int num = 0;
			++i;

			if (expr[i] == ' ') {
				throw std::invalid_argument("error"); //x 1 & x2
			}

			while (i < expr.size() && isdigit(expr[i]) != 0) {
				num = num * 10 + (expr[i] - '0');
				++i;
			}
			--i;
			result.push_back(Symbol('x', num));


		}
		else if (ch == '~') {
			result.push_back(Symbol('~'));
		}
		else if (ch == '&') {
			result.push_back(Symbol('&'));
		}
		else if (ch == 'v') {
			result.push_back(Symbol('v'));
		}
		else if (ch == '+') {
			result.push_back(Symbol('+'));
		}
		else if (ch == '>') {
			result.push_back(Symbol('>'));
		}
		else if (ch == '<') {
			result.push_back(Symbol('<'));
		}
		else if (ch == '=') {
			result.push_back(Symbol('='));
		}
		else if (ch == '|') {
			result.push_back(Symbol('|'));
		}
		else if (ch == '^') {
			result.push_back(Symbol('^'));
		}
		else if (ch == '0') {
			result.push_back(Symbol('0'));
		}
		else if (ch == '1') {
			result.push_back(Symbol('1'));
		}
		else if (ch == '(') {
			result.push_back(Symbol('('));
		}
		else if (ch == ')') {
			result.push_back(Symbol(')'));
		}
		else if (ch != ')' || ch != '(' || ch != '1' || ch != '0' || ch != '^' 
			  || ch != '~' || ch != '&' || ch != 'v' || ch != '+' || ch != '>' 
			  || ch != '=' || ch != '|' ) {

			throw std::invalid_argument("error");
		}
		
	}
	
	int cntOperations = 0;
	int cntVariables = 0;

	for (int i = 0; i < result.size(); ++i) {
		if (result[i].type == 'x' || result[i].type == '0' || result[i].type == '1') {
			cntVariables++;
		}
		else if (result[i].type == '&' || result[i].type == 'v' || result[i].type == '^' || result[i].type == 'v' || result[i].type == '+'
			|| result[i].type == '>' || result[i].type == '=' || result[i].type == '|' || result[i].type == '<') {
			cntOperations++;
		}
	}

	if (cntVariables != cntOperations + 1) {
		throw std::invalid_argument("error");
	}

	return result;
}

std::vector<Symbol> postfixNotation(std::vector<Symbol>& parcedVector) {
	std::stack<Symbol> operations;
	std::vector<Symbol> result;

	for (int i = 0; i < parcedVector.size(); ++i) {
		if (parcedVector[i].type == 'x' || parcedVector[i].type == '1' || parcedVector[i].type == '0') {
			result.push_back(parcedVector[i]);
		}
		else {
			if (parcedVector[i].type == '(') {
				operations.push(parcedVector[i]);
			}
			else if (parcedVector[i].type == ')') {
				while (!operations.empty() && operations.top().type != '(') {
					result.push_back(operations.top());
					operations.pop();
				}
				if (!operations.empty()) {
					operations.pop();
				}
			}
			else {
				while (!operations.empty() && operations.top().type != '(' &&
					prioritise(operations.top().type) >= prioritise(parcedVector[i].type)) {
					result.push_back(operations.top());
					operations.pop();
				}
				operations.push(parcedVector[i]);
			}
		}
	}

	while (!operations.empty()) {
		result.push_back(operations.top());
		operations.pop();
	}

	return result;
}

std::vector<int> getVars(const std::vector<Symbol>& postfix) {
	std::set<int> uniqueVars; //set для упорядоченного набора неповторяющихся переменных

	for (const Symbol& symbol : postfix) {
		if (symbol.type == 'x') {
			uniqueVars.insert(symbol.varNum);
		}
	}
	std::vector<int> variables(uniqueVars.begin(), uniqueVars.end());

	return variables;
}

bool calc(std::vector<Symbol>& postfix, std::vector<bool> values, const std::vector<int>& variables) {

	std::stack<bool> operands;

	for (const Symbol& symbol : postfix) {

		if (symbol.type == 'x') {
			int index = -1; 
			for (int i = 0; i < variables.size(); i++) {
				if (variables[i] == symbol.varNum) {
					index = i;
					break;
				}
			}

			operands.push(values[index]);
		}
		else if (symbol.type == '0') {
			operands.push(false);
		}
		else if (symbol.type == '1') {
			operands.push(true);
		}
		else if (symbol.type == '~') {
			bool x = operands.top();
			operands.pop();
			operands.push(!x);
		}
		else if (symbol.type == '&') {
			bool x1 = operands.top();
			operands.pop();
			bool x2 = operands.top();
			operands.pop();
			operands.push(x2 && x1);
		}
		else if (symbol.type == '|') {
			bool x1 = operands.top();
			operands.pop();
			bool x2 = operands.top();
			operands.pop();
			operands.push(!(x2 && x1));
		}
		else if (symbol.type == '>') {
			bool x1 = operands.top();
			operands.pop();
			bool x2 = operands.top();
			operands.pop();
			operands.push(!x2 || x1);
		}
		else if (symbol.type == '<') {
			bool x1 = operands.top();
			operands.pop();
			bool x2 = operands.top();
			operands.pop();
			operands.push(x2 || !x1);
		}
		else if (symbol.type == '=') {
			bool x1 = operands.top();
			operands.pop();
			bool x2 = operands.top();
			operands.pop();
			operands.push(x2 == x1);
		}
		else if (symbol.type == '+') {
			bool x1 = operands.top();
			operands.pop();
			bool x2 = operands.top();
			operands.pop();
			operands.push(x2 != x1);
		}
		else if (symbol.type == 'v') {
			bool x1 = operands.top();
			operands.pop();
			bool x2 = operands.top();
			operands.pop();
			operands.push(x2 || x1);
		}
		else if (symbol.type == '^') {
			bool x1 = operands.top();
			operands.pop();
			bool x2 = operands.top();
			operands.pop();
			operands.push(!(x2 || x1));
		}
	}

	return operands.top();
}

BooleanExpression::BooleanExpression(const char* expr) {
	initial = expr;

	std::vector<Symbol> symbols = parcer(expr);

	postfix = postfixNotation(symbols);

	variables = getVars(postfix);

}

std::string BooleanExpression::table() const {

	std::string result;
	int n = static_cast<int>(this->variables.size()); // 2^n
	std::vector<Symbol> postfix_vec = this->postfix;


	for (int i = 0; i < (1 << n); ++i) {
		std::vector<bool> combs;
		for (int j = 0; j < n; ++j) {
			int combination = (i >> (n - 1 - j)) & 1; //получаем бит для каждой переменной
			combs.push_back(combination != 0);
		}

		bool res = calc(postfix_vec, combs, this->variables);
		result += (res ? '1' : '0');
	}

	return result;
}

std::string BooleanExpression::dnf() const {

	int n = static_cast<int>(this->variables.size());
	std::string boolTable = this->table();
	if (n == 0) {
		if (boolTable == "0") {
			return "0";
		}
		return "1";
	}
	std::vector<std::string> expression_massive;

	std::string literal;

	for (int i = 0; i < boolTable.size(); ++i) {
		if (boolTable[i] == '1') {

			std::vector<std::string> dnf_vars;
			for (int j = 0; j < n; ++j) {
				int bit = (i >> (n - 1 - j)) & 1;
				int variable_number = this->variables[j];
				if (bit == 0) {
					literal += "~x" + std::to_string(variable_number);

				}
				else {
					literal += "x" + std::to_string(variable_number);
				}
				dnf_vars.push_back(literal);
				literal = "";
			}

			std::string expression;
			for (int k = 0; k < dnf_vars.size(); ++k) {
				expression += dnf_vars[k];

				if (k < dnf_vars.size() - 1) {
					expression += " & ";
				}

			}
			expression_massive.push_back(expression);

		}
	}

	std::string result;
	for (int i = 0; i < expression_massive.size(); ++i) {
		result += expression_massive[i];
		if (i < expression_massive.size() - 1) {
			result += " v ";
		}

	}

	return result;
}

std::string BooleanExpression::cnf() const {

	int n = static_cast<int>(this->variables.size());
	std::string boolTable = this->table();
	if (n == 0) {
		if (boolTable == "0") {
			return "0";
		}
		return "1";
		
	}
	char zero = '0';
	int count_zero = 0;

	for (char c : boolTable) {
		if (c == zero) {
			++count_zero;
		}
	}

	std::vector<std::string> expression_massive;

	std::string literal;

	for (int i = 0; i < boolTable.size(); ++i) {
		if (boolTable[i] == '0') {

			std::vector<std::string> dnf_vars;
			for (int j = 0; j < n; ++j) {
				int bit = (i >> (n - 1 - j)) & 1;
				int variable_number = variables[j];
				if (bit == 1) {
					literal += "~x" + std::to_string(variable_number);

				}
				else {
					literal += "x" + std::to_string(variable_number);
				}
				dnf_vars.push_back(literal);
				literal = "";
			}


			if (count_zero > 1) {

				std::string expression = "(";
				for (int k = 0; k < dnf_vars.size(); ++k) {
					expression += dnf_vars[k];

					if (k < dnf_vars.size() - 1) {
						expression += " v ";
					}
				}
				expression += ")";

				expression_massive.push_back(expression);

			}
			else {
				std::string expression;
				for (int k = 0; k < dnf_vars.size(); ++k) {
					expression += dnf_vars[k];

					if (k < dnf_vars.size() - 1) {
						expression += " v ";
					}

				}
				expression_massive.push_back(expression);
			}
		}
	}

	std::string result;
	for (int i = 0; i < expression_massive.size(); ++i) {
		result += expression_massive[i];
		if (i < expression_massive.size() - 1) {
			result += " & ";
		}

	}

	return result;
}

int cntVars(const std::string& expression) {
	int cnt = 0;
	for (int i = 0; i < expression.size(); ++i) {
		if (expression[i] == 'x') {
			++cnt;
		}
	}
	return cnt;
}

bool expressionSort(const std::string& a, const std::string& b) {

	if (a == "1") {
		return true;
	}

	if (b == "1") {
		return false;
	}

	int a_vars = cntVars(a);
	int b_vars = cntVars(b);

	if (a_vars < b_vars) {
		return true;
	}

	if (a_vars > b_vars) {
		return false;
	}

	return a < b;
}

std::string BooleanExpression::zhegalkin() const {

	std::string boolTable = this->table();
	std::vector<int> RowOfPascalTriangle;
	std::vector<int> firstIndexCol;

	RowOfPascalTriangle.reserve(boolTable.size());
	for (int i = 0; i < boolTable.size(); ++i) {
		RowOfPascalTriangle.push_back(boolTable[i] - '0');
	}

	firstIndexCol.push_back(RowOfPascalTriangle[0]);
	int n = static_cast<int>(this->variables.size());
	if (n == 0) {
		return boolTable;
	}
	while (RowOfPascalTriangle.size() > 1) {
		std::vector<int> tmpRow;
		for (int c = 0; c < RowOfPascalTriangle.size() - 1; ++c) {
			int indexElement = (RowOfPascalTriangle[c] + RowOfPascalTriangle[c+1]) % 2;
			tmpRow.push_back(indexElement);
		}
		firstIndexCol.push_back(tmpRow[0]);

		RowOfPascalTriangle.clear();
		
		for (int k = 0; k < tmpRow.size(); ++k) {
			RowOfPascalTriangle.push_back(tmpRow[k]);
		
		}
		tmpRow.clear();
	}

	std::vector<std::string> allExpressions;

	for (int i = 0; i < firstIndexCol.size(); ++i) {
		if (firstIndexCol[i] == 1) {
			std::vector<std::string> zhegalkin_vars;

			for (int j = 0; j < n; ++j) {
				if (((i >> (n - 1 - j)) & 1) == 1) {
					zhegalkin_vars.push_back("x" + std::to_string(variables[j]));
				}
			}

			std::string expression;
			if (zhegalkin_vars.empty()) {
				expression = "1";
			}
			else {
				for (int k = 0; k < zhegalkin_vars.size(); ++k) {
					expression += zhegalkin_vars[k];
					if (k < zhegalkin_vars.size() - 1) {
						expression += " & ";
					}
				}
			}

			allExpressions.push_back(expression);
		}
	}


	std::sort(allExpressions.begin(), allExpressions.end(), expressionSort);

	std::string result;
	for (int i = 0; i < allExpressions.size(); ++i) {
		result += allExpressions[i];
		if (i < allExpressions.size() - 1) {
			result += " + ";
		}
	}
	

	return result;
}



bool BooleanExpression::isSave0() const {
	std::string boolTable = this->table();
	return (boolTable[0] == '0');
}

bool BooleanExpression::isSave1() const {
	std::string boolTable = this->table();
	return (boolTable[boolTable.size() - 1] == '1');
}

bool BooleanExpression::selfDual() const {
	std::string boolTable = this->table();
	int j = 0;
	for (int i = 0; i < boolTable.size() / 2; ++i) {
		j = static_cast<int>((boolTable.size() - 1) - i);
		if (boolTable[i] == boolTable[j]) {
			return false;
		}
	}
	return true;
}

bool BooleanExpression::isMonotonous() const {
	std::string boolTable = this->table();
	for (int i = 0; i < boolTable.size(); ++i) {
		for (int j = i + 1; j < boolTable.size(); ++j) {
			if ((i & j) == i && boolTable[i] > boolTable[j]) {
				return false;
			}
		}
	}
	return true;
}

bool BooleanExpression::isLineal() const {

	std::string zhegalkinReadyFormula = this->zhegalkin();
	for (int i = 0; i < zhegalkinReadyFormula.size(); ++i) {
		if (zhegalkinReadyFormula[i] == '&') {
			return false;
		}
	}
	return true;
}

bool isFullSystem(const std::vector<BooleanExpression>& someFunctions) {

	bool save0 = true;
	bool save1 = true;
	bool self_dual = true;
	bool monotonous = true;
	bool lineal = true;

	for (const BooleanExpression& oneFunction : someFunctions) {

		if (!oneFunction.isSave0()) {
			save0 = false;
		}
		if (!oneFunction.isSave1()) {
			save1 = false;
		}
		if (!oneFunction.selfDual()) {
			self_dual = false;
		}
		if (!oneFunction.isMonotonous()) {
			monotonous = false;
		}
		if (!oneFunction.isLineal()) {
			lineal = false;
		}
	}

	return !save0 && !save1 && !self_dual && !monotonous && !lineal;
}

void BooleanExpression::help(){
	std::cout << "The boolcalc -table input output call builds a truth table for expressions written in the input file and writes the results to the output file (for each function, the result is written in a separate line)." << std::endl;
	std::cout << "The boolcalc -cnf input output call builds a conjunctive normal form for expressions written in the input file and writes the results to the output file (for each function, the result is written in a separate line)." << std::endl;
	std::cout << "The boolcalc -dnf input output call builds a disjunctive normal form for expressions written in the input file and writes the results to the output file (for each function, the result is written in a separate line)." << std::endl;
	std::cout << "The boolcalc -zh input output call builds the Zhegalkin polynomial for expressions written in the input file and writes the results to the output file (for each function, the result is written in a separate line)." << std::endl;
	std::cout << "The boolcalc -isfull input output call checks the function system for completeness. If the system of functions in the input file is complete, then yes is output to the output file, otherwise no." << std::endl;
}


