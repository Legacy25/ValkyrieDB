#ifndef SELECT_OPERATOR_H
#define SELECT_OPERATOR_H

#include "Operator.h"
#include "Expression.h"
#include "ExpressionParser.h"

namespace valkyrie{
	class SelectOperator : public Operator {
		Expression* selectClause;
		void updateExpression(Expression *newExp, unordered_map<string, Expression *> m, std::string tableName);
	public:
		SelectOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
		void consume();
		void produce();
	};
}

#endif