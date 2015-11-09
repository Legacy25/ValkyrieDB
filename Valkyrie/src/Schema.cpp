#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "../include/Schema.h"
#include "../include/Operator.h"

using namespace std;
using namespace valkyrie;

string Schema::attrsVecsToCommaSepString(const vector<string>& attr, const vector<DataType>& types) const{

    string line = "", tStr = "";
    auto a = attr.begin();
    auto t = types.begin();

    while(a != attr.end() && t != types.end())
    {
        switch(t.operator*()) {
            case LONG:
            tStr = "LONG";
            break;
            case DOUBLE:
            tStr = "DOUBLE";
            break;
            case STRING:
            tStr = "STRING";
            break;
            case DATE:
            tStr = "DATE";
            break;
        }
        line.append(a.operator*()).append(" ");
        line.append(tStr).append(", ");
        a++;
        t++;
    }

    return line;
}

Schema::Schema(string tablename, string datafile)
: tablename(tablename), datafile(datafile) {
    materialized = false;
}

void Schema::addAttribute(string attr, DataType t) {
    attributes.push_back(attr);
    types.push_back(t);
}

void Schema::materialize() {
    if(materialized)
        return;

    ifstream infile(datafile);

    if(infile) {
        string line;

        while(getline(infile, line)) {

            unsigned long n = attributes.size();
            LeafValue *tuple = new LeafValue[n];
            stringstream linestream;
            linestream.str(line);
            string str;

            int i=0;
            while(getline(linestream, str, '|')) {
                switch(types.at(i)) {
                    case LONG:
                    tuple[i] = (int64_t) stol(str);
                    break;
                    case DOUBLE:
                    tuple[i] = (int64_t) stod(str);
                    break;
                    case STRING:
                    case DATE:
                    unsigned long buf_size = str.size()+1;
                    tuple[i] = (int64_t) new char[buf_size];
                    memcpy((char *)tuple[i], str.c_str(), buf_size);
                    break;
                }
                i++;
            }
            tuples.push_back(tuple);
        }
        infile.close();
    }
    else {
        return;
    }

    materialized = true;
}

bool Schema::isMaterialized() const {
    return materialized;
}

ostream& operator<<(ostream &stream, const  Schema &schema) {
    string attrs = schema.attrsVecsToCommaSepString(schema.getAttributes(), *(schema.getTypes()));
    return stream << schema.getTableName() << " : " << schema.getDataFile() << endl
    << attrs;
}

uint64_t Schema::getTuplePtr() const {
    return (uint64_t) &tuples[0];
}

const vector<DataType>* Schema::getTypes() const{
    return &types;
}

std::string Schema::getDataFile() const{
    return datafile;
}

vector<string> Schema::getAttributes() const{
    return attributes;
}

string Schema::getTableName() const{
    return tablename;
}

size_t Schema::getTupleCount() const {
    return tuples.size();
}
