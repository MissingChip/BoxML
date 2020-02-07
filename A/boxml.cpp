
#include "boxml.h"

BoxML BoxML::parse(FILE* fptr)
{
    BoxML out;
    int next;
    while(true){
        next = getc(fptr);;
        if(next == EOF){
            return out;
        }
        if(next == '['){
            out.boxv.push_back(Box::parse(fptr));
        }
    }
}

string BoxML::to_string()
{
    string out;
    for(int i=0; i<boxv.size(); i++){
        out += boxv[i].to_string();
    }
    return out;
}
string BoxML::to_html()
{
    string out;
    for(int i=0; i<boxv.size(); i++){
        out += boxv[i].to_html();
    }
    return out;
}
