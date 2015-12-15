//
// Created by vinayak on 12/7/15.
//

#ifndef JOIN_OPERATOR_H
#define JOIN_OPERATOR_H

#include "Operator.h"
#include "Expression.h"

namespace valkyrie {
    class JoinOperator : public Operator {
    private:
        Expression* joinClause;
        vector<ColExpression*> left;
        vector<ColExpression*> right;
        char status = 0;

        void updateExpression(Expression* exp, unordered_map<string, Expression*> lm, unordered_map<string, Expression*>rm
                , string ltable, string rtable);
        Schema* mergeSchemas(Schema* lsch, Schema* rsch);
    public:
        unordered_map<string, vector<vector<LeafValue>>> hashtable;
        JoinOperator(std::vector<std::string> expressions, std::vector<Operator*> children);
        void produce();
        void consume();
        vector<ColExpression*> getLeftJoinAttrs();
        vector<ColExpression*> getRightJoinAttrs();
    };
}


#endif
