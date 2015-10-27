#ifndef SCAN_OPERATOR_H
#define SCAN_OPERATOR_H

#include "Operator.h"

class ScanOperator : public Operator {
public:
	ScanOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
	void consume();
	void produce();
};

#endif