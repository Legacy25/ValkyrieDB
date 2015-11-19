#include "../include/ExpressionParser.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

using namespace valkyrie;

ExpressionParser::ExpressionParser(){
	arithmeticOperators.insert("+");
	arithmeticOperators.insert("-");
	arithmeticOperators.insert("*");
	arithmeticOperators.insert("/");
	logicalOperators.insert("=");
	logicalOperators.insert("<>");
	logicalOperators.insert(">");
	logicalOperators.insert(">=");
	logicalOperators.insert("<");
	logicalOperators.insert("<=");
	logicalOperators.insert("AND");
	logicalOperators.insert("OR");
}

BinaryExpression* ExpressionParser::logicalExpression(std::string s){
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
	return NULL;
}

BinaryExpression* ExpressionParser::arithmeticExpression(std::string s){
	if(s == "+")
		return new AdditionExpression();
	if(s == "-")
		return new SubtractionExpression();
	if(s == "*")
		return new MultiplicationExpression();
	return new DivisionExpression();
}

Expression* ExpressionParser::leafExpression(std::string s){
	std::cout << "lead : " << s << std::endl;
	if(s == "*"){
		//TODO check this, replace for all columns
		return new ColExpression(s);
	}else if(s[0] == '\'' && s[s.length()-1] == '\''){
		return new StringValueExpression(&s);
		//add starts with date
		//string expression
	}else if(isLong(s)){
		std::cout << "int detected : " << std::endl;
		return new LongValueExpression(extractLong(s));
	}else if(isDouble(s)){
		std::cout << "double detected : " << std::endl;
		return new DoubleValueExpression(extractDouble(s));
	}else{
		std::cout << "colimn detected : " << std::endl;
		return new ColExpression(s);
	}
	return NULL;
}

Expression* ExpressionParser::parseExpression(std::string exp){
	std::cout << "string : " << exp << std::endl;
	std::vector<std::string> tokens = splitString(exp, ' ');
	std::cout << tokens.size() << std::endl;
	if(tokens.size() == 1)
		return leafExpression(tokens[0]);
	for(int i = 0; i < tokens.size(); i++){
		if(logicalOperators.find(tokens[i]) != logicalOperators.end() || 
			arithmeticOperators.find(tokens[i]) != arithmeticOperators.end()){
			std::cout << tokens[i] << std::endl;
			Expression *left = parseExpression(tokens[i-1]);
			Expression *right = parseExpression(tokens[i+1]);
			//TODO generalize to Expression
			BinaryExpression *op = NULL;
			if(arithmeticOperators.find(tokens[i]) != arithmeticOperators.end())
				op = arithmeticExpression(tokens[i]);
			else op = logicalExpression(tokens[i]);
			op->setLeftExpression(left);
			op->setRightExpression(right);
			return op;
		}
	}
	return NULL;
}

std::vector<Expression*> ExpressionParser::parse(std::string exp){
	std::vector<std::string> tokens = splitString(exp, ',');
	std::vector<Expression*> exps;
	for(int i = 0; i < tokens.size(); i++){
		if(tokens[i] == "")
			continue;
		//trim expression string
		Expression* e = parseExpression(tokens[i]);
		exps.push_back(e);
		//std::cout << "Expressiontype: " << e->getType() << "left " << ((BinaryExpression*)e)->getLeftExpression()->getType()
		 //<< "right " << ((BinaryExpression*)e)->getRightExpression()->getType() << std::endl;
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
	//check for 2 or more decimal points
	for(int i = 0; i < s.length(); i++)
		if((s[i] < '0' || s[i] > '9') && s[i] != '.')
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
