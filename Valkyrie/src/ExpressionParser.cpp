#include "../include/ExpressionParser.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <string.h>

using namespace valkyrie;

static void raise_exception() {
	cout << "Parse Exception!" << endl;
	exit(-1);
}

ExpressionParser::ExpressionParser(){
	arithmeticOperators.insert("+");
	arithmeticOperators.insert("-");
	arithmeticOperators.insert("*");
	arithmeticOperators.insert("/");
	relationalOperators.insert("=");
	relationalOperators.insert("<>");
	relationalOperators.insert(">");
	relationalOperators.insert(">=");
	relationalOperators.insert("<");
	relationalOperators.insert("<=");
	logicalOperators.insert("AND");
	logicalOperators.insert("OR");
}

BinaryExpression* ExpressionParser::binaryExpression(std::string s){
	if(s == "=")
		return new EqualExpression();
	if(s == "<>")
		return new NotEqualExpression();
	if(s == ">")
		return new GreaterThanExpression();
	if(s == ">=")
		return new GreaterThanEqualExpression();
	if(s == "<")
		return new LessThanExpression();
	if(s == "<=")
		return new LessThanEqualExpression();
	if(s == "AND")
		return new AndExpression();
	if(s == "OR")
		return new OrExpression();
	if(s == "+")
		return new AdditionExpression();
	if(s == "-")
		return new SubtractionExpression();
	if(s == "*")
		return new MultiplicationExpression();
	if(s == "/")
		return new DivisionExpression();
	raise_exception();
	return NULL;
}

Expression* ExpressionParser::leafExpression(std::string s){
	if(s[0] == '\'' && s[s.length()-1] == '\''){
		char* ns = new char[s.length()-1];
		std::copy(s.begin()+1, s.end()-1, ns);
		ns[s.length()-2] = '\0';
		return new StringValueExpression(ns);
		//add starts with date
		//string expression
	}else if(isLong(s)){
		return new LongValueExpression(extractLong(s));
	}else if(isDouble(s)){
		return new DoubleValueExpression(extractDouble(s));
	}else{
		return new ColExpression(s);
	}
	raise_exception();
	return NULL;
}

Expression* ExpressionParser::parseExpression(std::vector<std::string> tokens){
	if(tokens.size() == 1)
		return leafExpression(tokens[0]);
	if(tokens.size() > 3 && tokens[tokens.size()-2] == "AS")
		return parseExpression(std::vector<std::string>(tokens.begin(), tokens.end()-2));
	for(int i = 0; i < tokens.size(); i++){
		if(logicalOperators.find(tokens[i]) != logicalOperators.end()){
			Expression *left = parseExpression(std::vector<std::string>(tokens.begin(), tokens.begin()+i));
			Expression *right = parseExpression(std::vector<std::string>(tokens.begin()+i+1, tokens.end()));
			BinaryExpression *op = NULL;
			op = binaryExpression(tokens[i]);
			op->setLeftExpression(left);
			op->setRightExpression(right);
			return op;
		}
	}
	for(int i = 0; i < tokens.size(); i++){
		if(relationalOperators.find(tokens[i]) != relationalOperators.end()){
			Expression *left = parseExpression(std::vector<std::string>(tokens.begin(), tokens.begin()+i));
			Expression *right = parseExpression(std::vector<std::string>(tokens.begin()+i+1, tokens.end()));
			BinaryExpression *op = NULL;
			op = binaryExpression(tokens[i]);
			op->setLeftExpression(left);
			op->setRightExpression(right);
			return op;
		}
	}
	for(int i = 0; i < tokens.size(); i++){
		if(arithmeticOperators.find(tokens[i]) != arithmeticOperators.end()){
			Expression *left = parseExpression(std::vector<std::string>(tokens.begin(), tokens.begin()+i));
			Expression *right = parseExpression(std::vector<std::string>(tokens.begin()+i+1, tokens.end()));
			BinaryExpression *op = NULL;
			op = binaryExpression(tokens[i]);
			op->setLeftExpression(left);
			op->setRightExpression(right);
			return op;
		}
	}
	raise_exception();
	return NULL;
}

std::vector<Expression*> ExpressionParser::parse(std::string exp){
	std::vector<std::string> tokens = splitString(exp, ',');
	std::vector<Expression*> exps;
	for(int i = 0; i < tokens.size(); i++){
		if(tokens[i] == "")
			continue;
		//trim expression string
		std::vector<std::string> expTokens = splitString(exp, ' ');
		Expression* e = parseExpression(expTokens);
		exps.push_back(e);
	}
	return exps;
}

//Shift to a utilities class
std::vector<std::string> ExpressionParser::splitString(const std::string s, char delimiter) {
	std::vector<string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delimiter))
		elems.push_back(item);
	return elems;
}

bool ExpressionParser::isDouble(std::string s){
	char* endptr = 0;
    std::strtod(s.c_str(), &endptr);
    if(*endptr != '\0' || endptr == s)
        return false;
    return true;
}

bool ExpressionParser::isLong(std::string s){
	for(int i = 0; i < s.length(); i++)
		if(s[i] < '0' || s[i] > '9')
			return false;
	return true;
}
	
bool ExpressionParser::isDate(std::string s){
	//TODO
	return true;
}

bool ExpressionParser::isString(std::string s){
	return s[0] == '\'' && s[s.length()-1] == '\'';
}

long ExpressionParser::extractLong(std::string s){
	return std::atoi(s.c_str());
}

double ExpressionParser::extractDouble(std::string s){
	return std::atof(s.c_str());
}
