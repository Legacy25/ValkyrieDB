#include "Parser.h"

Operator* Parser::parseJson(std::string json){
	rapidjson::Document doc;
	doc.Parse(json);
	if(!document.isObject())
		return NULL;
	Operator *root = createTree(document);
	return root;
}

Operator* Parser::createTree(rapidjson::Value node){
	std::string type = node["TYPE"].GetString();
	std::vector<std::string> expressions;
	Value expression = node["EXPRESSION"];
	for(Value::ConstValueIterator it = expression.begin(); it != expression.end(); it++)
		children.push_back(it->GetString());
	std::vector<Operator*> children;
	for(Value::ConstValueIterator it = children.begin(); it != children.end(); it++){
		Operator *c = createTree(*it);
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