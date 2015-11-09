#ifndef CDB_SCHEMA_H
#define CDB_SCHEMA_H

#include <vector>
#include <string>

#include "DataTypes.h"

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

    public:
        Schema(string tablename, string datafile);

        void addAttribute(string, DataType);
        void materialize();
        bool isMaterialized() const;

        // Getters
        uint64_t getTuplePtr() const;
        size_t getTupleCount() const;
        const vector<DataType>* getTypes() const;
        std::string getDataFile() const;
        vector<string> getAttributes() const;
        string getTableName() const;


        // Utilities
        friend ostream& operator<<(ostream &stream, const Schema &schema);
        std::string attrsVecsToCommaSepString(const vector<string>& attr, const vector<DataType>& types) const;
    };
}

#endif //CDB_SCHEMA_H
