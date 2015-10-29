#ifndef PRINT_OPERATOR_H
#define PRINT_OPERATOR_H

#include "Operator.h"

namespace valkyrie{
	class PrintOperator : public Operator {
		int* types;
	public:
		PrintOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
		void consume();
		void produce();
	};
}

#endif