#ifndef OPERATOR_H
#define OPERATOR_H

class Operator {
public:
	virtual void consume() = 0;
	virtual Tuple produce() = 0;
};

#endif