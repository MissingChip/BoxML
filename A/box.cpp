
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
    if(!member_separator(next)){
        out.typed = true;
    }
    string* member = new string;
    bool member_val = false;
    while(true)
    {
        if(next == EOF){
            fprintf(stderr, "EOF reached unexpectedly\n");
            exit (1);
        }
        if(member_separator((char)next)){
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
                fprintf(stderr, "Member \"%s\" has no value\n", member->c_str());
                exit (1);
            }
        }
        else if(next == '='){
            if(member->size() > 0){
                out.memberv.push_back(*member);
                delete member;
                member = new string;
            }
            else{
                fprintf(stderr, "No member assigned with '='\n");
                exit (1);
            }
        }
        else if(inner_separator((char)next)){
            if(member_val){
                out.member_valv.push_back(*member);
            }
            else{
                out.idv.push_back(*member);
            }
            return out;
        }
        //accepts any weird characters that could start a variable name
        //TODO fix that^
        else{
            member->push_back((char)next);
            //printf("%c", next);
        }
        next = getc(fptr);
    }
}


bool Box::Val::member_separator(char c)
{
    if(c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f'){return true;}
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
        out += "--!>";
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
            out += idv[i];
            out.push_back(' ');
        }
        out += idv[i];
        out.push_back('"');
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
