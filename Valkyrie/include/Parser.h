#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "Operator.h"
#include "PrintOperator.h"
#include "ScanOperator.h"
#include "SelectOperator.h"
#include "ProjectionOperator.h"
#include "JoinOperator.h"
#include "rapidjson/document.h"

namespace valkyrie{
	class Parser {
	private:
		Operator* createTree(const rapidjson::Value& node);
	public:
		Operator* parseJson(std::string json);
	};
}

#endif