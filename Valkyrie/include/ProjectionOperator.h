#ifndef PROJECTION_OPERATOR_H
#define PROJECTION_OPERATOR_H

#include "Operator.h"
#include "Expression.h"

namespace valkyrie{
	class ProjectionOperator : public Operator {
	private:
		std::vector<Expression*> projectionClauses;
	public:
		ProjectionOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
		void consume();
		void produce();
	};
}

#endif