#ifndef OPERATOR_H
#define OPERATOR_H

#include <vector>
#include <string>

#include "Tuple.h"

class Operator {
protected:
	std::string type;
	std::vector<std::string> expressions;
	std::vector<Operator*> children;
public:
	std::string toString();
	std::vector<Operator*> getChildren();
	//LLVM code generation
	virtual void consume(Tuple tuple) = 0;
	virtual Tuple produce() = 0;
};

#endif