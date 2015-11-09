#ifndef OPERATOR_H
#define OPERATOR_H

#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <iostream>

#include "DataTypes.h"
#include "Schema.h"

namespace valkyrie{

	class Operator {
	protected:
		std::string type;
		valkyrie::Schema *schema;
		std::vector<std::string> expressions;
		Operator* parent;
		std::vector<Operator*> children;
		DataType mapType(std::string type);

	public:
		Operator(std::vector<Operator*> children);
		~Operator();
		valkyrie::Schema* getSchema();
		void setParent(Operator* parent);
		std::string queryPlan();
		std::string toString();
		std::vector<Operator*> getChildren();

	//LLVM code generation
		virtual void consume() = 0;
		virtual void produce() = 0;
	};
}

#endif