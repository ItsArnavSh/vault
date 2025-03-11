#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

typedef enum colFlags{
    PRIMARY,
    COMPRESSED,
    INDEXED
}colFlags;
typedef enum dataType{
    NUMBER,
    DECIMAL,
    TEXT,
    BLOB
}dataTypes;
typedef struct colDef{
    std::string name;
    dataType ds;
    colFlags* flags;
}colDef;
typedef struct tableDef{
    colDef* columns;
}tableDef;

std::string readFile(std::string filePath);
