#include "../include/Parser.h"

using namespace valkyrie;

Operator* Parser::parseJson(std::string json){
	rapidjson::Document doc;
	doc.Parse(json.c_str());
	Operator *tree = createTree(doc["SRC"]);
	std::vector<Operator*> src;
	src.push_back(tree);
	Operator *root = new PrintOperator(std::vector<std::string>(), src);
	return root;
}

Operator* Parser::createTree(const rapidjson::Value& node){
	if(node == NULL)
		return NULL;
	std::string type = node["TYPE"].GetString();
	std::vector<std::string> expressions;
	const rapidjson::Value& expression = node["EXPRESSION"];
	const rapidjson::Value& src = node["SRC"];
	for(rapidjson::Value::ConstValueIterator it = expression.Begin(); it != expression.End(); it++)
		expressions.push_back(it->GetString());
	std::vector<Operator*> children;
	for(rapidjson::Value::ConstValueIterator it = src.Begin(); it != src.End(); it++){
		Operator *c = createTree(*it);
		children.push_back(c);
	}
	Operator *op = NULL;
	if(type == "SELECT"){
		op = new SelectOperator(expressions, children);
	} else if(type == "PROJECT"){
		op = new ProjectionOperator(expressions, children);
	} else if(type == "TABLE"){
		op = new ScanOperator(expressions, children);
	} else if(type == "PRINT"){
		op = new PrintOperator(expressions, children);
	} else if(type == "JOIN"){
		op = new JoinOperator(expressions, children);
	}
	if(op == NULL){
		std::cout << type << " Operator not implemented" << std::endl;
		exit(-1);
	}
	return op;
}