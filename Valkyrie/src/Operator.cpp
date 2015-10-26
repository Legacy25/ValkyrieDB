#include "Operator.h"

std::vector<Operator*> Operator::getChildren(){
	return this->children;
}

std::string Operator::toString(){
	std::string exps = "";
	for(int i = 0; i < expressions.size(); i++){
		exps += expressions[i];
		if(i != expressions.size()-1)
			exps += " ,";
	}
	std::string res = this->type + "[" + exps + "]";
	for(int i = 0; i < children.size(); i++)
		res += "\n\t" + children[i]->toString();
	return res;
}