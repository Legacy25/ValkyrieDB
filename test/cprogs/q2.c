#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void handle_error(char* err_mesg, int exit_code) {
    printf("%s\n", err_mesg);
    exit(exit_code);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        handle_error("Usage: q2 <path/to/data>", -1);
    }

    char* tablename = "lineitem";
    char* filename = (char*)malloc(strlen(argv[1]) + strlen(tablename) + 6);
    sprintf(filename, "%s%s.tbl", argv[1], tablename);

    FILE *f = fopen(filename, "r");
    if(!f) {
        handle_error("Couldn't open file", -1);
    }

    long ok, pk, sk, ln, q;
    double ep, disc, tax;
    char rf[1], ls[1], sd[10], cd[10], rd[10], sm[10], si[25], com[44];

    while(!feof(f)) {
        fscanf(f, "%ld|%ld|%ld|%ld|%ld|%lf|%lf|%lf|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|",
               &ok, &pk, &sk, &ln, &q, &ep, &disc, &tax, rf, ls, sd, cd, rd, sm, si, com);
        if(disc > 0.05 && disc < 0.07 && q < 24)
            printf("%ld|%ld|%ld|%ld|%ld|%lf|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%s|\n",
                   ok, pk, sk, ln, q, ep, disc, tax, rf, ls, sd, cd, rd, sm, si, com);
    }

    fclose(f);
}