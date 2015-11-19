#include "../include/Operator.h"

using namespace valkyrie;

Operator::Operator(std::vector<Operator*> children){
	this->children = children;
	for(int i = 0; i < children.size(); i++)
		children[i]->setParent(this);
}

Operator::~Operator(){
	delete schema;
}

std::vector<Operator*> Operator::getChildren(){
	return this->children;
}

std::string Operator::toString(){
	std::string exps = "";
	for(int i = 0; i < expressions.size(); i++){
		exps += expressions[i];
		if(i != expressions.size()-1)
			exps += ", ";
	}
	return this->type + "[" + exps + "]";
}

std::string Operator::queryPlan(){
	std::string res = "";
	std::stack<std::pair<Operator*, int> > s;
	s.push(std::make_pair(this, 0));
	while(!s.empty()){
		std::pair<Operator*, int> p = s.top();
		s.pop();
		res += "\n";
		for(int i = 0; i < p.second; i++)
			res += "\t";
		res += p.first->toString();
		std::vector<Operator*> v = p.first->getChildren();
		for(int i = 0; i < v.size(); i++)
			s.push(std::make_pair(v[i], p.second+1));
	}
	return res;
}

DataType Operator::mapType(std::string type){
	if(type == "int"  || type == "integer")
		return LONG;
	if(type == "char" || type == "varchar")
		return STRING;
	//check this
	if(type == "date")
		return DATE;
	if(type == "double" || type == "decimal")
		return DOUBLE;
	std::cerr << "Error parsing type" << std::endl;
	std::exit(-1);
}

Schema* Operator::getSchema(){
	return schema;
}

void Operator::setParent(Operator* parent){
	this->parent = parent;
}
