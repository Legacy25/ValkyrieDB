#ifndef VALKYRIEDB_LEAFVALUE_H
#define VALKYRIEDB_LEAFVALUE_H


class longValue: public Expression {
private:
    long* data;
public:
    long* getData();
    void setData(long*);
    Value* getValue();
};


class DoubleValue: public Expression {
private:
    double* data;
public:
    double* getData();
    void setData(double*);
    Value* getValue();
};


class StringValue: public Expression {
private:
    String* data;
public:
    String* getData();
    void setData(String*);
    Value* getValue();
};


class dateValue: public Expression {
private:
    String* data;
public:
    String* getData();
    void setData(String*);
    Value* getValue();
};

class booleanValue: public Expression {
private:
    bool data;
public:
    bool* getData();
    void setData(boolean*);
    Value* getValue();
};


#endif //VALKYRIEDB_LEAFVALUE_H
