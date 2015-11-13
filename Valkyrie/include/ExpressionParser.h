#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <string>
#include <unordered_set>
#include <vector>
#include "Expression.h"

namespace valkyrie{
	class ExpressionParser {
	private:
		std::unordered_set<std::string> logicalOperators, arithmeticOperators;
		std::vector<std::string> splitString(const std::string s, char delimiter);
		bool isDouble(std::string s);
		bool isLong(std::string s);
		bool isDate(std::string s);
		bool isString(std::string s);
		long extractLong(std::string s);
		double extractDouble(std::string s);
		valkyrie::Expression* leafExpression(std::string s);
		valkyrie::BinaryExpression* logicalExpression(std::string s);
		//TODO change to Expression
		valkyrie::BinaryExpression* arithemeticExpression(std::string s);
		valkyrie::Expression* parseExpression(std::string s);
	public:
		ExpressionParser();
		std::vector<valkyrie::Expression*> parse(std::string exp);
	};
}

#endif