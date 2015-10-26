#include "Parser.h"

Operator* Parser::parseJson(std::string json){
	rapidjson::Document doc;
	doc.Parse(json.c_str());
	Operator *root = createTree(doc);
	return root;
}

Operator* Parser::createTree(rapidjson::Value node){
	std::string type = node["TYPE"].GetString();
	std::vector<std::string> expressions;
	const rapidjson::Value& expression = node["EXPRESSION"];
	const rapidjson::Value& src = node["SRC"];
	for(rapidjson::Value::ConstValueIterator it = expression.Begin(); it != expression.End(); it++)
		expressions.push_back(it->GetString());
	std::vector<Operator*> children;
	for(rapidjson::Value::ConstValueIterator it = src.Begin(); it != src.End(); it++){
		Operator *c = createTree(it->GetObject());
		children.push_back(c);
	}
	Operator *op = NULL;
	switch(type){
		case "SELECT":
			op = new SelectOperator(expressions, children);
			break;
		case "PROJECT":
			op = new ProjectionOperator(expressions, children);
			break;
		default:
			break;
	}
	return op;
}