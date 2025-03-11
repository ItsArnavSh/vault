#pragma once

typedef enum types{
    CHAR,//1
    I8,//1
    I16,//2
    I32,//4
    I64,//8
    TEXT,//Var
    BLOB//Var
}types;

typedef struct rowDataOrder{
    types *rowOrder;
}rowDataOrder;
