#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>

#include "../include/Schema.h"
#include "../include/Operator.h"
#include <algorithm>

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

Schema::Schema(string tablename) {
    setTableName(tablename);
    infile = NULL;
}

Schema::Schema(string tablename, string datafile) : datafile(datafile) {
    setTableName(tablename);
    infile = NULL;
}

void Schema::init() {
    if(infile != NULL) {
        close();
    }

    assert(infile == NULL);

    infile = new ifstream(datafile);

    if(!infile) {
        cout << "Error, datafile invalid" << endl;
        exit(-1);
    }

    tuples.reserve(VDB_BLOCK_SIZE);
}

uint64_t Schema::loadBlock() {

    assert(infile != NULL);

    delete[] tuples[0];
    tuples.clear();

    assert(tuples.size() == 0);

    string line;
    uint64_t k = 0;

    unsigned long n = attributes.size();
    LeafValue *theap = new LeafValue[VDB_BLOCK_SIZE * n];

    while(getline(*infile, line)) {
        const char *l = line.c_str();
        size_t lineptr = 0;

        for(int i=0; i<n; i++) {
            size_t startlineptr = lineptr;
            while(*(l+lineptr) != '|') {
                lineptr++;
            }
            string str = line.substr(startlineptr, lineptr-startlineptr);
            switch(types.at(i)) {
                case LONG:
                    theap[k*n+i].l = stol(str);
//                    theap[k*n+i].l = strtol(str.c_str(), NULL, 10);
                    break;
                case DOUBLE:
//                    theap[k*n+i].d = stod(str);
                    theap[k*n+i].d = strtod(str.c_str(), NULL);
                    break;
                case STRING:
                case DATE:
                    unsigned long buf_size = str.size()+1;
                    theap[k*n+i].c = new char[buf_size];
                    memcpy(theap[k*n+i].c, str.c_str(), buf_size);
                    break;
            }
            lineptr++;
        }
        tuples.push_back(&theap[k*n]);
        k++;
        if(k >= VDB_BLOCK_SIZE) break;
    }

    return k;
}

uint64_t Schema::close() {
    if(infile != NULL) {
        infile->close();
        delete(infile);
        infile = NULL;
    }

    if(tuples.size() > 0) {
        if(block_allocated) {
            delete[] tuples[0];
        } else {
            for(auto i : tuples) {
                delete(i);
            }
        }
        tuples.clear();
    }

    return 0;
}

void Schema::addAttribute(string colName, DataType t) {
    string attr = formatAttrName(colName);
    attributes.push_back(attr);
    types.push_back(t);
    colMap.insert(make_pair(attr, new ColExpression(attr, attributes.size()-1, t)));
}

void Schema::addAttribute(string colName, DataType t, Expression *expression) {
    string attr = formatAttrName(colName);
    attributes.push_back(attr);
    types.push_back(t);
    colMap.insert(make_pair(attr, expression));
}



ostream& operator<<(ostream &stream, const  Schema &schema) {
    string attrs = schema.attrsVecsToCommaSepString(schema.getAttributes(), schema.getTypes());
    return stream << schema.getTableName() << " : " << schema.getDataFile() << endl << attrs;
}

void Schema::setTuples(vector<LeafValue*> tuples) {
    this->tuples = tuples;
}

vector<LeafValue *> Schema::getTuples() {
    return this->tuples;
}

uint64_t Schema::getTuplePtr() const {
    return (uint64_t) &tuples[0];
}

const vector<DataType> Schema::getTypes() const{
    return types;
}

std::string Schema::getDataFile() const{
    return datafile;
}

vector<string> Schema::getAttributes() const{
    return attributes;
}

void Schema::setTableName(string tablename) {
    std::transform(tablename.begin(), tablename.end(), tablename.begin(), ::tolower);
    this->tablename = tablename;
}

size_t Schema::getAttributePos(string attr) const {
    string colName = formatAttrName(attr);
    for(size_t i =0; i < attributes.size(); i++){
        if(attributes.at(i) == colName){
            return  i;
        }
    }
    std::cerr << "colName " << colName << " NOT Found" <<std::endl;
    exit(-1);
}

string Schema::getTableName() const{
    return tablename;
}

DataType Schema::getAttributeType(string attr) const {
    string colName = formatAttrName(attr);
    for(size_t i =0; i < attributes.size(); i++){
        if(attributes.at(i) == colName){
            return  types.at(i);
        }
    }
    std::cerr << "colName " << colName << " NOT Found" <<std::endl;
    exit(-1);
}

size_t Schema::getTupleCount() const {
    return tuples.size();
}

std::unordered_map<std::string, valkyrie::Expression*> Schema::getColumnMap(){
    return this->colMap;
};

Expression *Schema::getAttrExpression(string attr) {
    string colName = formatAttrName(attr);
    if(colMap.find(colName) == colMap.end())
        return NULL;
    return colMap[colName];
}

void Schema::addTuple(LeafValue *lv) {
    block_allocated = false;
    tuples.push_back(lv);
}

string Schema::formatAttrName(string name) const {
    //Check if the column name has table name with it, if not, then return qualified name
    if(name.find(".") == string::npos)
        return tablename + "." + name;
    return name;
}
