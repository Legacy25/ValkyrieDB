#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "Operator.h"
#include "SelectOperator.h"
#include "ProjectionOperator.h"
#include "rapidjson/document.h"

class Parser {
private:
	Operator* createTree(rapidjson::Value doc);
public:
	Operator* parseJson(std::string json);
};

#endif