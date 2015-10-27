#ifndef PRINT_OPERATOR_H
#define PRINT_OPERATOR_H

#include "Operator.h"

class PrintOperator : public Operator {
public:
	PrintOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
	void consume();
	void produce();
};

#endif