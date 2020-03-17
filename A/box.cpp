
#include <stdio.h>

#include "box.h"

Box Box::parse(FILE* fptr)
{
    /**
     * Read Box data from a file, assuming '[' has already been read
     */
    Box out;
    string* inner = new string;
    fpos_t f;
    fgetpos(fptr, &f);
    int next = getc(fptr);
    bool escaped = false;
    //[# designates the box as a comment
    if(next == '#'){
        out.iscomment = true;
    }
    else{
        fsetpos(fptr, &f);
        out.iscomment = false;
        out.val = Box::Val::parse(fptr);
    }
    while(true)
    {
        next = getc(fptr);
        if(next == EOF){
            fprintf(stderr, "EOF reached unexpectedly\n");
            exit (1);
        }
        if(next == ']' && !escaped){
            out.innerv.push_back(*inner);
            delete inner;
            return out;
        }
        if(next == '[' && !escaped){
            out.innerv.push_back(*inner);
            out.childv.push_back(parse(fptr));
            delete inner;
            inner = new string;
        }
        else if(next == '\\'){
            if(escaped == true){
                inner->push_back((char)next);
                escaped = false;
            }
            else{
                escaped=true;
            }
        }
        else{
            escaped = false;
            if(next == '[' || next == ']'){
                inner->push_back('\\');
            }
            inner->push_back((char)next);
        }
    }
        
}

Box::Val Box::Val::parse(FILE* fptr)
{
    Val out;
    int next = getc(fptr);
    if(!(member_separator(next) || inner_separator(next) || group_start(next))){
        out.typed = true;
    }else{out.typed=false;};
    string* member = new string;
    bool member_val = false;
    char group = 0;
    while(true)
    {
        if(next == EOF){
            fprintf(stderr, "EOF reached unexpectedly\n");
            exit (1);
        }
        bool end = inner_separator((char)next);
        if(member_separator((char)next) || end){
            if(member->size() > 0){
                if(member_val){
                    out.member_valv.push_back(*member);
                }
                else{
                    out.idv.push_back(*member);
                }
                delete member;
                member = new string;
            }
            else if(member_val){
                exit (1);
            }
            member_val = false;
            if(end){
                return out;
            }
        }
        else if(next == '='){
            if(member->size() > 0){
                out.memberv.push_back(*member);
                delete member;
                member = new string;
                member_val = true;
            }
            else{
                fprintf(stderr, "No member assigned with '='\n");
                exit (1);
            }
        }
        //accepts any weird characters that could start a variable name
        //TODO fix that^
        else if(group_start(next)){
            *member += parse_group(fptr, next);
            if(member_val){
                out.member_valv.push_back(*member);
                member_val = false;
            }
            else{
                out.idv.push_back(*member);
            }
            delete member;
            member = new string;
        }
        else{
            member->push_back((char)next);
            //printf("%c\n", next);
        }
        next = getc(fptr);
    }
}


bool Box::Val::member_separator(char c)
{
    if(c == ',' || isspace(c)){return true;}
    return false;
}

bool Box::inner_separator(char c)
{
    if(c == ':' || c == ';'){return true;}
    return false;
}

std::string Box::to_string()
{
    string out = "[";
    if(iscomment){
        out.push_back('#');
        out += innerv[0];
    }else{
        out += val.to_string();
        out.push_back(':');
        int i;
        for(i=0;i<childv.size();i++){
            out+=innerv[i];
            out+=childv[i].to_string();
        }
        out+=innerv[i];
    }
    out.push_back(']');
    return out;
}

std::string Box::to_html()
{
    string out = "";
    if(iscomment){
        out += "<!--";
        out += innerv[0];
        out += "-->";
    }else{
        out += val.to_html();
        int i;
        for(i=0;i<childv.size();i++){
            out+=innerv[i];
            out+=childv[i].to_html();
        }
        out+=innerv[i];
        out += val.closing_html();
    }
    return out;
}

std::string Box::to_latex()
{
    string out = "";
    if(iscomment){
        out += "%";
        out += innerv[0];
        out += "\n";
    }else{
        out += val.to_latex();
        int i;
        for(i=0;i<childv.size();i++){
            out+=innerv[i];
            out+=childv[i].to_latex();
        }
        out+=innerv[i];
        out += val.closing_latex();
    }
    return out;
}

std::string Box::Val::to_string()
{
    string out;
    int i;
    if(!typed){
        out.push_back(' ');
    }
    if(idv.size()>0){
        for(i=0;i<idv.size()-1;i++){
            out += idv[i];
            out.push_back(' ');
        }
        out += idv[i];
    }
    if(memberv.size() > 0){
        for(i=0;i<memberv.size()-1;i++){
            out += memberv[i];
            out.push_back('=');
            out += member_valv[i];
            out.push_back(' ');
        }
        out += memberv[i];
        out.push_back('=');
        out += member_valv[i];
    }
    return out;
}


std::string Box::Val::to_html()
{
    string out = "<";
    int i = 0;
    if(typed){
        out += idv[0];
        i=1;
    }
    else{
        out += "div";
    }
    if(idv.size()>i){
        out += " class=\"";
        for(;i<idv.size()-1;i++){
            if(!group_start(idv[i][0])){
                out += idv[i];
                out.push_back(' ');
            }
        }
        if(!group_start(idv[i][0])){
            out += idv[i];
            out.push_back('"');
        }
    }
    if(memberv.size() > 0){
        out.push_back(' ');
        for(i=0;i<memberv.size()-1;i++){
            out += memberv[i];
            out.push_back('=');
            out += member_valv[i];
            out.push_back(' ');
        }
        out += memberv[i];
        out.push_back('=');
        out += member_valv[i];
    }
    out.push_back('>');
    return out;
}
std::string Box::Val::to_latex()
{
    string out = "\n\\";
    int i = 0;
    if(typed){
        out += idv[0];
        i=1;
    }
    else{
        out += "begin";
    }
    int total = 0;
    out.push_back('[');
    if(idv.size()>i){
        for(;i<idv.size();i++){
            if(!group_start(idv[i][0])){
                out += idv[i];
                out.push_back(',');
                total++;
            }
        }
    }
    if(memberv.size() > 0){
        for(i=0;i<memberv.size()-1;i++){
            out += memberv[i];
            out.push_back('=');
            out += member_valv[i];
            out.push_back(',');
            total++;
        }
        out += memberv[i];
        out.push_back('=');
        out += member_valv[i];
        total++;
    }
    if(total > 0){
        out[out.size()-1] = ']';
    }
    else{
        out.pop_back();
    }
    if(idv.size()>0){
        for(i=0;i<idv.size();i++){
            if(idv[i][0] == '{'){
                out += idv[i];
                break;
            }
        }
    }
    out.push_back('\n');
    return out;
}
std::string Box::Val::closing_html()
{
    string out = "</";
    if(typed){
        out += idv[0];
    }
    else{
        out += "div";
    }
    out.push_back('>');
    return out;
}
std::string Box::Val::closing_latex()
{
    string out;
    if(!typed){
        out = "\n\\end";
        if(idv.size()>0){
            for(int i=0;i<idv.size();i++){
                if(idv[i][0] == '{'){
                    out += idv[i];
                    break;
                }
            }
        }
    }
    out.push_back('\n');
    return out;
}
bool group_start(char c){
    switch(c){
        case '\'':
        case '"':
        case '{':
        case '[':
        case '<':
        case '(':
            return true;
    }
    return false;
}

char group_end(char start){
    switch(start){
        case '\'':
            return '\'';
        case '"':
            return '"';
        case '{':
            return '}';
        case '[':
            return ']';
        case '<':
            return '>';
        case '(':
            return ')';
    }
    return '\0';
}

bool group_match(char start, char test){
        if(group_end(start) == test){
            return true;
        };
        return false;
}
string parse_group(FILE* fptr, char start_c){
    string out;
    out.push_back(start_c);
    int next = getc(fptr);
    while(true)
    {
        if(next == EOF){
            exit (1);
        }
        else{
            out.push_back((char)next);
        }
        if(group_match(start_c, next)){
            return out;
        }
        next = getc(fptr);
    }
}
