//Heres a sample file for what I am looking for

/*
table Users{
    1: Id number primary;
    2: Name text;
    3: Hours decimal;
    4: Data blob compressed;
} */

#include <cmath>
#include <cstdint>
#include <string>

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

class UsersRow{
    public:
    int32_t Id;
    float_t Hours;
    std::string Name;
    unsigned char* Blob;
    UsersRow(){}//Making a new Entry
    UsersRow(unsigned char* data);//Getting the row from data
}
