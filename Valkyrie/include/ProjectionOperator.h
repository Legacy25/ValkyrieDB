#ifndef PROJECTION_OPERATOR_H
#define PROJECTION_OPERATOR_H

#include <unordered_map>
#include "Operator.h"
#include "Expression.h"

namespace valkyrie{
	class ProjectionOperator : public Operator {
	private:
		std::vector<Expression*> projectionClauses;
		void updateSchema();
		void updateExpression(valkyrie::Expression* newExp, unordered_map<std::string, valkyrie::Expression*> m, string tableName);
	public:
		ProjectionOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
		void consume();
		void produce();
	};
}

#endif