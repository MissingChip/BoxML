#pragma once

#include <vector>
#include <string>

using namespace std;

class Box
{    
public:
    string to_latex();
    string to_html();
    string to_string();
    
    static Box parse(FILE *fptr);
    
private:
    class Val
    {
    public:
        bool typed;
        
        string to_latex();
        string closing_latex();
        string to_html();
        string closing_html();
        string to_string();
        
        static Val parse(FILE *fptr);
        static bool member_separator(char c);
        
    private:
        vector<string> idv;
        vector<string> memberv;
        vector<string> member_valv;
    };
    
    bool iscomment;
    Val val;
    vector<string> innerv;
    vector<Box> childv;
    
    static bool inner_separator(char c);
};

bool group_start(char c);

char group_end(char start);

bool group_match(char start, char test);
string parse_group(FILE* fptr, char start_c);
