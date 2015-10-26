#include <iostream>
#include <string>

#include "Parser.h"

using namespace std;

int main(int argc, char** argv){
	string json;
	cin >> json;
	cout << "parsed json: " << json << endl;	

	//parse json
	Parser parser;
	Operator *root = parser.parseJson(json);

	cout << root->toString() << endl;
	//create tree

}