#ifndef SCAN_OPERATOR_H
#define SCAN_OPERATOR_H

#include "Operator.h"

namespace valkyrie{
	class ScanOperator : public Operator {
	private:
		void convertToSchema();
	public:
		ScanOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
		~ScanOperator();
		void consume();
		void produce();
	};
}

#endif