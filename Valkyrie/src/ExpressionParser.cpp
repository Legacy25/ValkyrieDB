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
	arithmeticOperators.insert("=");
	logicalOperators.insert(">");
}

BinaryExpression* ExpressionParser::logicalExpression(std::string s){
	if(s == ">")
		return new GreaterThanExpression();
	return NULL;
}

BinaryExpression* ExpressionParser::arithemeticExpression(std::string s){
	if(s == "=")
		return new EqualExpression();
	else if(s == "+")
		return new AdditionExpression();
	else if(s == "-")
		return new SubtractionExpression();
	return new MultiplicationExpression();
}

Expression* ExpressionParser::leafExpression(std::string s){
	std::cout << "lead : " << s << std::endl;
	if(s == "*"){
		return NULL;
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

//only arithemetic operators done
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
				op = arithemeticExpression(tokens[i]);
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
		std::cout << "Expressojtype: " << e->getType() << "left " << ((BinaryExpression*)e)->getLeftExpression()->getType()
		 << "right " << ((BinaryExpression*)e)->getRightExpression()->getType() << std::endl;
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
