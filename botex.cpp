
#include <cstdio>
#include "A/boxml.h"

int main(int argc, char* argv[])
{
    FILE* out;
    if(argc==1){
        fprintf(stderr, "No input file\n");
        exit (1);
    }
    if(argc < 3){
        out = fopen("out.tex", "w");
    }
    else{
        out = fopen(argv[2], "w");
    }
    FILE* f = fopen(argv[1], "r");
    if(f==nullptr){
        fprintf(stderr, "File not found\n");
        exit (1);
    }
    BoxML b = BoxML::parse(f);
    fprintf(out, "%s\n", b.to_latex().c_str());
    fclose(f);
    fclose(out);
}

