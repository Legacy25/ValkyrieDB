#include "Operator.h"

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