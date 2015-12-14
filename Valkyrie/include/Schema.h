#ifndef CDB_SCHEMA_H
#define CDB_SCHEMA_H

#include <vector>
#include <string>
#include <unordered_map>

#include "DataTypes.h"
#include "Expression.h"

#define VDB_BLOCK_SIZE 100000

using namespace std;

namespace valkyrie{
    class Schema {
    private:
        string tablename;
        string datafile;
        vector<string> attributes;
        vector<DataType> types;

        vector<LeafValue *> tuples;
        unordered_map<string, valkyrie::Expression*> colMap;

        string formatAttrName(string name) const;
        ifstream* infile;
        bool block_allocated = true;

    public:
        // Constructors
        Schema(string tablename);
        Schema(string tablename, string datafile);

        // LLVM Controller interface
        void init();
        uint64_t loadBlock();
        uint64_t close();

        // Setup
        void addAttribute(string, DataType);
        void addAttribute(string, DataType, Expression*);
        void addTuple(LeafValue *);

        // Getters, Setters
        void setTableName(string tablename);
        string getTableName() const;

        uint64_t getTuplePtr() const;
        size_t getTupleCount() const;

        void setTuples(vector<LeafValue*> ts);
        vector<LeafValue *> getTuples();

        const vector<DataType> getTypes() const;

        std::string getDataFile() const;

        vector<string> getAttributes() const;
        size_t getAttributePos(string) const;
        DataType getAttributeType(string) const;
        Expression* getAttrExpression(string colName);
        unordered_map<string, valkyrie::Expression*> getColumnMap();


        // Utilities
        std::string attrsVecsToCommaSepString(const vector<string>& attr, const vector<DataType>& types) const;
        friend ostream& operator<<(ostream &stream, const valkyrie::Schema &schema);
    };
}



#endif //CDB_SCHEMA_H
