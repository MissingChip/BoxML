#pragma once

#include <stdio.h>
#include "box.h"


class BoxML
{
public:
    string to_html();
    string to_string();
    
    static BoxML parse(FILE *fptr);
//private:
    vector<Box> boxv;
};
