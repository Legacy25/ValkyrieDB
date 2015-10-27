#ifndef PROJECTION_OPERATOR_H
#define PROJECTION_OPERATOR_H

#include "Operator.h"

class ProjectionOperator : public Operator {
public:
	ProjectionOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
	void consume();
	void produce();
};

#endif