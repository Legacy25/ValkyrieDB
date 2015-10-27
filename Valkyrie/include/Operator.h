#ifndef OPERATOR_H
#define OPERATOR_H

#include <vector>
#include <string>
#include <stack>
#include <algorithm>

#include "Tuple.h"

class Operator {
protected:
	std::string type;
	std::vector<std::string> expressions;
	std::vector<Operator*> children;
public:
	std::string queryPlan();
	std::string toString();
	std::vector<Operator*> getChildren();
	//LLVM code generation
	virtual void consume() = 0;
	virtual void produce() = 0;
};

#endif