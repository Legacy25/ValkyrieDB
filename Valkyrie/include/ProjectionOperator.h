#ifndef PROJECTION_OPERATOR_H
#define PROJECTION_OPERATOR_H

#include "Operator.h"

class ProjectionOperator : protected Operator {
public:
	ProjectionOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
	void consume(Tuple tuple);
	Tuple produce();
};

#endif