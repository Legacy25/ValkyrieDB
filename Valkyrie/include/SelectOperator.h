#ifndef SELECT_OPERATOR_H
#define SELECT_OPERATOR_H

#include "Operator.h"
#include "Expression.h"

namespace valkyrie{
	class SelectOperator : public Operator {
	public:
		SelectOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
		void consume();
		void produce();
	};
}

#endif