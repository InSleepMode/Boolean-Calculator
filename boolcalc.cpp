#include <iostream>
#include <fstream>
#include "boolexpr.h"



int main(int argc, char* argv[]) {
	
	std::string arg = argv[1];

	if (arg == "-h" || arg == "?") {
		BooleanExpression::help();
		return 0;
	}

	std::ifstream inFile(argv[2]);
	if (!inFile) {
		std::cerr << "Could not open inputFile" << std::endl;
		return 1;
	}
	
	std::ofstream outFile(argv[3]);
	if (!inFile) {
		std::cerr << "Could not write into outputFile" << std::endl;
		return 1;
	}

	

	if (arg == "-table" || arg == "-cnf" || arg == "-dnf" || arg == "-zh") {

		std::string line;


		while (std::getline(inFile, line)) {

			try {

				BooleanExpression expr(line.c_str());

				if (arg == "-table") {
					outFile << expr.table() << std::endl;
				}
				else if (arg == "-cnf") {
					outFile << expr.cnf() << std::endl;
				}
				else if (arg == "-dnf") {
					outFile << expr.dnf() << std::endl;
				}
				else if (arg == "-zh") {
					outFile << expr.zhegalkin() << std::endl;
				}
			
			}
			catch (const std::exception& e) {
				outFile << "error" << std::endl;
				return 0;
			}

		}

		
	}

	else if (arg == "-isfull") {

		std::vector<BooleanExpression> someFunctions;
		std::string line;

		while (std::getline(inFile, line)) {
			
			try {
				BooleanExpression expr(line.c_str());
				someFunctions.push_back(expr);
			}
			catch (const std::exception& e) {
				outFile << "error" << std::endl;
				return 0;

			}
		}
		bool resOfIsFullFuction = isFullSystem(someFunctions);

		if (!resOfIsFullFuction) {
			outFile << "no" << std::endl;
		}
		else {
			outFile << "yes" << std::endl;
		}
		inFile.close();
		outFile.close();
		return 0;
	}

	inFile.close();
	outFile.close();
	return 0;
}
