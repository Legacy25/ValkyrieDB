#include "../include/ScanOperator.h"
#include "../include/Codegen.h"
#include <unordered_map>

using namespace valkyrie;

ScanOperator::ScanOperator(std::vector<std::string> expressions, std::vector<Operator*> children) : Operator(children){
	this->type = "TABLE";
	this->expressions = expressions;
	convertToSchema();
}

void ScanOperator::convertToSchema(){
	std::string delimiter = "=";
	std::size_t t = expressions[0].find(delimiter);
	std::string tableName = expressions[0].substr(0, t);
	std::string tablePath = expressions[0].substr(t+1);
	schema = new Schema(tableName, tablePath);
	for(int i = 1; i < expressions.size(); i++){
		t = expressions[i].find(delimiter);
		std::string colName = expressions[i].substr(0, t);
		std::string colType = expressions[i].substr(t+1);
		schema->addAttribute(colName, mapType(colType));
	}
}

void ScanOperator::produce(){
	for(int i = 0; i < children.size(); i++)
		assert(children[i] == NULL);
	assert(parent != NULL);
	consume();
}

void ScanOperator::consume(){
	schema->init();
	//schema->dump();
	codegen::scanConsume(*schema, parent);
}