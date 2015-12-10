#ifndef CDB_SCHEMA_H
#define CDB_SCHEMA_H

#include <vector>
#include <string>
#include <unordered_map>

#include "DataTypes.h"
#include "Expression.h"

using namespace std;

namespace valkyrie{
    class Schema {
    private:
        string tablename;
        string datafile;
        vector<string> attributes;
        vector<DataType> types;
        vector<LeafValue *> tuples;
        bool materialized;
        unordered_map<string, valkyrie::Expression*> colMap;

    public:
        Schema(string tablename);
        Schema(string tablename, string datafile);
        void addAttribute(string, DataType);
        void addAttribute(string, DataType, Expression*);
        void addTuple(LeafValue *);
        void materialize();
        bool isMaterialized() const;

        // Getters, Setters
        void setTableName(string tablename);
        uint64_t getTuplePtr() const;
        uint64_t getTuplePtr1() const;
        void setTuples(vector<LeafValue*> tuples);
        vector<LeafValue *> getTuples();
        size_t getTupleCount() const;
        const vector<DataType> getTypes() const;
        void setTypes(vector<DataType> types);
        std::string getDataFile() const;
        vector<string> getAttributes() const;
        string getTableName() const;
        size_t getAttributePos(string) const;
        DataType getAttributeType(string) const;
        Expression* getAttrExpression(string colName);
        unordered_map<string, valkyrie::Expression*> getColumnMap();
        void setColumnMap(unordered_map<string, valkyrie::Expression*> m);
        std::string attrsVecsToCommaSepString(const vector<string>& attr, const vector<DataType>& types) const;


        friend ostream& operator<<(ostream &stream, const valkyrie::Schema &schema);

        // Utilities
    };
}



#endif //CDB_SCHEMA_H
