#include <iostream>
#include <string>

#include "Parser.h"

using namespace std;

int main(int argc, char** argv){
	string json;
	getline(cin, json);

	Parser parser;
	Operator *root = parser.parseJson(json);

	cout << root->queryPlan() << endl;
}