
#include <cstdio>
#include "A/boxml.h"

int main(int argc, char* argv[])
{
    if(argc==1){
        fprintf(stderr, "No input file\n");
        exit (1);
    }
    FILE* f = fopen(argv[1], "r");
    if(f==nullptr){
        fprintf(stderr, "File not found\n");
        exit (1);
    }
    fprintf(stdout, "Reading %s\n", argv[1]);
    BoxML b = BoxML::parse(f);
    printf("Read\n");
    //printf("%lu\n", b.boxv.size());
    printf("%s\n", b.to_string().c_str());
    printf("%s\n", b.to_html().c_str());
}
