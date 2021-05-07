#ifndef LABEL_H
#define LABEL_H

#include <string>

class Label{
public:
    static unsigned int count;
    std::string name;
    float opacity;
    float r;
    float g;
    float b;
    unsigned int index;

    Label();
    Label(std::string name, float r, float g, float b, float opacity);
    ~Label();
};

#endif // LABEL_H
