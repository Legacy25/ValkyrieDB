#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct data{
    int val;
    char* str;
    data(int val){ this->val = val; }
    data(char* val){ this->str = val; }
};

void handle_error(char* err_mesg, int exit_code) {
    printf("%s\n", err_mesg);
    exit(exit_code);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        handle_error("Usage: q4 <path/to/data>", -1);
    }

    char* table3 = "lineitem";
    char* table1 = "customer";
    char* table2 = "orders";

    char* file1 = malloc(strlen(argv[1]) + strlen(table1) + 6);
    char* file2 = malloc(strlen(argv[1]) + strlen(table2) + 6);
    char* file3 = malloc(strlen(argv[1]) + strlen(table3) + 6);

    sprintf(filename, "%s%s.tbl", argv[1], tablename);

    FILE *f = fopen(file2, "r");
    if(!f) {
        handle_error("Couldn't open file", -1);
    }

    unordered_map<long, vector<data>> join1;
    long orderKey, custKey, shippriority;
    double totalprice;
    char orderstatus, orderdate[10], orderpriority[15], clerk[15], comment[79];
    while(!feof(f)){
        fscanf(f, "%ld|%ld|%ld|%ld|%ld|%lf|%lf|%lf|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|",
               &orderKey, &custKey, &orderstatus, &totalprice, orderdate, orderpriority,
               clerk, &shippriority, comment);
        if(totalprice > 460000.0){
            vector<data> row;
            row.push_back(data(orderKey));
            row.push_back(data(custKey));
            row.push_back(data(orderstatus));
            row.push_back(data(totalprice));
            row.push_back(data(orderdate));
            row.push_back(data(orderpriority));
            row.push_back(data(clerk));
            row.push_back(data(shippriority));
            join1.insert(make_pair<int, vector<data>>(orderKey, row));
        }
    }

    long ok, pk, sk, ln, q;
    double ep, disc, tax;
    char rf[1], ls[1], sd[10], cd[10], rd[10], sm[10], si[25], com[44];

    while(!feof(f)) {
        fscanf(f, "%ld|%ld|%ld|%ld|%ld|%lf|%lf|%lf|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|",
               &ok, &pk, &sk, &ln, &q, &ep, &disc, &tax, rf, ls, sd, cd, rd, sm, si, com);
        printf("%ld|%ld|%ld|%ld|%ld|%lf|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%s|\n",
               ok, pk, sk, ln, q, ep, disc, tax, rf, ls, sd, cd, rd, sm, si, com);
    }

    fclose(f);
}